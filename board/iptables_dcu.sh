#!/bin/sh

#Define
IF_USBNET="sl+"
IF_USBNET2="usb+"
IF_WLAN="wlan0"
IF_P2P="p2p-wlan0-+"
IF_DCM="ppp0"
IF_BTSPP="sl0"
IF_ETHERNET="eth0"

PORT_TSL1="80"
PORT_TSL2="443"
PORT_TSL3="5540"
PORT_TSL4="6980"
PORT_TSL5="7050"
PORT_TSL6="31000"
PORT_TSL7="31001"

PORT_UPNP="1900:1901"
PORT_RTP1="8000"
PORT_RTP2="9000"
PORT_DHCP="67:68"
PORT_DNS="53"
PORT_RTCP_SERVER="15550:15560"
PORT_HDCP2_AUTHENTICATION="36918"
PORT_MDNS="5353"

PORT_DEBUG_SERVICE="11130:11139"
PORT_DEBUG_ADB1="5555"
PORT_DEBUG_ADB2="6666"
PORT_DEBUG_AVC_TOOL="11121"
PORT_DEBUG_GDB="5055"
PORT_DEBUG_BARGEIN_TUNE="2012"

PORT_DEBUG_AVC="33280"
PORT_DEBUG_WINDOWMANAGER="3553"
PORT_DEBUG_NAVI_AT1="5399"
PORT_DEBUG_NAVI_AT2="5391:5394"

PORT_TESTMODE_ALITE="7101"

IP_MEU="192.168.128.130"
IP_WHS="10.0.2.0/24"

DEFAULT_MODE=1
DCU_MEU_MODE=3
DCU_ONLY_MODE=4
DEFAULT_DCU_MEU_MODE=5
DEFAULT_DCU_ONLY_MODE=6
CLEAR_DEFAULT_MODE=7
CLEAR_NCM_MODE=8
INIT_FIREWALL_MOED=9

checkCmdReturn()
{
    exit_status=$1
    if [ $exit_status -ne 0 ] ; then
        exit $exit_status
    fi
}

mode=$1
#Run script for default config
if [ ${mode} -eq ${DEFAULT_MODE} -o ${mode} -eq ${DEFAULT_DCU_MEU_MODE} -o ${mode} -eq ${DEFAULT_DCU_ONLY_MODE} ] ; then
#Initialize:
iptables -F
checkCmdReturn $?

iptables -X
checkCmdReturn $?

#Default:
iptables -P INPUT DROP
checkCmdReturn $?

iptables -P OUTPUT ACCEPT
checkCmdReturn $?

iptables -P FORWARD DROP
checkCmdReturn $?

#Accept local packet:
iptables -A INPUT -i lo -j ACCEPT
checkCmdReturn $?

#DEFENDING chain:
iptables -N DEFEND_LIST
checkCmdReturn $?

#接続総数を制限する
iptables -A DEFEND_LIST -p tcp --syn -m connlimit --connlimit-above 1024 -j DROP
checkCmdReturn $?

#接続頻度を制限する
iptables -A DEFEND_LIST -p tcp --syn -m limit --limit 8/s -i $IF_USBNET -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p tcp --syn -m limit --limit 8/s -i $IF_WLAN -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p tcp --syn -m limit --limit 8/s -i $IF_P2P -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p tcp --syn -m limit --limit 8/s -i $IF_DCM -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p tcp --syn -m limit --limit 8/s -i $IF_BTSPP -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p tcp --syn -m limit --limit 8/s -i $IF_ETHERNET -j ACCEPT
checkCmdReturn $?

#単位時間内に制限を超えたパッケージを要求したクライアントを ○分間ブロックする
#No limit in MirrorLink
iptables -A DEFEND_LIST -p tcp -i $IF_USBNET2 -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p udp -i $IF_USBNET2 -j ACCEPT
checkCmdReturn $?

#Support 35Mbps for segments with MTU in ETHERNET
iptables -A DEFEND_LIST -p tcp -i $IF_ETHERNET -m limit --limit 3000/s --limit-burst 5000 -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p udp -i $IF_ETHERNET -m limit --limit 3000/s --limit-burst 5000 -j ACCEPT
checkCmdReturn $?

