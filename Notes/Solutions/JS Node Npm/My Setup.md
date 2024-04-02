# Description
Everything will be installed inside ~/.js, and no need to use sudo for npm

1. Create npm prefix:
```
npm config set prefix ~/.js/npm
```

2. Install n:
```
npm install n
```

3. Add `bin` to path
zshrc:
```
export PATH="$HOME/.js/npm/bin:$PATH"
```

4. Set n prefix:
zshrc
```
export N_PREFIX=~/.js/n
```

5. Install Run n:
```
n stable
```

6. Add node to path:
```
export PATH="$HOME/.js/n/bin:$PATH"
```