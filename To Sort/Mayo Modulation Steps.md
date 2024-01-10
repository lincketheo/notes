1. Generate input file (14 bytes):
```
Hello World...
```

2. Mayo Mod Pre
```sh
$ ./mayo_mod_pre --din input_file.txt --dout pre.bin
```

3. Generate Offsets
```
$ ./offsets --din pre.bin --dout offsets.bin
```

4. Spread Offsets
```
$ ./css --din offsets.bin --dout css.bin
```

# DEMOD

1. Generate match signals
```
$ ./match --dout match0.bin --symbol 0
$ ./match --dout match1.bin --symbol 1
```

2. Spread match signals
```
$ ./css --din match0.bin --dout match0.css.bin
$ ./css --din match1.bin --dout match1.css.bin
```

3. Demod File
```
$ ./dc_demod --din data.bin --m0in match0.css.bin --m1in match1.css.bin --repeat --dout output.bin
```

4. Demod Post File
```
$ ./mayo_demod_post --din output.bin --dout output.txt
```

