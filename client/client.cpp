#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <cstring>
#include <string>
#include <iostream>

class Client { 
public:
    Client(const char* host_ip, int portno) 
        : host_ip(host_ip), portno(portno) 
    { }
    
    ~Client() {
        close(sockfd);
    }
    bool start() {
        if (!openSocket()) 
            return false;
        if (!connect())
            return false;
        return true;
    }

    bool openSocket() {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            error_handler("Error: opening socket");
            return false;
        }
        server = gethostbyname(host_ip);
        if (server == NULL) {
            fprintf(stderr,"Error: no such host\n");
            return false;
        }
        return true;
        
    }

    bool connect() {
        memset(&serv_addr, '\0', sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        memcpy(&serv_addr.sin_addr.s_addr,
               server->h_addr,
               server->h_length);
        serv_addr.sin_port = htons(portno);
        int n = ::connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
        if (n < 0) {
            error_handler("Error: connecting");
            return false;
        }
        return true;
    }
    
    void write(const std::string& str) {
        int n = ::write(sockfd, str.c_str(), str.size());
        if (n < 0) 
            error_handler("Error: writing to socket");
    }

    void read() {
        memset(buffer, '\0', buffer_size);
        // TODO: change code to non-blocking mode
        int n = ::read(sockfd, buffer, buffer_size - 1);
        if (n < 0) 
            error_handler("Error: reading from socket");
        printf("%s", buffer);
    }

private:
    void error_handler(const char *msg) {
        perror(msg);
        exit(0); // add if necessary
    }

    const size_t buffer_size = 256;
    char buffer[256];
    int sockfd, portno;
    const char *host_ip;
    struct sockaddr_in serv_addr;
    struct hostent *server;
};


int main(int argc, char *argv[]) {

    if (argc < 3 || argc > 3) {
        std::cerr << "Usage " << argv[0] << " ip port" << std::endl;
        exit(0);
    }

    Client client {argv[1], atoi(argv[2])};
    client.start();

    std::string input;
    for (;;) {
        std::cout << "Please enter the message: ";
        std::getline(std::cin, input);
        if (input == "exit")
            exit(0);
        input += "\n"; // to add \n and avoid \r\n combination
        // std::cout << "\"" << input << "\"" << std::endl; debug output
        client.write(input);
        client.read();
    }
    
    return 0;
}
