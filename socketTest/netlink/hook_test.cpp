#ifndef __KERNEL__
#define __KERNEL__
#endif  
#ifndef MODULE
#define MODULE
#endif 

// #include   <linux/module.h>
#include   <linux/kernel.h>
// #include   <linux/skbuff.h>
#include   <sys/socket.h>
#include   <linux/ip.h>
#include   <linux/tcp.h>
#include   <linux/icmp.h>
#include   <linux/netdevice.h>
#include   <netinet/in.h>
#include   <linux/netfilter.h>
#include   <linux/netfilter_ipv4.h>
#include   <linux/if_arp.h>
#include   <linux/if_ether.h>
#include   <linux/if_packet.h>

//struct holding set of hook function options
static struct nf_hook_ops nfho;

//function to be called by hook  
unsigned int myhook_func(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *))  
{
    //drops the packet 
    return NF_DROP;
}

//Called when module loaded using 'insmod'  
int init_module()
{
    nfho.hook = myhook_func;                     //function to call when conditions below met  
    nfho.hooknum = NF_IP_PRE_ROUTING;            //called right after packet recieved, first hook in Netfilter  
    nfho.pf = PF_INET;                           //IPV4 packets  
    nfho.priority = NF_IP_PRI_FIRST;             //set to highest priority over all other hook functions  
    nf_register_hook(&nfho);                     //register hook  
    return 0;                                    //return 0 for success  
}

//Called when module unloaded using 'rmmod'  
void cleanup_module()  
{
    nf_unregister_hook(&nfho);
} 
