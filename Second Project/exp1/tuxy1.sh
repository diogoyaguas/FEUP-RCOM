ifconfig eth0 #we can see the MAC address in the entry HWaddr
ifconfig eth0 172.16.20.1/24
ping 172.16.20.254
route -n
arp -a
arp -d 172.16.20.254
