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

// #define SERVER_IP "127.0.0.1"
// #define SERVER_PORT 8887

#define CLIENT_PORT 51193
#define BUFFER_SIZE 1024

int main(int argc, char* argv[])
{
    printf("Socket_client::main, Start\n");
    if (argc != 3) {
        printf("Socket_client::main, Parameters error, client [IP] [PORT]\n");
        exit(1);
    }
    else {
        printf("Socket_client::main, Server ip [%s], server port [%d]\n", argv[1], atoi(argv[2]));
    }

    int sockClient = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(CLIENT_PORT);
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockClient, (struct sockaddr*)&clientAddr, sizeof(clientAddr)) == -1) {
        printf("Socket_client::main, Bind failed\n");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockClient, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Socket_client::main, Connect server failed\n");
        exit(1);
    }

    char sendBuf[BUFFER_SIZE] = { 0 };
    char recvBuf[BUFFER_SIZE] = { 0 };

    while (fgets(sendBuf, sizeof(sendBuf), stdin) != NULL) {
        send(sockClient, sendBuf, strlen(sendBuf), 0);
        if (strcmp(sendBuf, "exit\n") == 0) {
            break;
        }

        recv(sockClient, recvBuf, sizeof(recvBuf), 0);
        fputs(recvBuf, stdout);

        memset(sendBuf, 0, sizeof(sendBuf));
        memset(recvBuf, 0, sizeof(recvBuf));
    }

    close(sockClient);
    return 0;
}


