# Communication Styles:
1. `SOCK_STREAM`: Like a [pipe](https://www.gnu.org/software/libc/manual/html_node/Pipes-and-FIFOs.html). Transmits data in a stream of bytes
2. `SOCK_DGRAM`: Sends individually addressed packets unreliably. 
    - _send order is not guarenteed to be the same as recieved order_
    - Specify the address every time you send the packet
    - Usage: 
        - When it is acceptable to simply re sent a packet if no response is seen in a reasonable amount of time
3. `SOCK_RAW`: Low level network protocols. Most application developers won't need this socket

# Socket Addresses
- New socket created with `socket` has no address
    - You `bind` a socket to an _address_ so that other sockets can find it
    - An unnamed socket can still send data (sometimes client needs address for application - e.g. rsh)
- Details of addresses different between local and internet namespaces:
    - `PF_INET`: See [Internet](https://www.gnu.org/software/libc/manual/html_node/Internet-Namespace.html) Namespaces
    - `PF_LOCAL`: See [Local](https://www.gnu.org/software/libc/manual/html_node/Local-Namespace.html) Namespaces
- `bind` and `getsockname` use (abstract) `struct sockaddr` as parameter regardless of namespace, which you cast the argument to depending on which namespace you used

   
