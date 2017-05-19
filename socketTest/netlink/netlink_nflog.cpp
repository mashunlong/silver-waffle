#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>
#include <errno.h>
#include <linux/netfilter_ipv4/ipt_ULOG.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/icmp.h>

#define MAX_PAYLOAD 1024 // maximum payload size
#define MOD_ID 2
#define ERROR 0

//for print the information string

#if 0
#define DEBUG_INFO(...) \
        do \
        { \
                printf(__VA_ARGS__); \
        }while (0)
#else
#define WIFI_DEBUG_INFO(...) ((void)0)
#endif


static int netlink_group_mask(int group)
{
    return group ? 1 << (group - 1) : 0;
}

static void print_info(struct iphdr *iph)
{
        if(NULL == iph)
                return;
        DEBUG_INFO("srcaddr:%d.%d.%d.%d\tdestaddr:%d.%d.%d.%d\n", 
                (iph->saddr >>24)&0xff, 
                (iph->saddr >>16)&0xff, 
                (iph->saddr >> &0xff, 
                iph->saddr&0xff, 
                (iph->daddr >>24)&0xff, 
                (iph->daddr >>16)&0xff, 
                (iph->daddr >> &0xff, 
                iph->daddr&0xff);
}

int main(int argc, char* argv[])
{
        socklen_t len = 0;
        int count = 0;
    int state;
    struct sockaddr_nl src_addr, dest_addr;
    struct nlmsghdr *nlh = NULL;
        struct ulog_packet_msg *umsg = NULL;
    int sock_fd, res;
        struct iphdr *iph = NULL;
        struct icmphdr *icp = NULL;
        char buffer[2048] = {0};

        len = sizeof(struct sockaddr_nl);
    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_NFLOG);
    if(sock_fd == -1){
        DEBUG_INFO("error getting socket: %s", strerror(errno));
        return -1;
    }

    // To prepare binding

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); // self pid
    src_addr.nl_groups = netlink_group_mask(MOD_ID); // multi cast
  
    res = bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));
    if(res < 0){
        DEBUG_INFO("bind failed: %s", strerror(errno));
        close(sock_fd);
        return -1;
    }

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    if(!nlh){
        DEBUG_INFO("malloc nlmsghdr error!\n";
        close(sock_fd);
        return -1;
    }

    memset(&dest_addr,0,sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;
    dest_addr.nl_groups = 0;
    memset(nlh,0,NLMSG_SPACE(MAX_PAYLOAD));
    DEBUG_INFO("waiting received!\n";
    // Read message from kernel
        while(1)
        {
                ++count;

                state = recvfrom(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&dest_addr, &len);
                if(state < 0)
                        DEBUG_INFO("recvfrom error !\n";

                nlh = (struct nlmsghdr *)buffer;
                if(!NLMSG_OK(nlh,state))
                {
                        DEBUG_INFO("nlmsg error !\n";
                        return ERROR;
                }
                                                        
                DEBUG_INFO("recved msg type: %d\t msg len: %d\n", nlh->nlmsg_type, nlh->nlmsg_len);        
                
                umsg =(struct ulog_packet_msg *)NLMSG_DATA(nlh);
                DEBUG_INFO("mark:%ld\tindev_name: %s\toutdev_name:%s\tprefix:%s\tmac:%s\n", umsg->mark, umsg->indev_name, umsg->outdev_name, umsg->prefix, umsg->mac);
                iph = (struct iphdr *)umsg->payload;


                switch(iph->protocol)
                {
                        case IPPROTO_ICMP:
                        {                                
                                DEBUG_INFO("this is icmp protocal !\n";
                                print_info(iph);
                                DEBUG_INFO("iph->ihl:%d\n", iph->ihl);
                                icp = (void *)iph + iph->ihl*4;
//                                icp = (void *)iph + 20;
                                DEBUG_INFO("icp->type:%d\t icp->code:%d\n", icp->type, icp->code);
                                if((icp->type == 0) && (icp->code == 0))
                                        DEBUG_INFO("this is echo reply message !\n";
                                if((icp->type ==  && (icp->code == 0))
                                        DEBUG_INFO("this is echo request message !\n";
                                break;
                        }
                        case IPPROTO_TCP:
                        {
                                DEBUG_INFO("this is TCP protocal !\n";
                                print_info(iph);
                                break;
                        }
                        case IPPROTO_UDP:
                        {
                                DEBUG_INFO("this is UDP protocal !\n";
                                print_info(iph);
                                break;
                        }
                        default:
                                DEBUG_INFO("this is default protocal !\n";
                                print_info(iph);
                                break;
                }
                DEBUG_INFO("count = %d\n", count);
  }

  close(sock_fd);

    return 0;
}