#Support 40Mbps for segments with MTU in WLAN
iptables -A DEFEND_LIST -p tcp -i $IF_WLAN -m limit --limit 3500/s --limit-burst 5500 -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p udp -i $IF_WLAN -m limit --limit 3500/s --limit-burst 5500 -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p tcp -i $IF_P2P -m limit --limit 3500/s --limit-burst 5500 -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p udp -i $IF_P2P -m limit --limit 3500/s --limit-burst 5500 -j ACCEPT
checkCmdReturn $?

#Support 100Mbps for segments with MTU in BT
iptables -A DEFEND_LIST -p tcp -i $IF_DCM -m limit --limit 8700/s --limit-burst 10000 -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p udp -i $IF_DCM -m limit --limit 8700/s --limit-burst 10000 -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p tcp -i $IF_BTSPP -m limit --limit 90/s --limit-burst 110 -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p udp -i $IF_BTSPP -m limit --limit 90/s --limit-burst 110 -j ACCEPT
checkCmdReturn $?

#Support 100Mbps for segments with MTU in USBNET
iptables -A DEFEND_LIST -p tcp -i $IF_USBNET -m limit --limit 8700/s --limit-burst 10000 -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p udp -i $IF_USBNET -m limit --limit 8700/s --limit-burst 10000 -j ACCEPT
checkCmdReturn $?

#Pingを受け付ける頻度を制限する
iptables -A DEFEND_LIST -p icmp --icmp-type echo-request -m limit --limit 1/s -i $IF_USBNET -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p icmp --icmp-type echo-request -m limit --limit 1/s -i $IF_WLAN -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p icmp --icmp-type echo-request -m limit --limit 1/s -i $IF_P2P -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p icmp --icmp-type echo-request -m limit --limit 1/s -i $IF_DCM -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p icmp --icmp-type echo-request -m limit --limit 1/s -i $IF_BTSPP -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p icmp --icmp-type echo-request -m limit --limit 1/s -i $IF_ETHERNET -j ACCEPT
checkCmdReturn $?

iptables -A DEFEND_LIST -p icmp ! --icmp-type echo-request -j ACCEPT
checkCmdReturn $?

#INPUT chain for TCP:
iptables -N TCP_INPUT
checkCmdReturn $?

#Accept established/related packet:
iptables -A TCP_INPUT -p tcp -m state --state ESTABLISHED,RELATED -j DEFEND_LIST
checkCmdReturn $?

#Accept TSL packet:
iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_TSL1 -j DEFEND_LIST
checkCmdReturn $?

iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_TSL2 -j DEFEND_LIST
checkCmdReturn $?

iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_TSL3 -j DEFEND_LIST
checkCmdReturn $?

iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_TSL4 -j DEFEND_LIST
checkCmdReturn $?

iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_TSL5 -j DEFEND_LIST
checkCmdReturn $?

#Accept UPnP packet:
iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_UPNP -j DEFEND_LIST
checkCmdReturn $?

#Accept RTP packet:
iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_RTP1 -i $IF_P2P -j DEFEND_LIST
checkCmdReturn $?

iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_RTP2 -i $IF_P2P -j DEFEND_LIST
checkCmdReturn $?

iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_RTP1 -i $IF_USBNET2 -j DEFEND_LIST
checkCmdReturn $?

iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_RTP2 -i $IF_USBNET2 -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept Test Service packet:
iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_DEBUG_SERVICE -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept adb packet:
iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_DEBUG_ADB1 -j DEFEND_LIST
checkCmdReturn $?

iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_DEBUG_ADB2 -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept AvcLan debug tool packet
iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_DEBUG_AVC_TOOL -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept gdb debug packet
iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_DEBUG_GDB -j DEFEND_LIST
checkCmdReturn $?

#Accept RTP/RTCP packet:
iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_RTCP_SERVER -j DEFEND_LIST
checkCmdReturn $?

