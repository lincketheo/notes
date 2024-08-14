# Extract Method Considerations
- Better to take it slow as you refactor methods than to correct yourself - take your time at the start, don't rush + correct
- Considerations:
    - Reusability vs Readability
        - Extracting a method is good, extracting a method without depending on a whole lot of other context is better _iff_ there's
        some motivation to reuse the code. Like everything, analyse it with a scrutinous eye
```c
void do_thing(private_data* pd, int i, int j){
    return pd->q + i + j;
}
```
```c
void do_thing(int q, int i, int j){
    return q + i + j;
}
```
This doesn't mean always avoid context's, just think about when a function is being extracted for readability vs reusability.