# Prefer `ss`. Netstat is often deprecated 

# Using SS
- `ss -lntu`
- l: Only services that are listening
- n: Show port number, don't resoolve service name
- t: tcp ports
- u: udp ports
- p: name of the program

# Using Netstat:
- (Part of `net-tools` package)
- `netstat -lntu`

## Mac
- Netstat is not ported to macos
- lsof -i -P | grep LISTEN 
- lsof - List open files
- i: Listening files
- P: Doesn't convert port numbers to port names for network files


