#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>

#define SERVER_PORT 8887
#define BUFFER_SIZE 1024
#define SERVER_QUEUE 20

int main()
{
    int sockServer = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockServer, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        printf("sock_server::main, Bind failed\n");
        exit(1);
    }

    if (listen(sockServer, SERVER_QUEUE) == -1) {
        printf("sock_server::main, Listen failed\n");
        exit(1);
    }

    char buffer[BUFFER_SIZE] = { 0 };
    struct sockaddr_in clientAddr;
    socklen_t length = sizeof(clientAddr);

    int conn = accept(sockServer, (struct sockaddr*)&clientAddr, &length);
    if (conn < 0) {
        printf("sock_server::main, Accept failed\n");
        exit(1);
    }
    else {
        printf("sock_server::main, clinet [%s] [%d] connect server\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    }

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int len = recv(conn, buffer, sizeof(buffer), 0);
        if (strcmp(buffer, "exit\n") == 0) {
            break;
        }

        fputs(buffer, stdout);
        send(conn, buffer, len, 0);
    }

    close(conn);
    close(sockServer);
    return 0;
}