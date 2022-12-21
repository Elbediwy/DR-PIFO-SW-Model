#!/usr/bin/env python

import sys
from scapy.all import sniff, get_if_list
import time

def handle_pkt(pkt):
    print "got a packet"
    print("packet is received at time :", time.time())
    pkt.show2()
    sys.stdout.flush()


def main():
    iface = 'eth0'
    print "sniffing on %s" % iface
    print("the simulation started at time :", time.time())
    sys.stdout.flush()
    sniff(iface=iface, prn=lambda x: handle_pkt(x))


if __name__ == '__main__':
    main()
