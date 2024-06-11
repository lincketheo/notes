# Computation
Example:
Encode 14 bits (11010011101100) with polynomial x^3 + x + 1 => 1011

Pad 14 bits with the length of the crc
CRC Length:
    - highest power of the coefficient

```
11010011101100 000
1011
XOR
01100011101100 000
 1011
00111011101100 000
  1011
00010111101100 000
   1011
00000001101100 000
...
00000000001110 000
          1011
00000000000101 000
           101 1
00000000000000 100 => Stop here because left is 0
            10 11
00000000000010 010 => If you kept going, you would have 1's on the left -> bad
            10 11
00000000000010 010
             1 011
```