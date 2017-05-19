#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/netdevice.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>

int main()
{
    int nSocket, nLen, nAttrLen;
    char szBuffer[4096];

    struct {
        struct nlmsghdr nh;
        struct ifinfomsg ifi;
    }struReq;

    struct sockaddr_nl struAddr;
    struct nlmsghdr *pstruNL;
    struct ifinfomsg *pstruIF;
    struct rtattr *pstruAttr;
    struct net_device_stats *pstruInfo;
    struct ether_addr *pstruEther;

    // creat PF_NETLINK socket with NETLINK_ROUTE protocol
    nSocket = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
    if (nSocket < 0) {
        fprintf(stderr, "creat socket failed: %s\n", strerror(errno));
        return -1;
    }

    // bind address
    memset(&struAddr, 0, sizeof(struAddr));
    struAddr.nl_family = AF_NETLINK;
    struAddr.nl_pid = getpid();  // local process ID
    struAddr.nl_groups = 0;
    if (bind(nSocket, (struct sockaddr*)&struAddr, sizeof(struAddr)) < 0) {
        fprintf(stderr, "bind socket failed: %s\n", strerror(errno));
        return -1;
    }

    // send request
    memset(&struReq, 0, sizeof(struReq));
    struReq.nh.nlmsg_len = NLMSG_LENGTH(sizeof(struReq));
    struReq.nh.nlmsg_type = RTM_GETLINK;
    struReq.nh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    struReq.ifi.ifi_family = AF_UNSPEC;

    memset(&struAddr, 0, sizeof(struAddr));
    struAddr.nl_family = AF_NETLINK;
    struAddr.nl_pid = 0;
    struAddr.nl_groups = 0;

    if (sendto(nSocket, &struReq, struReq.nh.nlmsg_len, 0, (struct sockaddr*)&struAddr, sizeof(struAddr)) < 0) {
        fprintf(stderr, "send data failed: %s\n", strerror(errno));
        return -1;
    }

    // receive message from kernel
    alarm(30);
    memset(szBuffer, 0, sizeof(szBuffer));
    while((nLen = recv(nSocket, szBuffer, sizeof(szBuffer), 0))) {
        alarm(0);
        pstruNL = (struct nlmsghdr*)szBuffer;

        while(NLMSG_OK(pstruNL, nLen)) {
            if (pstruNL->nlmsg_type == NLMSG_DONE) {
                break;
            }

            if (pstruNL->nlmsg_type == NLMSG_ERROR) {
                struct nlmsgerr *pstruError = (struct nlmsgerr*)NLMSG_DATA(pstruNL);
                fprintf(stderr, "nlmsgerr: %s\n", strerror(pstruError->error));
                break;
            }

            pstruIF = (struct ifinfomsg*)NLMSG_DATA(pstruNL);
            fprintf(stderr, "device info: [%d]\n", pstruIF->ifi_index);
            fprintf(stderr, "device type: ");

            switch(pstruIF->ifi_type) {
                case ARPHRD_ETHER:
                fprintf(stderr, "ETHER\n");
                break;
                case ARPHRD_PPP:
                fprintf(stderr, "PPP\n");
                break;
                case ARPHRD_LOOPBACK:
                fprintf(stderr, "LOOPBACK\n");
                break;
                default:
                fprintf(stderr, "UNKNOW\n");
                break;
            }

            fprintf(stderr, "device status:");
            if ((pstruIF->ifi_flags & IFF_UP) == IFF_UP) {
                fprintf(stderr, " UP");
            }

            if ((pstruIF->ifi_flags & IFF_BROADCAST) == IFF_BROADCAST) {
                fprintf(stderr, " BROADCAST");
            }

            if ((pstruIF->ifi_flags & IFF_DEBUG) == IFF_DEBUG) {
                fprintf(stderr, " DEBUG");
            }

            if ((pstruIF->ifi_flags & IFF_LOOPBACK) == IFF_LOOPBACK) {
                fprintf(stderr, " LOOPBACK");
            }

            if ((pstruIF->ifi_flags & IFF_POINTOPOINT) == IFF_POINTOPOINT) {
                fprintf(stderr, " POINTOPOINT");
            }

            if ((pstruIF->ifi_flags & IFF_RUNNING) == IFF_RUNNING) {
                fprintf(stderr, " RUNNING");
            }

            if ((pstruIF->ifi_flags & IFF_NOARP) == IFF_NOARP) {
                fprintf(stderr, " NOARP");
            }

            if ((pstruIF->ifi_flags & IFF_PROMISC) == IFF_PROMISC) {
                fprintf(stderr, " PROMISC");
            }

            if ((pstruIF->ifi_flags & IFF_NOTRAILERS) == IFF_NOTRAILERS) {
                fprintf(stderr, " NOTRAILERS");
            }

            if ((pstruIF->ifi_flags & IFF_ALLMULTI) == IFF_ALLMULTI) {
                fprintf(stderr, " ALLMULTI");
            }

            if ((pstruIF->ifi_flags & IFF_MASTER) == IFF_MASTER) {
                fprintf(stderr, " MASTER");
            }

            if ((pstruIF->ifi_flags & IFF_SLAVE) == IFF_SLAVE) {
                fprintf(stderr, " SLAVE");
            }

            if ((pstruIF->ifi_flags & IFF_MULTICAST) == IFF_MULTICAST) {
                fprintf(stderr, " MULTICAST");
            }

            if ((pstruIF->ifi_flags & IFF_PORTSEL) == IFF_PORTSEL) {
                fprintf(stderr, " PORTSEL");
            }

            if ((pstruIF->ifi_flags & IFF_AUTOMEDIA) == IFF_AUTOMEDIA) {
                fprintf(stderr, " AUTOMEDIA");
            }

            if ((pstruIF->ifi_flags & IFF_PROMISC) == IFF_PROMISC) {
                fprintf(stderr, " PROMISC");
            }

            if ((pstruIF->ifi_flags & IFF_DYNAMIC) == IFF_DYNAMIC) {
                fprintf(stderr, " DYNAMIC");
            }

            fprintf(stderr, "\n");

            pstruNL = NLMSG_NEXT(pstruNL, nLen);
        }

        memset(szBuffer, 0 , sizeof(szBuffer));
        alarm(5);
    }

    return 0;
}