#Accept HDCP2 authentication packet
iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_HDCP2_AUTHENTICATION -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept Bargein tune packet:
iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_DEBUG_BARGEIN_TUNE -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept zeroconf packet
iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_MDNS -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept testmode alite packet
iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_TESTMODE_ALITE -i $IF_ETHERNET -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept AVC Handler packet:
iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_DEBUG_AVC -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept Window Manager packet:
iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_DEBUG_WINDOWMANAGER -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept Navi autotest packet:
iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_DEBUG_NAVI_AT1 -j DEFEND_LIST
checkCmdReturn $?

iptables -A TCP_INPUT -p tcp -m tcp --dport $PORT_DEBUG_NAVI_AT2 -j DEFEND_LIST
checkCmdReturn $?

iptables -A TCP_INPUT -p tcp -j DROP
checkCmdReturn $?

#INPUT chain for UDP:
iptables -N UDP_INPUT
checkCmdReturn $?

#Drop UDP packet which is from broadcast address:
iptables -A UDP_INPUT -p udp -m pkttype --pkt-type broadcast -j DROP
checkCmdReturn $?

#Accept established/related packet:
iptables -A UDP_INPUT -p udp -m state --state ESTABLISHED,RELATED -j DEFEND_LIST
checkCmdReturn $?

#Accept DHCP packet:
iptables -A UDP_INPUT -p udp -m udp --dport $PORT_DHCP -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_INPUT -p udp -m udp --sport $PORT_DHCP -j DEFEND_LIST
checkCmdReturn $?

#Accept DNS packet:
iptables -A UDP_INPUT -p udp -m udp --dport $PORT_DNS -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_INPUT -p udp -m udp --sport $PORT_DNS -j DEFEND_LIST
checkCmdReturn $?

#Accept TSL packet:
iptables -A UDP_INPUT -p udp -m udp --dport $PORT_TSL1 -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_INPUT -p udp -m udp --dport $PORT_TSL2 -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_INPUT -p udp -m udp --dport $PORT_TSL3 -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_INPUT -p udp -m udp --dport $PORT_TSL4 -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_INPUT -p udp -m udp --dport $PORT_TSL5 -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_INPUT -p udp -m udp --dport $PORT_TSL6 -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_INPUT -p udp -m udp --dport $PORT_TSL7 -j DEFEND_LIST
checkCmdReturn $?

#Accept UPnP packet:
iptables -A UDP_INPUT -p udp -m udp --dport $PORT_UPNP -j DEFEND_LIST
checkCmdReturn $?

#Accept RTP packet:
iptables -A UDP_INPUT -p udp -m udp --dport $PORT_RTP1 -i $IF_P2P -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_INPUT -p udp -m udp --dport $PORT_RTP2 -i $IF_P2P -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_INPUT -p udp -m udp --dport $PORT_RTP1 -i $IF_USBNET2 -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_INPUT -p udp -m udp --dport $PORT_RTP2 -i $IF_USBNET2 -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept Test Service packet:
iptables -A UDP_INPUT -p udp -m udp --dport $PORT_DEBUG_SERVICE -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept adb packet:
iptables -A UDP_INPUT -p udp -m udp --dport $PORT_DEBUG_ADB1 -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_INPUT -p udp -m udp --dport $PORT_DEBUG_ADB2 -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept gdb debug packet
iptables -A UDP_INPUT -p udp -m udp --dport $PORT_DEBUG_GDB -j DEFEND_LIST
checkCmdReturn $?

#Accept RTP/RTCP packet:
iptables -A UDP_INPUT -p udp -m udp --dport $PORT_RTCP_SERVER -j DEFEND_LIST
checkCmdReturn $?

#Accept HDCP2 authentication packet
iptables -A UDP_INPUT -p udp -m udp --dport $PORT_HDCP2_AUTHENTICATION -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept Bargein tune packet:
iptables -A UDP_INPUT -p udp -m udp --dport $PORT_DEBUG_BARGEIN_TUNE -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept zeroconf packet
iptables -A UDP_INPUT -p udp -m udp --dport $PORT_MDNS -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept AVC Handler packet:
iptables -A UDP_INPUT -p udp -m udp --dport $PORT_DEBUG_AVC -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept Window Manager packet:
iptables -A UDP_INPUT -p udp -m udp --dport $PORT_DEBUG_WINDOWMANAGER -j DEFEND_LIST
checkCmdReturn $?

