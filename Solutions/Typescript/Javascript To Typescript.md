## Migrating From Javascript To Typescript in Vue3

- Install typescript
```
$ npm install --save typescript
```

- Add a tsconfig
```
$ npx tsc --init
```

- Make sure tsconfig has:
```
"script" : true,
"allowJs" : true,
```

- ESLint needs typescript support too:
https://stackoverflow.com/questions/70083042/eslint-parsing-error-unexpected-token
https://typescript-eslint.io/getting-started/
