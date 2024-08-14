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