#eng: Accept Navi autotest packet:
iptables -A UDP_INPUT -p udp -m udp --dport $PORT_DEBUG_NAVI_AT1 -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_INPUT -p udp -m udp --dport $PORT_DEBUG_NAVI_AT2 -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_INPUT -p udp -j DROP
checkCmdReturn $?

#INPUT chain for ICMP:
iptables -N ICMP_INPUT
checkCmdReturn $?

#Drop ping packet which is from broadcast address:
iptables -A ICMP_INPUT -p icmp -m pkttype --pkt-type broadcast -j DROP
checkCmdReturn $?

iptables -A ICMP_INPUT -p icmp --icmp-type echo-request -j DEFEND_LIST
checkCmdReturn $?

iptables -A ICMP_INPUT -p icmp --icmp-type echo-reply -j DEFEND_LIST
checkCmdReturn $?

iptables -A ICMP_INPUT -p icmp --icmp-type 11 -j DEFEND_LIST
checkCmdReturn $?

iptables -A ICMP_INPUT -p icmp --icmp-type 3 -j DEFEND_LIST
checkCmdReturn $?

iptables -A ICMP_INPUT -p icmp -j DROP
checkCmdReturn $?

#FORWARD chain for TCP:
iptables -N TCP_FORWARD
checkCmdReturn $?

#Accept established/related packet:
iptables -A TCP_FORWARD -p tcp -m state --state ESTABLISHED,RELATED -j DEFEND_LIST
checkCmdReturn $?

#Accept packet which is from WIFI Hotspot, BT Pan:
iptables -A TCP_FORWARD -p tcp -i $IF_WLAN -s $IP_WHS -j DEFEND_LIST
checkCmdReturn $?

iptables -A TCP_FORWARD -p tcp -j DROP
checkCmdReturn $?

#FORWARD chain for UDP:
iptables -N UDP_FORWARD
checkCmdReturn $?

#Drop UDP packet which is from broadcast address:
iptables -A UDP_FORWARD -p udp -m pkttype --pkt-type broadcast -j DROP
checkCmdReturn $?

#Accept established/related packet:
iptables -A UDP_FORWARD -p udp -m state --state ESTABLISHED,RELATED -j DEFEND_LIST
checkCmdReturn $?

#Accept DNS packet:
iptables -A UDP_FORWARD -p udp -m udp --dport $PORT_DNS -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_FORWARD -p udp -m udp --sport $PORT_DNS -j DEFEND_LIST
checkCmdReturn $?

#Accept packet which is from WIFI Hotspot, BT Pan:
iptables -A UDP_FORWARD -p udp -i $IF_WLAN -s $IP_WHS -j DEFEND_LIST
checkCmdReturn $?

iptables -A UDP_FORWARD -p udp -j DROP
checkCmdReturn $?

#FORWARD chain for ICMP:
iptables -N ICMP_FORWARD
checkCmdReturn $?

iptables -A ICMP_FORWARD -p icmp --icmp-type echo-request -j DEFEND_LIST
checkCmdReturn $?

iptables -A ICMP_FORWARD -p icmp --icmp-type echo-reply -j DEFEND_LIST
checkCmdReturn $?

iptables -A ICMP_FORWARD -p icmp --icmp-type 11 -j DEFEND_LIST
checkCmdReturn $?

iptables -A ICMP_FORWARD -p icmp --icmp-type 3 -j DEFEND_LIST
checkCmdReturn $?

iptables -A ICMP_FORWARD -p icmp -j DROP
checkCmdReturn $?

#Jump to indicated chain according to the protocol (so far only tcp/udp/icmp are supported):
iptables -A INPUT -p tcp -j TCP_INPUT
checkCmdReturn $?

