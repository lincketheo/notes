Deals with "frames"

- Physical addressing (MAC address)
- [[Ethernet Switch]]
	- Figures out which port various MAC addresses live
		- If MAC address doesn't exist, flood all ports
		- maintain a table of MAC locations
	- Do not examine the contents of packets or payload
- Ethernet is a layer 2 protocol