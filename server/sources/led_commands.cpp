/* The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include <list>
#include <string>
#include <iostream>
#include <memory>

// class the main responsibility is manipulate led 
class LedManager {
public:
    enum class LedState {
        on,
        off,
        unknownState, 
    };
    enum class LedColor {
        red,
        green,
        blue,
        unknownColor, 
    };

    LedManager(LedState initState = LedState::off, 
               LedColor initColor = LedColor::red,
               int initLedRate = 1) 
    : state(initState), 
      color(initColor), 
      refrashRateHz(1) 
    { }

    const LedState& getLedState() const { return state; }
    const LedColor& getLedColor() const { return color; }
    const int getLedRate() const {return refrashRateHz; }
    const std::string getLedStateString() const { 
        std::string str;
        switch (state) {
            case LedState::on:
                str = "on";
                break;
            case LedState::off:
                str = "off";
                break;
        }
        return str;
    }
    
    // TODO: change enum class to sophisticated data structure (maybe map or unordered_map<enum, string>?)
    const std::string getLedColorString() const { 
        std::string str;
        switch (color) {
            case LedColor::red:
                str = "red";
                break;
            case LedColor::green:
                str = "green";
                break;
            case LedColor::blue:
                str = "blue";
                break;
        }
        return str;
    }

    LedState string2LedState(const std::string& str) const {
        if (str == "on") {
            return LedState::on;
        } else if (str == "off") {
            return LedState::off;
        }
        return LedState::unknownState;
    }

    LedColor string2LedColor(const std::string& str) const {
        if (str == "red") {
            return LedColor::red;
        } else if (str == "green") {
            return LedColor::green;
        } else if (str == "blue") {
            return LedColor::blue;
        }
        return LedColor::unknownColor;
    }

    void setLedState(LedState newState) {
        if (newState != state) {
            state = newState;
        }
    }

    void setLedColor(LedColor newColor) {
        if (newColor != color) {
            color = newColor;
        }
    }

    void setLedRate(int newRateHz) {
        if (newRateHz != refrashRateHz) {
            refrashRateHz = newRateHz;
        }
    }

private:
    // led switcher timer with lambda
    LedState state;
    LedColor color;
    int refrashRateHz;
};

// abstract class for basic command
class BaseCommand {
public:
    BaseCommand(LedManager* ledManager) : ledManager(ledManager) { }
    virtual ~BaseCommand() {}
    // must return true if the command executed successfully, otherwise false
    virtual bool run(const std::string& input, std::string& output) = 0;
protected:
    LedManager* ledManager;
};

// set-led-state
// get-led-state
// set-led-color
// get-led-color
// set-led-rate
// get-led-rate
class GetLedStateCommand : public BaseCommand {
public:
    GetLedStateCommand(LedManager* ledManager) : BaseCommand(ledManager) { }
    virtual ~GetLedStateCommand() { }
    virtual bool run(const std::string& input, std::string& output) override {
        // verify input
        // if (input.substr(0, 14) != "get-led-state\n")
        if (input != "get-led-state\n")
            return false;
        // get led state and set output
        output = "OK " + ledManager->getLedStateString();
        return true;
    }
};

class GetLedColorCommand : public BaseCommand {
public:
    GetLedColorCommand(LedManager* ledManager) : BaseCommand(ledManager) { }
    virtual ~GetLedColorCommand() { }
    virtual bool run(const std::string& input, std::string& output) override {
        bool commandWasExecuted = false;
        // verify input
        if (input != "get-led-color\n")
            return false;
        // get led state and set output
        output = "OK " + ledManager->getLedColorString();
        return true;
    }
};

class GetLedRateCommand : public BaseCommand {
public:
    GetLedRateCommand(LedManager* ledManager) : BaseCommand(ledManager) { }
    virtual ~GetLedRateCommand() { }
    virtual bool run(const std::string& input, std::string& output) override {
        bool commandWasExecuted = false;
        // verify input
        if (input != "get-led-rate\n")
            return false;
        output = "OK " + std::to_string(ledManager->getLedRate());
        return true;
    }
};

class SetLedStateCommand : public BaseCommand {
public:
    SetLedStateCommand(LedManager* ledManager) : BaseCommand(ledManager) { }
    virtual ~SetLedStateCommand() { }
    virtual bool run(const std::string& input, std::string& output) override {
        bool commandWasExecuted = false;
        // verify input
        if (input.substr(0, cmdName.size()) != cmdName)
            return false;
        LedManager::LedState state = ledManager->string2LedState(input.substr(cmdName.size(), input.size() - cmdName.size() - 1));
        if (state == LedManager::LedState::unknownState) {
            output = "FAILED: wrong argument (state name)";
        } else {
            ledManager->setLedState(state);
            output = "OK";
        }
        return true;
    }
private:
    inline static const std::string cmdName = "set-led-state"; // remove inline if compiled with c++14 or lower
};

class SetLedColorCommand : public BaseCommand {
public:
    SetLedColorCommand(LedManager* ledManager) : BaseCommand(ledManager) { }
    virtual ~SetLedColorCommand() { }
    virtual bool run(const std::string& input, std::string& output) override {
        bool commandWasExecuted = false;
        // verify input
        if (input.substr(0, cmdName.size()) != cmdName)
            return false;
        LedManager::LedColor color = ledManager->string2LedColor(input.substr(cmdName.size(), input.size() - cmdName.size() - 1)); // -1 to remove last \n symbols
        if (color == LedManager::LedColor::unknownColor) {
            output = "FAILED: wrong argument (color name)";
        } else {
            ledManager->setLedColor(color);
            output = "OK";
        }
        return true;
    }
private:
    inline static const std::string cmdName = "set-led-color"; // remove inline if compiled with c++14 or lower
};

class SetLedRateCommand : public BaseCommand {
public:
    SetLedRateCommand(LedManager* ledManager) : BaseCommand(ledManager) { }
    virtual ~SetLedRateCommand() { }
    virtual bool run(const std::string& input, std::string& output) override {
        bool commandWasExecuted = false;
        // verify input
        if (input.substr(0, cmdName.size()) != cmdName)
            return false;
        int refreshRate = atoi(input.substr(cmdName.size(), input.size() - cmdName.size() - 1).c_str()); 
        if (refreshRate > 5 || refreshRate < 0) {
            output = "FAILED: wrong argument (led rate should be between 0 and 5)";
        } else {
            ledManager->setLedRate(refreshRate);
            output = "OK";
        }
        return true;
    }
private:
    inline static const std::string cmdName = "set-led-rate";
};


// custom implementation of 'chain of responsibility' pattern to provide easier way adding and modifying commands
class CommandContainer {
public:
    CommandContainer(const std::list<BaseCommand*>& commandsList = {}) : commandsList(commandsList) { }   

    virtual ~CommandContainer() {
        commandsList.remove_if( [] (BaseCommand* elem) { delete elem; return true; } ); // use this instead of clear() won't clean memory
    }

    bool execute(const std::string& argument, std::string& output) {
        bool cmdWasExecuted = false;
        for (auto cmd = commandsList.begin(); cmd != commandsList.end(); ++cmd) {
            cmdWasExecuted = (*cmd)->run(argument, output);
            if (cmdWasExecuted) break;
        }
        return cmdWasExecuted;
    }

private:
    std::list<BaseCommand*> commandsList;
};

class Server {
public:
    Server(int portno, CommandContainer& commands) : portno(portno), commands(commands) {
        // create a socket
        // socket(int domain, int type, int protocol)
        sockfd =  socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
            error("ERROR opening socket");

        // clear address structure
        bzero((char *) &serv_addr, sizeof(serv_addr));


        /* setup the host_addr structure for use in bind call */
        // server byte order
        serv_addr.sin_family = AF_INET;  

        // automatically be filled with current host's IP address
        serv_addr.sin_addr.s_addr = INADDR_ANY;  

        // convert short integer value for port must be converted into network byte order
        serv_addr.sin_port = htons(portno);
    }

    // additional function
    // help to avoid get error in constructor
    bool portBind() {
        // bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
        // bind() passes file descriptor, the address structure, 
        // and the length of the address structure
        // This bind() call will bind  the socket to the current IP address on port, portno
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            error("ERROR on binding");
            return false;
        }
        return true;
    }

    bool connect() {
        printf("Waiting for connection\n");
        // This listen() call tells the socket to listen to the incoming connections.
        // The listen() function places all incoming connection into a backlog queue
        // until accept() call accepts the connection.
        // Here, we set the maximum size for the backlog queue to 5.
        listen(sockfd, 5);

        // The accept() call actually accepts an incoming connection
        clilen = sizeof(cli_addr);

        // This accept() function will write the connecting client's address info 
        // into the the address structure and the size of that structure is clilen.
        // The accept() returns a new socket file descriptor for the accepted connection.
        // So, the original socket file descriptor can continue to be used 
        // for accepting new connections while the new socker file descriptor is used for
        // communicating with the connected client.
        newsockfd = accept(sockfd, 
                    (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0) {
            error("ERROR on accept");
            return false;
        }

        printf("server: got connection from someServer port %d\n",
            /*inet_ntoa(cli_addr.sin_addr),*/ ntohs(cli_addr.sin_port));
        return true;
    }

    bool listenServer() {
        // This send() function sends the 13 bytes of the string to the new socket
        // send(newsockfd, "Hello, world!\n", 13, 0);

        bzero(buffer, 256);

        // if receive buffer TODO: change this to non-blocking reading
        if (read(newsockfd, buffer, 255) < 0) {
            error("ERROR reading from socket");
            return false;
        }
        
        printf("DEBUG: Here is the message: |%s|", buffer);
        cmdOutput = "";
        // if suitable command found
        if (commands.execute(buffer, cmdOutput)) {
            cmdOutput += "\n";
            printf("%s", cmdOutput.c_str());
            send(newsockfd, cmdOutput.c_str(), cmdOutput.length(), 0);
            return true;
        }
        cmdOutput = "Command is not recognized. Your command: " + std::string(buffer);
        send(newsockfd, cmdOutput.c_str(), cmdOutput.length(), 0);
        return true;
    }

    ~Server() {
        close(newsockfd);
        close(sockfd);
    }

private: 
    inline void error(const char *msg) {
        perror(msg); 
    }

    int newsockfd; // change this to std::list multiple connection is needed
    int sockfd;    // 
    int portno;    // server port number

    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    CommandContainer& commands;
    std::string cmdOutput;
};

int main(int argc, char *argv[]) {
    // check input arguments
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    } else if (argc > 3) {
        fprintf(stderr,"ERROR, too many arguments\n");
        exit(1);
    }

    LedManager* ledManager = new LedManager();
    // create commands container
    CommandContainer c { 
        {new GetLedStateCommand(ledManager), new GetLedColorCommand(ledManager), new GetLedRateCommand(ledManager),
         new SetLedStateCommand(ledManager), new SetLedColorCommand(ledManager), new SetLedRateCommand(ledManager) } };
    // create server object
    Server serv {atoi(argv[1]), c};
    serv.portBind();

    while (true) {
        serv.connect();
        while (serv.listenServer()) {  }
        exit(0);
    }
    return 0; 
}