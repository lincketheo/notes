# Tracking a branch instead of a commit
https://stackoverflow.com/questions/10443627/force-git-submodules-to-always-stay-current

## Already cloned repo - make an update
```
git config -f .gitmodules submodule.<name>.branch <branch>
git submodule update --remote
```

## Add a submodule and track a branch instead of commit
```
git submodule add -b <branch> <repository> [<path>]
```