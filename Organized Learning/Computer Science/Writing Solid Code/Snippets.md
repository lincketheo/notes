# Assertions
***
Use assert for incorrectly used functions not runtime errors:
```
int calc_data(char* data) {
    assert(data != NULL);

    char* new_data = malloc(10);
    assert(new_data != NULL);
}
```
First is good, second is bad
***