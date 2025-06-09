# Check for listening ports
_Prefer `ss`. Netstat is often deprecated_

## Using SS
- `ss -lntu`
- l: Only services that are listening
- n: Show port number, don't resoolve service name
- t: tcp ports
- u: udp ports
- p: name of the program

## Using Netstat:
- (Part of `net-tools` package)
- `netstat -lntu`

## Mac
- Netstat is not ported to macos
- `lsof -i -P | grep LISTEN`
- lsof - List open files
- i: Listening files
- P: Doesn't convert port numbers to port names for network files


# Isolate chunk of Bytes in a file 
```
dd skip=<starting byte> count=<num bytes> if=<input> of=<output> bs=1 # bs = 1 => 1 byte "chunks"
```

# Link files
```
ln - s <path to the _actual_ file> <path to the link to be created>
```

# List all files under 100 kb
```bash
#!/bin/bash

# Could add these as cli arguments
folder=.
max=100

df_res=$(du -s $folder/*) 

while read -r line
do
    split=( $line )
    size=${split[0]}
    folder=${split[1]}

    if test $size -lt $max
    then
        echo "$folder with size $size KB is less than $max KB"
    fi
done <<< "$df_res"
```

# Use of `exec`

Command replaces current shell

Useful for process replacement:
```sh
exec /usr/bin/zsh
```

Starts a new shell (`zsh`) in place of the current shell

Assigns current pid:
```sh
$ echo $$
```

to the new process:
```sh
$ exec sleep 10
$ ps aux | grep $USER | grep sleep # In a new shell - should be the pid of the calling shell
```

Never outputs "World" because current pid replaces `echo "Hello"`
```sh
#!/bin/bash

exec echo "Hello"
echo "World"
```

