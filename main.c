#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>

int parse_port(char *argv) {
    int port = -1;
    
    for (int i = 0; i < strlen(argv); i++) {
        if(isdigit(argv[i]) == 0) {
            println("Invalid port number.");
            exit(EXIT_FAILURE);
        }
    }
    port = atoi(argv);

    if (port > 65535 || port < 1) {
        println("Invalid port number.");
        exit(EXIT_FAILURE);
    }

    return port;
}

void println(char *s) {
    printf("%s\n", s);
}

int main(int argc, char *argv[]) {
    int listen_fd, new_socket, port, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *pass;
    
    if (argc != 3) {
        println("argv");

        return 1;
    }

    port = parse_port(argv[1]);
    pass = argv[2];

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        println("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        println("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );

    if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        println("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(listen_fd, 3) < 0) {
        println("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(listen_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        println("accept");
        exit(EXIT_FAILURE);
    }

    println("started");
    send(new_socket, "password\n", 9, 0);
    valread = read(new_socket, buffer, 1024);
    println(buffer);
    buffer[strlen(buffer)-1] = 0;
    if (strcmp(buffer, pass) != 0) {
        println("kicked");
        send(new_socket, "kicked\n", 7, 0);
        exit(EXIT_FAILURE);
    }
    send(new_socket, "accepted\n", 9, 0);
    
    return 0;
}
