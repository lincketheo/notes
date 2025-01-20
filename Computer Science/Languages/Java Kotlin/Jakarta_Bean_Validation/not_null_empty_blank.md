# Not Null
```
public class UserNotNull {
    @NotNull(message = "Name may not be null")
    private String name;
}
```

- null: ERROR 
- "John": SUCCESS 
- "": SUCCESS

# Not Empty
```
public class UserNotEmpty {
    @NotEmpty(message = "Name may not be empty")
    private String name;
}
```

- null: ERROR 
- "John": SUCCESS 
- "": ERROR 

# Not Blank
```
public class UserNotBlank {
    @NotBlank(message = "Name may not be blank")
    private String name;
}
```

- null: ERROR 
- "John": SUCCESS 
- "": ERROR 
- " ": ERROR 