iptables -A INPUT -p udp -j UDP_INPUT
checkCmdReturn $?

iptables -A INPUT -p icmp -j ICMP_INPUT
checkCmdReturn $?

iptables -A FORWARD -p tcp -j TCP_FORWARD
checkCmdReturn $?

iptables -A FORWARD -p udp -j UDP_FORWARD
checkCmdReturn $?

iptables -A FORWARD -p icmp -j ICMP_FORWARD
checkCmdReturn $?

iptables -A INPUT -j ACCEPT
checkCmdReturn $?

iptables -A FORWARD -j ACCEPT
checkCmdReturn $?

fi

if [ ${mode} -eq ${DCU_MEU_MODE} -o ${mode} -eq ${DEFAULT_DCU_MEU_MODE} ] ; then
IF_NCM=$2
#Define port
PORT_HTTPFILE="49153"
PORT_1SEG="1111"
PORT_IOV="6082"

#No limit in NCM
iptables -I DEFEND_LIST 2 -p tcp -i $IF_NCM -j ACCEPT
checkCmdReturn $?

iptables -I DEFEND_LIST 3 -p udp -i $IF_NCM -j ACCEPT
checkCmdReturn $?

iptables -I DEFEND_LIST 4 -p icmp --icmp-type echo-request -i $IF_NCM -j ACCEPT
checkCmdReturn $?

#Accept tcp packet for HttpFile, 1Seg, IOV:
iptables -I TCP_INPUT 2 -p tcp -i $IF_NCM -s $IP_MEU -m tcp --dport $PORT_HTTPFILE -j DEFEND_LIST
checkCmdReturn $?

iptables -I TCP_INPUT 3 -p tcp -i $IF_NCM -s $IP_MEU -m tcp --dport $PORT_1SEG -j DEFEND_LIST
checkCmdReturn $?

iptables -I TCP_INPUT 4 -p tcp -i $IF_NCM -s $IP_MEU -m tcp --dport $PORT_IOV -j DEFEND_LIST
checkCmdReturn $?

#Accept udp packet for HttpFile, 1Seg, IOV:
iptables -I UDP_INPUT 3 -p udp -i $IF_NCM -s $IP_MEU -m udp --dport $PORT_HTTPFILE -j DEFEND_LIST
checkCmdReturn $?

iptables -I UDP_INPUT 4 -p udp -i $IF_NCM -s $IP_MEU -m udp --dport $PORT_1SEG -j DEFEND_LIST
checkCmdReturn $?

iptables -I UDP_INPUT 5 -p udp -i $IF_NCM -s $IP_MEU -m udp --dport $PORT_IOV -j DEFEND_LIST
checkCmdReturn $?

#Accept packet which is from DCU-MEU
iptables -I TCP_FORWARD 2 -p tcp -i $IF_NCM -s $IP_MEU -j DEFEND_LIST
checkCmdReturn $?

iptables -I UDP_FORWARD 3 -p udp -i $IF_NCM -s $IP_MEU -j DEFEND_LIST
checkCmdReturn $?
fi

if [ ${mode} -eq ${CLEAR_DEFAULT_MODE} -o ${mode} -eq ${CLEAR_NCM_MODE} ] ; then
iptables -F
checkCmdReturn $?

iptables -X
checkCmdReturn $?

iptables -P INPUT DROP
checkCmdReturn $?

iptables -P FORWARD DROP
checkCmdReturn $?

iptables -P OUTPUT DROP
checkCmdReturn $?

fi

if [ ${mode} -eq ${INIT_FIREWALL_MOED} -o ${mode} -eq ${CLEAR_NCM_MODE} ] ; then
IF_NCM=$2

iptables -A INPUT -p tcp -i $IF_NCM -j ACCEPT
checkCmdReturn $?

iptables -A INPUT -p udp -i $IF_NCM -j ACCEPT
checkCmdReturn $?

iptables -A OUTPUT -p tcp -o $IF_NCM -j ACCEPT
checkCmdReturn $?

iptables -A OUTPUT -p udp -o $IF_NCM -j ACCEPT
checkCmdReturn $?

fi

exit 0






