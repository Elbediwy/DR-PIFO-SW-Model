#!/usr/bin/env python

import argparse
import socket
import struct
import sys
import time

from scapy.all import sendp, send, hexdump, get_if_list, get_if_hwaddr, get_if_addr, srp
from scapy.all import Ether, IP, UDP, TCP,IPv6
from scapy.all import Packet, IPOption


from time import sleep


def get_if():
    iface = None
    for i in get_if_list():
        if "eth0" in i:
            iface = i
            break
    if not iface:
        print "Cannot find eth0 interface"
        exit(1)
    return iface



def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--h", help="The host ID, to send the corresponding flow", type=str)
    parser.add_argument("--des", help="IP address of the destination", type=str)
    #parser.add_argument("--m", help="Raw Message", type=str)

    args = parser.parse_args()

    enq_file_in = open(args.h, "r")

    enq_lines = enq_file_in.readlines()
    enq_file_len = len(enq_lines)
    read_rank = 0
    args.m = "P4 is cool"
    if args.h and args.des and args.m:
        addr = socket.gethostbyname(args.des)
        iface = get_if()
          
       # pkt = Ether(src=get_if_hwaddr(iface), dst="ff:ff:ff:ff:ff:ff") / IP(src=get_if_addr(iface), dst="10.0.0.0", options=IPOption(bytes(1000))) / TCP() / args.m
       # sendp(pkt, iface=iface, verbose=False)
       # sleep(10)
       # sleep(1)
        for line in enq_lines:
            rank = int(enq_lines[read_rank])
            rank_bytes = struct.pack('>i',rank)
            #sleep(10)
            read_rank = read_rank + 1
            pkt = Ether(src=get_if_hwaddr(iface), dst="ff:ff:ff:ff:ff:ff", type=0x800) / IP(src=get_if_addr(iface), dst=addr, options=IPOption(rank_bytes)) / TCP() / args.m
            #pkt = Ether(src=get_if_hwaddr(iface), dst="ff:ff:ff:ff:ff:ff", type=0x800) / IP(src=get_if_addr(iface), dst=addr, options=IPOption('\x00\x02\x07\x88')) / TCP() / args.m
            pkt_good = Ether(str(pkt)) 

            print("This host has sent ",read_rank,"packets until now :", time.time())

            try:
                sendp(pkt_good, iface=iface, verbose=False)
                if read_rank > 4445:
                	sleep(0.1)
                else:
                	sleep(0.1)
            except KeyboardInterrupt:
                raise


if __name__ == '__main__':
    main()
