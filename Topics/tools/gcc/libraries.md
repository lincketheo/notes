# -l vs -L: Linking and Searching for libraries
```
-l<name>
```
Searches `/usr/lib` for `/usr/lib/lib<name>.so`

```
-L<library/path>
```
Searches `<library/path>`

## Setting `LIBRARY_PATH`
```
export LIBRARY_PATH=/path/to/custom/library:$LIBRARY_PATH
gcc -o myprogram myprogram.c -lcunit
```

## ldconfig
- Make a new config in `/etc/ld.so.conf.d/<custom_conf_name>.conf
- paste the path in there:
