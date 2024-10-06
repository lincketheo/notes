https://ftp.gnu.org/old-gnu/Manuals/gprof-2.9.1/html_mono/gprof.html#SEC1

# gprof
- Compile it
```
$ gcc program.c -pg
```

- Run it
```
$ ./a.out
```

- Profile in `gmon.out`
```
$ gprof > output.txt
```

# Valgrind
(I like this)
```
valgrind --tool=callgrind ./binary
kcachegrind callgrind.out.x
```

```
valgrind --tool=callgrind --dump-instr=yes -v --instr-atstart=no ./binary > tmp
```

In another window:
```
callgrind_control -i on
```

```
callgrind_control -k
```

```
kcachegrind callgrind.out.x
```

# Perf
TODO

# gperftools
