
# Logging

- What should I log?

Generally:
- Do:
    - Clearly articulate what went wrong and try to include as much _context_ in your logs as possible
    - Include non sensitive data to catch edge cases
    - Make your logs as different from one another as possible (previous two bullet points are examples)
- Don't
    - Put sensitive information inside logs
    - Log things that could not be traced back to their origin e.g. `logger.log_info("done")`

1. `log_info`:
    - These are logs you intend to show up on a day to day basis. 
    - Do:
        - Put vital information on what general steps the program is making
        - Log within `O(1)` subroutines
    - Don't:
        - Put repetitive / verbose output in these logs. 
            - If an area of the code base scales `O(n)`, don't log inside the repetative part
2. `log_debug`:
    - Very verbose output. These are more free form and don't always adapt to the standard rules of logging, but be nice 
    - Ok:
        - Logging within an `O(n)` subroutine - try to capture something unique in each log in the subroutine
3. `log_warn` vs `log_error`:
    - `log_error` means the code is broken and would benefit from being killed. As a general rule of thumb, don't log error for things
    that are recoverable or code that might fail one thing but not another thing. The intention of log error is to indicate **system failure**
        - Does it warant a call to pager duty? No? Then log it as a warning.
        - Therefore, this testing code base probably shouldn't have any error logs.
    - `log_warn` is for everything else that went wrong that isn't an `error`