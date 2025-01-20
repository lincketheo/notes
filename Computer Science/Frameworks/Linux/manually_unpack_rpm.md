# List the files in a package
```
$ rpm2cpio file.rpm | cpio -t
```

# Output to a cpio archive
```
$ rpm2cpio file.rpm > file.cpio
```

# Take a file:
```
$ rpm2cpio file.rpm | cpio -ivd /path/to/file/inside/rpm
```
- i: Extract one or more files from an archive 
- v: Display the names of files processed and size of the archive file 
- d: Create direcotries that precede the filename
