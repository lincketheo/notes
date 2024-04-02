# NPM Prefix
Tells npm where to install global modules

Set it per install using:
```
npm --prefix=$PREFIX ...
```

Set it globally
```
npm config set prefix ~/.npm
```

Or, add `prefix=~/.npm` to ~/.npmrc

Also, add this prefix to your path:
```
export PATH="$HOME/.npm/bin:$PATH"
```