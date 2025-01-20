# Exception Handling

## Rules:
As a rule of thumb, `try catch` is a **code smell**
- `try catch` blocks increase cyclomatic complexity. An increase in cycomatic complexity should be the response to a new feature or behavior of your application.
- Handle one exception for each logical "reason" you need to catch that exception
- Reasons you want to handle exceptions might include:
    - I want to continue running a sequential set of steps even if this code block fails
    - I want to actually display different information on the screen if this function fails
- If you don't have a logical reason to handle the exception, let it throw. If it throws, then it's a bug and you fix it. 
- It's better to be lazy and not write a `try catch` than it is to sweep an exception under the rug.

Bad:
```python
def io_function():
    try:
        # do some io call that could throw an exception
    except Exception as ex:
        # do something else
        return False

def top_level_function():
    result = io_function()
    if result:
        # do more stuff
    else:
        print("Error! Couldn't do IO stuff")
```
Keep in mind - this isn't necessarily _wrong_... just notice how it actually handles exceptions in two places. 

It would take less code to write:
```python
def io_function():
    # do some io call that could throw an exception

def top_level_function():
    try:
        result = io_function()
    except Exception as ex:
        print("Error! Couldn't do IO stuff")
```
In highly object oriented languages like Java, this converts `io_function` into a throwing function 
(depending on checked / unchecked'ness of the exception, but that's a different story)

Sometimes, people will just raise the exception in the except block, e.g.:
```python
def foo_bar():
    try: 
        # do something
    except Exception as ex:
        raise ex
```
This is pointless and is logically equivalent to:
```python
def foo_bar():
    # do something
```

You may ask, but what if I want to notify the user in the exception block!? Something like:
```python
def foo_bar():
    try: 
        # do something
    except Exception as ex:
        logger.log_warn(f"Something went wrong in foo_bar function with reason: {ex}")
        raise ex
```
Sure, this gives the end user some idea where the exception happened, but if ex is uncaught, your stack traces start looking awful.
In general, [_log and throw is an antipattern_](https://www.google.com/search?q=log+and+throw+antipattern&client=firefox-b-1-e&sca_esv=06eb692d555481e2&ei=p5v8ZfHLGojf5NoPrfGLgAE&ved=0ahUKEwixu97wm4aFAxWIL1kFHa34AhAQ4dUDCBA&uact=5&oq=log+and+throw+antipattern&gs_lp=Egxnd3Mtd2l6LXNlcnAiGWxvZyBhbmQgdGhyb3cgYW50aXBhdHRlcm4yChAAGEcY1gQYsAMyChAAGEcY1gQYsAMyChAAGEcY1gQYsAMyChAAGEcY1gQYsAMyChAAGEcY1gQYsAMyChAAGEcY1gQYsAMyChAAGEcY1gQYsAMyChAAGEcY1gQYsANIogRQAFgAcAF4AZABAJgBAKABAKoBALgBA8gBAJgCAaACBZgDAIgGAZAGCJIHATGgBwA&sclient=gws-wiz-serp)

# Never Throw Exceptions under the rug:
```python
try: 
    # do something
except Exception as ex:
    logger.log_warn(f"Something went wrong in foo_bar function with reason: {ex}")
```
It's way better to be lazy and just write:
```python
# do something
```
than throw that exception under the rug. These exceptions are an absolute _pain_ to debug.


- [Other "Logging" (really exception) anti patterns](https://rolf-engelhard.de/2013/04/logging-anti-patterns-part-ii/)