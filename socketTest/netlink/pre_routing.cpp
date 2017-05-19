#include <linux/init.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/socket.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/inet.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_bridge.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>

#define ETHALEN 14
#define NF_IP_PRE_ROUTING 0 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lyh");

static struct nf_hook_ops nfho; 
unsigned int myhook_func(unsigned int hooknum,
                        struct sk_buff *_skb,
                        const struct net_device *in,
                        const struct net_device *out,
                        int (*okfn)(struct sk_buff *)) {
        struct sk_buff *skb;
        struct net_device *dev;
        struct iphdr *iph;

        int ret;
        int tot_len;    //定义IP数据报总长度
        int iph_len;    //IP头部长度
        int tcph_len;   //TCP头部长度

        struct tcphdr *tcph;
        __u16 midder_var;    //rst复位时候交换源端口和目的端口的中间变量
        struct ethhdr *ethh;
        unsigned char eth_MAC[6];
        int ip_address;

        skb = skb_copy(_skb,GFP_ATOMIC);

        if(skb == NULL) {
            return NF_ACCEPT;
        }

        iph = ip_hdr(skb);//将skb强制转换成ip结构体型

        printk(KERN_EMERG "iph->saddr=%02x\n",iph->saddr);
        printk(KERN_EMERG "iph->daddr=%02x\n",iph->daddr);


        if(iph == NULL) {
            return NF_ACCEPT;
        }

        tot_len = ntohs(iph->tot_len);

        //这里假设通过过滤IP地址，如果过滤掉的包是TCP包，那么直接发送一个rst包
        if(iph->protocol == IPPROTO_TCP) {
                iph_len = ip_hdrlen(skb);
                tcph_len = tcp_hdrlen(skb);

                //修改MAC部分
                {
                ethh = eth_hdr(skb);
                memcpy(eth_MAC,ethh->h_source,sizeof(ethh->h_source));
                memcpy(ethh->h_source,ethh->h_dest,sizeof(ethh->h_dest));
                memcpy(ethh->h_dest,eth_MAC,sizeof(ethh->h_source));
                }


                //修改IP部分
                {
                ip_address = iph->saddr;
                iph->saddr = iph->daddr;
                iph->daddr = ip_address;
                iph->check = 0;
                }


                //修改TCP部分，使得发送
                {
                tcph = tcp_hdr(skb);//将skb强制转换成tcp结构体型
                midder_var = tcph->source;
                tcph->source = tcph->dest;
                tcph->dest = midder_var;
                tcph->ack_seq = htonl(ntohl(tcph->seq)+1);
                tcph->seq = 0;
                tcph->ack = 1;
                tcph->rst = 1;
                tcph->check = 0;
                }

                {
                printk(KERN_EMERG "**************************************************\n");
                printk(KERN_EMERG "iph->saddr=%02x\n",iph->saddr);
                printk(KERN_EMERG "iph->daddr=%02x\n",iph->daddr);
                printk(KERN_EMERG "iph->protocol=%02x\n",iph->protocol);
                printk(KERN_EMERG "tcph->source=%02x\n",tcph->source);
                printk(KERN_EMERG "tcph->dest=%02x\n",tcph->dest);
                printk(KERN_EMERG "tcph->dest=%02x\n",tcph->dest);
                printk(KERN_EMERG "tcph->check=%02x\n",tcph->check);
                printk(KERN_EMERG "tcph->rst=%02x\n",tcph->rst);
                printk(KERN_EMERG "**************************************************\n");
                }

                skb->pkt_type = PACKET_OTHERHOST;//发给别人的帧(监听模式时会有这种帧)
                skb->ip_summed = CHECKSUM_NONE;//需要软件重新计算校验和
                skb->csum = csum_partial((unsigned char *)tcph, tot_len-iph_len, 0);
                tcph->check = csum_tcpudp_magic(iph->saddr,iph->daddr, tot_len - iph_len, iph->protocol, skb->csum);
                iph->check = ip_fast_csum((unsigned char *)iph,iph->ihl);

                dev = dev_get_by_name(&init_net,"eth7");
                if(dev==NULL)
                        goto out;
                skb->dev = dev;
                skb_push(skb, ETHALEN);//将skb->data指向l2层，之后将数据包通过dev_queue_xmit()发出
                ret = dev_queue_xmit(skb);
                return NF_STOLEN;

                }

        else
                {
                return NF_ACCEPT;
                }


        out:
        dev_put(dev);
        //free(skb);
        printk(KERN_EMERG "send fail\n");
        return NF_DROP;
}


static struct nf_hook_ops nfho={
        .hook           = myhook_func,
        .owner          = THIS_MODULE,
        .pf             = PF_BRIDGE,
        .hooknum        = NF_BR_PRE_ROUTING,
        .priority       = NF_BR_PRI_FIRST,
};

static int __init myhook_init(void)//__init,表示告诉gcc把该函数放在init这个section;
{
        int ret;
        ret = nf_register_hook(&nfho);
        if(ret < 0)
                {
                        printk(KERN_EMERG "can't modify skb hook!\n");
                        return ret;
                }
        return ret;
}
static void myhook_fini(void)
{
        nf_unregister_hook(&nfho);
}
module_init(myhook_init);
module_exit(myhook_fini);