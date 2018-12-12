ifconfig eth0 172.16.20.254/24
ifconfig eth1 172.16.21.253/24
echo 1 > /proc/sys/net/ipv4/ip_forward
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
#observe MAC addresses
ifconfig eth0
ifconfig eth1
route -n
arp -d 172.16.20.1
arp -d 172.16.21.1
