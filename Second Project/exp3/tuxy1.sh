ifconfig eth0 172.16.20.1/24
route -n
route add -net 172.16.21.0/24 gw 172.16.20.254
ping 172.16.20.254
ping 172.16.21.253
ping 172.16.21.1
arp -d 172.16.20.254
ping 172.16.21.1
