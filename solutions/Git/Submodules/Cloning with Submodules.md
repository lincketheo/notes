# With Recursive (Better)
```
git clone <remote> --recurse-submodules
```

# Without recursive
```
git clone <remote> 
cd <folder>
git submodule init
git submodule update
```

_or_

```
git clone <remote>
cd <folder>
git submodule update --init
```

