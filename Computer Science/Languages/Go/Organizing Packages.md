## Basic Package
```
/root
	go.mod
		---
		module github.com/lincketheo/module	
		...
		---
	module.go
		---
		package module
		...
		---
	module_test.go
```

- Another Library:
```
import "github.com/lincketheo/module"
```

### Split into multiple files
```
/root
	go.mod
		---
		module github.com/lincketheo/module	
		...
		---
	module.go
		---
		package module
		...
		---
	auth.go
		---
		package module
		...
		---
	auth_test.go
	...
	module_test.go
```

- Another Library:
```
import "github.com/lincketheo/module"
```

## Basic Command
```
/root
	go.mod
		---
		module github.com/lincketheo/module	
		...
		---
	main.go
		---
		package main
		func main ...
		---
	auth.go
		---
		package module
		...
		---
	auth_test.go
	...
	module_test.go
```

- Another Library:
```
go install github.com/lincketheo/module@latest
```

## Supporting Packages
```
/root
	internal/
		auth/
			auth.go
			---
			package auth
			---
			auth_test.go
		hash/
			hash.go
			hash_test.go
	go.mod
		---
		module github.com/lincketheo/module	
		...
		---
	module.go
		---
		package module
		import "github.com/lincketheo/module/internal/auth"
		---
```

## Multiple Packages
```
root/
	go.mod
	---
	module github.com/lincketheo/module
	---
	module.go
	auth/
		auth.go
		token/
			token.go
	hash/
		hash.go
	internal/
		trace/
			trace.go
```

```
import github.com/lincketheo/module
import github.com/lincketheo/module/auth/token
import github.com/lincketheo/module/internal/trace XXX ERROR XXX
```

## Multiple Commands
```
root/
	go.mod
	---
	module github.com/lincketheo/module
	---
	prog1
		main.go
	prog2 
		main.go
	internal/
```

```
go install github.com/lincketheo/module/prog1@latest
go install github.com/lincketheo/module/prog2@latest
```

## Packages and commands in the same repository¶

```
project-root-directory/
  go.mod
  modname.go
  modname_test.go
  auth/
    auth.go
    auth_test.go
  internal/
    ... internal packages
  cmd/
    prog1/
      main.go
    prog2/
      main.go
```

```
import "github.com/someuser/modname"
import "github.com/someuser/modname/auth"
```

```
$ go install github.com/someuser/modname/cmd/prog1@latest
$ go install github.com/someuser/modname/cmd/prog2@latest
```







