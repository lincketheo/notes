# Notes:
1. Not every comment is a suggested fix:
    - Ask question about an unkown language feature or design pattern
    - Suggest non crucial changes without an indication of a required change
    - Indicate comments that you would like to see a change or a ticket to change - have a conversation if need be
2. Try not to merge PR's unless at least 1 person has reviewed your code. (Some platforms have branch protections - this is a good idea)


# Code review Check List
1. Functionality
    - If there are acceptance criteria in the original ticket, does the code satisfy them?
    - Else, does the code meet the original intention of the ticket?
2. Testability
    - Is each new logical branch in some way tested?
    - Else, for each new feature that isn't tested, is there a reason why not or a ticket to implement testing?
4. Readability
    - Go through the code and spend ~5 minutes trying to understand each addition. If you can't understand parts, ask what the code does and the PR maintainer should either:
        - Add docs explaining the complex code
        - Refactor to make more readable
    - Suggest better variable or function names
    - Find functions that might be lying (e.g. expanded responsibility)
5. Code structure and design
    - Iterate through SOLID, KISS, YAGNI / whatever principles you use and point out areas that can be improved. Only hold up the code review if you believe your suggestions are requirements, otherwise, add a note saying "not crucial".
6. Performance
    - Note performance bottlenecks. For each bottleneck, PR maintainer should either:
        - Fix the code
        - Write a ticket to fix the code
7. Observability
    - Suggest locations to add logs
    - Suggest more comprehensive log messages
8. Error Handling
9. Code reuse
    - Note areas in the PR that could be abstracted out. No one is familiar with 100% of the code base, so this helps us pin point repeated code
    - Determine the efficacy of abstraction and either:
        - Fix the code 
        - Write a ticket to abstract the code
10. Documentation
    - Appart from the reusability docs, suggest locations in the README / docstrings that could be expanded upon
11. Coding Standards 
    - Reformat the code (this is a good place to add a git commit hook)