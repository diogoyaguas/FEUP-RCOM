#!/bin/bash

ifconfig eth0 up 172.16.10.254/24
ifconfig eth1 up 172.16.11.253/24
echo 1 > /proc/sys/net/ipv4/ip_forward
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
route add default gw 172.16.11.254
nano /etc/resolv.conf
