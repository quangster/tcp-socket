#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include "constants.h"

int main(int argc, char *argv[]) {
    // Check arguments
    int server_port;
    char server_ip_addr[INET_ADDRSTRLEN];
    if (argc == 3) {
        strcpy(server_ip_addr, argv[1]);
        server_port = atoi(argv[2]);
    } else {
        strcpy(server_ip_addr, SERVER_ADDR);
        server_port = SERVER_PORT;
    }

    // Create socket file descriptor
    int client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_fd < 0) {
        perror("Error establishing socket");
        return 1;
    }

    // Set timeout
    int time_out = 10000;
    setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (char *) &time_out, sizeof(time_out));

    // Define server address
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip_addr, &server_addr.sin_addr) < 0) {
        perror("Invalid server address");
    }

    // Connect to server
    int status = connect(client_fd, (sockaddr *) &server_addr, sizeof(server_addr));
    if (status < 0) {
        perror("Error connecting to server");
        return 1;
    }

    std::cout << "=> Connected to server " << server_ip_addr << "::" << server_port << "\n";

    char buff[BUFF_SIZE];
    int ret;
    std::string message;

    while(true) {
        // Read message from stdin
        std::getline(std::cin, message);
        strcpy(buff, message.c_str());

        // Send message to server
        ret = send(client_fd, buff, strlen(buff), 0);
        if (ret < 0) {
            perror("Error sending message to server");
            break;
        }

        std::cout << "Sent: " << buff << "\n";

        // Receive message from server
        ret = recv(client_fd, buff, BUFF_SIZE, 0);
        if (ret < 0) {
            perror("Error receiving message from server");
            break;
        }
        buff[ret] = 0;
        std::cout << "Received: " << buff << "\n";
    }
    close(client_fd);
    return 0;
}
