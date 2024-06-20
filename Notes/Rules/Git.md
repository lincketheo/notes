
1. Main branch is always protected
    - With some exceptions, nothing is ever pushed to main. Only PRs
    - With some exceptions, don't rebase - keep branch history as git intended. (Generally, no `--force`)
2. Base functionality:
    - Commitizen commits - install globally using npm for all languages
    - Changelog - Manually do this - figure out how to keep this automated?
    - (Optional) ADR - Design References