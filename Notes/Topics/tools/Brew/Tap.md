Third Party Repositories

```sh
$ brew tap # Show current tapped repositories
$ brew tap <user/repo> # Clones repo https://github.com/<user>/homebrew-<repo> into $(brew --repository)/Library/Taps
$ brew tap <user/repo> <URL> # Same as above, but doesn't assume github
$ brew tap --repair # Migrates tapped formulae from symlink to directory based
$ brew untap <user/repo> <user/repo> ... # Removes taps
```