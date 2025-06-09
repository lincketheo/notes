# Types
1. Strongly Typed Relational Data 
    - Entities are strongly typed and can relate to one another
    - Examples:
        - PSQL
        - Sqlite
        - Generally any SQL 
2. Contiguous array (usually numerical) data 
    - Entities are small 1 type no relations stored contiguously 
    - Generally support math operations:
        - If I have a vector of this type of data, I can enact any math operation + - / * or and ~ etc 
          and intuitively know what is going to result
        - Might cover numstore
3. Unstructured 
4. Key Value
    - Values can either be strongly typed or not
5. Graph 
6. Time series


class DateTime {
    date: String 
    time: String
}

class Users {
    age: int 
    birthday: DateTime
    name: String
}
