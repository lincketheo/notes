```
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
```

```
format:
	find . -name "*.c" -o -name "*.h" | xargs clang-format -i
```

