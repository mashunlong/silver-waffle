/**
 * Copyright @ 2013 - 2014 Suntec Software(Shanghai) Co., Ltd.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are NOT permitted except as agreed by
 * Suntec Software(Shanghai) Co., Ltd.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */

#include <gtest/gtest.h>
#include <debugger/crash_handler.h>
#include <sys/system_properties.h>
#include <ncprocess/NCProcess.h>
#include <nceventsys/NCEventSystem.h>
#include <nceventsys/NCHalEventFactory.h>
#include "TestNCNetworkAccess.h"
#include "TestNCNetworkData.h"
#include "TestNCNetworkDhcp.h"
#include <ncprocess/NCDaemonProcess.h>

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


using namespace nutshell;

/**
 * TestProcess.
 *
 * test process for gtest.
 */
class TestProcess : public NCDaemonProcess
{
public:
    TestProcess(int argc, char** argv)
    : NCDaemonProcess("TestProcess", argc, argv)
    {
    }

    virtual ~TestProcess()
    {

    }

private:
    virtual VOID OnInitialize()
    {
    }

    virtual VOID OnStart()
    {
    }

    virtual VOID OnStop()
    {
    }

    virtual VOID OnDeinitialize()
    {
    }
};


// #define CLIENT_PORT 51193
#define BUFFER_SIZE 1024

int main(int argc, char* argv[])
{
    printf("Socket_client::main, Start\n");
    if (argc != 4) {
        printf("Socket_client::main, Parameters error, Program [Client Port] [Server Ip] [Server Port]\n");
        exit(1);
    }
    else {
        printf("Socket_client::main, Client port [%d], Server ip [%s], server port [%d]\n", atoi(argv[1]), argv[2], atoi(argv[3]));
    }

    int sockClient = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(atoi(argv[1]));
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockClient, (struct sockaddr*)&clientAddr, sizeof(clientAddr)) == -1) {
        printf("Socket_client::main, Bind failed\n");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(argv[2]);
    serverAddr.sin_port = htons(atoi(argv[3]));

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

/* EOF */
