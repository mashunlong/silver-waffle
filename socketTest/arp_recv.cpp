/**
 * @file arp_recv.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <net/ethernet.h>

/* 以太网帧首部长度 */
#define ETHER_HEADER_LEN sizeof(struct ether_header)
/* 整个arp结构长度 */
#define ETHER_ARP_LEN sizeof(struct ether_arp)
/* 以太网 + 整个arp结构长度 */
#define ETHER_ARP_PACKET_LEN ETHER_HEADER_LEN + ETHER_ARP_LEN
/* IP地址长度 */
#define IP_ADDR_LEN 4

void err_exit(const char *err_msg)
{
    perror(err_msg);
    exit(1);
}

int main(void)
{
    struct ether_arp *arp_packet;
    char buf[ETHER_ARP_PACKET_LEN];
    int sock_raw_fd, ret_len, i;

    if ((sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) == -1)
        err_exit("socket()");

    while (1)
    {
        printf("WHILE START\n");
        system("date -u");

        fd_set readfds;

        FD_ZERO(&readfds);
        FD_SET(sock_raw_fd, &readfds);

        int maxfds = sock_raw_fd + 1;

        struct timeval timeo;
        timeo.tv_sec = 1;
        timeo.tv_usec = 0;

        int n = select(maxfds, &readfds, NULL, NULL, &timeo);
        system("date -u");
        if (n <= 0) {
            printf("SELECT ERROR \n");
            close(sock_raw_fd);
            return -1;
        }

        bzero(buf, ETHER_ARP_PACKET_LEN);
        ret_len = recv(sock_raw_fd, buf, ETHER_ARP_PACKET_LEN, 0);
        printf("RET LEN %d \n", ret_len);

        if (ret_len > 0)
        {
            /* 剥去以太头部 */
            arp_packet = (struct ether_arp *)(buf + ETHER_HEADER_LEN);

            printf("ARP OP %d \n", ntohs(arp_packet->arp_op));

            /* arp操作码为2代表arp应答 */
            if (ntohs(arp_packet->arp_op) == 2)
            {
                printf("==========================arp replay======================\n");
                char fromIp[16] = { 0 };
                snprintf(fromIp, sizeof(fromIp)-1, "%u.%u.%u.%u", arp_packet->arp_spa[0], arp_packet->arp_spa[1], 
                    arp_packet->arp_spa[2], arp_packet->arp_spa[3]);
                printf("from ip : %s \n", fromIp);
                // printf("from ip:");
                // for (i = 0; i < IP_ADDR_LEN; i++)
                //     printf(".%u", arp_packet->arp_spa[i]);
                // printf("\nfrom mac");
                // for (i = 0; i < ETH_ALEN; i++)
                //     printf(":%02x", arp_packet->arp_sha[i]);
                // printf("\n");
            }
        }
    }

    close(sock_raw_fd);
    return 0;
}