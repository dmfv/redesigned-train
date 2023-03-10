#include <boost/asio.hpp>
#include <optional>
#include <queue>
#include <unordered_set>
#include <iostream>

#include "led_commands.h"

namespace io = boost::asio;
using tcp = io::ip::tcp;
using error_code = boost::system::error_code;
using namespace std::placeholders;

using message_handler = std::function<void (const std::string&, std::string&)>; 
using error_handler = std::function<void (error_code&)>;

class Session : public std::enable_shared_from_this<Session> {
public:

    Session(tcp::socket&& socket)
    : socket(std::move(socket))
    {
    }

    ~Session() {
        close_connection();
    }


    void start(const message_handler& on_message, const error_handler& on_error) {
        this->on_message = on_message;
        this->on_error   = on_error;
        error_code error;
        endpoint = socket.remote_endpoint(error);
        std::cout << endpoint << ": connected" << std::endl;
        async_read();
    }

    inline void post(const std::string& message) {
        outgoing.push(message);
        async_write();
    }

    void close_connection() {
        close_flag = true;
        post("Server closed connection\n");

        size_t counter = 0; // counter to avoid infinite loop
        while (!socket_closed || ++counter < 50) { // 1 second timeout
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }

private:

    void async_read() {
        io::async_read_until(socket, streambuf, "\n", std::bind(&Session::on_read, shared_from_this(), _1, _2));
    }

    void on_read(error_code error, std::size_t bytes_transferred) {
        if(!error) {
            std::stringstream message;
            message << std::istream(&streambuf).rdbuf();
            std::string callbackInput = message.str(), callbackOutput;

            std::cout << endpoint << ": " << callbackInput; // logging
            streambuf.consume(bytes_transferred);
            on_message(callbackInput, callbackOutput);
            callbackOutput += "\n";
            post(callbackOutput);
            async_read();
        }
        else {
            socket.close(error);
            socket_closed = true;
            if (error.message() == "The operation completed successfully") {
                std::cout << endpoint << ": disconnected" << std::endl;
            } else {
                on_error(error);
            }
        }
    }

    void async_write() {
        io::async_write(socket, io::buffer(outgoing.front()), std::bind(&Session::on_write, shared_from_this(), _1, _2));
    }

    void on_write(error_code error, std::size_t bytes_transferred) {
        if (!error) {
            outgoing.pop();
            if(!outgoing.empty())
            {
                async_write();
            }
        // if got close connection flag doesn't matter error exists or not
        } else if (close_flag || error) {
            socket.close(error);
            socket_closed = true;
            std::cout << "error " << error << std::endl;
            if (error == boost::asio::error::eof) {
                std::cout << "Client disconnected" << std::endl;
            } else {
                on_error(error);
            }
        }
    }

    tcp::socket socket; 
    tcp::endpoint endpoint;
    io::streambuf streambuf; 
    bool close_flag = false;
    bool socket_closed = false;
    std::queue<std::string> outgoing;
    // callbacks
    message_handler on_message;
    error_handler on_error;
};

class Server {
public:

    Server(io::io_context& io_context, 
           std::uint16_t port,
           message_handler on_message,
           error_handler on_error)
    : io_context(io_context),
      acceptor(io_context, tcp::endpoint(tcp::v4(), port)),
      on_message(on_message),
      on_error(on_error)
    {
        std::cout << "Running server on port " << port << std::endl;
    }

    void async_accept() {
        socket.emplace(io_context);

        acceptor.async_accept(*socket, [&] (error_code error)
        {
            auto client = std::make_shared<Session>(std::move(*socket));
            client->start(on_message, on_error);
            clients.insert(client);

            async_accept();
        });
    }
    
    void stop_Server() {
        for(auto& client : clients)
        {
            client->close_connection();
        }
    }

private:
    io::io_context& io_context;
    tcp::acceptor acceptor;
    std::optional<tcp::socket> socket;
    std::unordered_set<std::shared_ptr<Session>> clients;
    // callbacks for Sessions
    message_handler on_message;
    error_handler on_error;
};

int main(int argc, char* argv[]) {
    std::uint16_t server_port = 15001;
    if (argc > 1) 
        server_port = atoi(argv[1]);
    
    std::shared_ptr<LedManager> lm = std::make_shared<LedManager>();
    std::unordered_set<std::unique_ptr<BaseCommand>> cmds;
    cmds.insert(std::make_unique<GetLedStateCommand>(lm));  
    cmds.insert(std::make_unique<SetLedStateCommand>(lm));
    cmds.insert(std::make_unique<GetLedColorCommand>(lm));
    cmds.insert(std::make_unique<SetLedColorCommand>(lm));
    cmds.insert(std::make_unique<GetLedRateCommand>(lm));
    cmds.insert(std::make_unique<SetLedRateCommand>(lm));

    std::unique_ptr<CommandContainer> CC = std::make_unique<CommandContainer>(std::move(cmds));
    io::io_context io_context;
    Server srv(io_context, server_port, [&] (const std::string& in, std::string& out) {CC->execute(in, out);},
                                  [ ] (error_code& error) {std::cout << "Error occurred: "    << error.message() << std::endl;});
    srv.async_accept();
    io_context.run();
    return 0;
}