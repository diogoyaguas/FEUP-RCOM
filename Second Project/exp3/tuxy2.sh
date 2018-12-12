ifconfig eth1 172.16.21.1/24
route -n
route add -net 172.16.20.0/24 gw 172.16.21.253
arp -d 172.16.21.253
