#include <boost/asio.hpp>
#include <optional>
#include <queue>
#include <unordered_set>


#include <iostream>

namespace io = boost::asio;
using tcp = io::ip::tcp;
using error_code = boost::system::error_code;
using namespace std::placeholders;

using message_handler = std::function<void (std::string)>; // callback function 
// using message_handler = std::function<void (std::string, std::string&)>; // callback function 
using error_handler = std::function<void (error_code&)>;

class session : public std::enable_shared_from_this<session> {
public:

    session(tcp::socket&& socket)
    : socket(std::move(socket))
    {
    }

    ~session() {
        close_connection();
    }


    void start(message_handler&& on_message, error_handler&& on_error) {
        this->on_message = std::move(on_message);
        this->on_error = std::move(on_error);
        error_code error;
        std::cout << "Connected from client: " << socket.remote_endpoint(error) << std::endl;
        async_read();
    }

    inline void post(const std::string& message) {
        outgoing.push(message);
        async_write();
    }

    void close_connection() {
        close_flag = true;
        post("Server closed connection\n");
    }

private:

    void async_read() {
        io::async_read_until(socket, streambuf, "\n", std::bind(&session::on_read, shared_from_this(), _1, _2));
    }

    void on_read(error_code error, std::size_t bytes_transferred) {
        if(!error) {
            std::stringstream message;
            message << socket.remote_endpoint(error) << ": " << std::istream(&streambuf).rdbuf();
            streambuf.consume(bytes_transferred);
            on_message(message.str());
            async_read();
        }
        else {
            socket.close(error);
            on_error(error);
        }
    }

    void async_write() {
        io::async_write(socket, io::buffer(outgoing.front()), std::bind(&session::on_write, shared_from_this(), _1, _2));
    }

    void on_write(error_code error, std::size_t bytes_transferred) {
        // if no errors
        if (!error) {
            outgoing.pop();
            if(!outgoing.empty())
            {
                async_write();
            }
        // if got close connection flag doesn't matter error exists or not
        } else if (close_flag) {
            socket.close(error);
            if (error) {
                on_error(error);
            }
        } else if (error) {
            socket.close(error);
            on_error(error);
        }
    }

    tcp::socket socket; 
    io::streambuf streambuf; 
    bool close_flag = false;
    std::queue<std::string> outgoing;
    // callbacks
    message_handler on_message;
    error_handler on_error;
};

class server {
public:

    server(io::io_context& io_context, std::uint16_t port)
    : io_context(io_context)
    , acceptor  (io_context, tcp::endpoint(tcp::v4(), port))
    {
    }

    void async_accept() {
        socket.emplace(io_context);

        acceptor.async_accept(*socket, [&] (error_code error)
        {
            auto client = std::make_shared<session>(std::move(*socket));
            client->start([] (std::string m)     {std::cout << "On message handler " << m;}, 
                          [] (error_code& error) {std::cout << "Error occurred: "    << error.message() << std::endl;});
            client->post("Successfully connected to server\n\r"); 
            clients.insert(client);

            async_accept();
        });
    }
    
    void stop_server() {
        for(auto& client : clients)
        {
            client->close_connection();
        }
    }
    // notify all clients
    // void post(std::string const& message)
    // {
    //     for(auto& client : clients)
    //     {
    //         client->post(message);
    //     }
    // }

private:

    io::io_context& io_context;
    tcp::acceptor acceptor;
    std::optional<tcp::socket> socket;
    std::unordered_set<std::shared_ptr<session>> clients;
};

int main()
{
    io::io_context io_context;
    server srv(io_context, 15001);
    srv.async_accept();
    io_context.run();
    return 0;
}