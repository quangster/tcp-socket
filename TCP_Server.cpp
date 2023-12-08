#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include "constants.h"

std::vector<std::string> split(char* data, const char* delimiter = ENDING_DELIMITER) {
    std::vector<std::string> result;
    char* token = strtok(data, delimiter);
    while (token != NULL) {
        result.push_back(std::string(token));
        token = strtok(NULL, delimiter);
    }
    return result;
}

std::string calculate(const std::string& str) {
    int sum = 0;
    int len = str.length();
    for (const auto& c : str) {
        if(!isdigit(c)) {
            return "-Failed: String contains non-number character.";
        }
        sum += c - '0';
    }
    return "+" + std::to_string(sum);
}

int main(int argc, char *argv[]) {
    // Check arguments
    int server_port;
    if (argc == 2) {
        server_port = atoi(argv[1]);
    } else {
        server_port = SERVER_PORT;
    }

    // Create socket file descriptor
    int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd < 0) {
        perror("Error establishing socket...\n");
        return 1;
    }

    std::cout << "=> Socket server has been created...\n";

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("Error setting socket options...\n");
    }

    // Bind address to socket
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
    if (bind(server_fd, (sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("=> Error binding connection, the socket has already been established...\n");
        return -1;
    }

    std::cout << "=> Server started on port " << server_port << "\n";
    std::cout << "=> Looking for clients...\n";

    // Listen for connections
    listen(server_fd, 5);

    char buff[BUFF_SIZE];
    char client_ip[INET_ADDRSTRLEN];
    int ret;
    int client_fd;
    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_port;

    while(true) {
        // Accept connection from client
        client_fd = accept(server_fd, (sockaddr *) &client_addr, &client_addr_len);
        if (client_fd < 0) {
            perror("=> Error accepting client...\n");
            continue;
        }
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        client_port = ntohs(client_addr.sin_port);
        std::cout << "=> Connected client (" << client_ip << "::" << client_port << ")\n";

        // Communicate with client
        while(true) {
            // Receive message from client
            ret = recv(client_fd, buff, BUFF_SIZE, 0);
            if (ret < 0) {
                perror("=> Error receiving message from client...\n");
                break;
            } else if (ret == 0) {
                std::cout << "=> Disconnected client (" << client_ip << "::" << client_port << ")\n";
                break;
            }
            
            buff[ret] = 0;

            for (const auto& s : split(buff)) {
                strcpy(buff, s.c_str());
                std::cout << "[" << client_ip << "::" << client_port << "]: " << buff << "\n";
                strcpy(buff, calculate(s).c_str());
                std::cout << "Reply: " << buff << "\n";
                ret = send(client_fd, buff, strlen(buff), 0);
                if (ret < 0) {
                    perror("=> Error sending message to client...\n");
                    break;
                }
            }

        }
        close(client_fd);
    }
    close(server_fd);
    return 0;
}