# Simple Data Types

_Contain a single value_

-   Strings
    -   `"Foo bar"` Interpreted string
    -   Raw string ('\n' is the characters '\' and 'n')

```
`fizbuz \n
bar biz`
```

-   Numbers
    -   `int`
    -   `uint`
    -   `float32`
    -   `float64`
    -   `complex64`
    -   `complex128`
-   Booleans
    -   `true` / `false`
    -   No such things as falsey and truthey
-   Errors

```
type error interface {
    Error() string
}
```

# Declaring Variables

-   Reverse syntax is a compiler optimization
    -   If we don't explicitly define it, it gets zero type
        -   string = ""
        -   boolean = false
        -   ...

```go
var myName string           // declare (get's 'zero type')
var myName string = "Theo"  // declare and init
var myName = "Theo"         // type inference
myName := "Theo"            // Short declaration
const a = 4                 // constant implicitly typed
const b string = "hello"    // constant explicitly typed
const c = a                 // Assigned to others
const (
    d = true
    e = 3.14
)
const c = 2 * 5
const d = "hello" + ", wor"  // Must be evaluated at compile time
XX const e = func()          // Not allowed
```

# Pointers

```
a := 42
b := a      // copy
a = 27      // b stays the same
```

```
a := 42
b := &a     // Reference
a = 27      // *b is 42
c = new(int)
```

# Aggregate Data Types

## Arrays

Fixed sized collection of 1 data type

```
var arr [3]int
fmt.Println(arr)        // [0 0 0]
arr = [3]int{1, 2, 3}
fmt.Println(arr)        // [1 2 3]
fmt.Println(arr[1])     // 2
arr[1] = 99
fmt.Println(len(arr))   // 3
arr2 = arr              // Every element is copied into arr2
arr2 == arr             // false
```

## Slices

```
var s []int
fmt.Println(s)              // nil
s = []int{1, 2, 3}
s = append(s, 5, 10, 15)
s = slices.Delete(s, 1, 3)  // deletes [1, 3)
s2 := s                     // copied by reference
XX s == s2                  // Compile error (can only compare values in go)
```

## Maps

```
var m map[string]int
m = map[string]int{"foo" : 1, "bar" : 2}
delete(m, "foo")
m["baz"] = 418
m["foo"]                    // returns 0  (think uninitialized rule earlier)
v, ok := m["foo"]           // ok is false
m2 := m                     // Maps copied by reference
XX m == m2                  // Compile error (can only compare values in go)
```

## Structs

```
var s struct{
    name string
    id int
}
fmt.Println(s)      // {"", 0} - struct is a value type
s.name = "Arthur"

type myStruct struct {
    name string 
    id int
}
var s myStruct
s = myStruct {
    name: "Arthur",
    id: 42 
}
s2 = s              // Copied by value
s2 == s             // false
```

# If statements
```
if test {

}

if test {

} else {

}

if test {

} else if test {

} else {

}

if initializer; test {

}
```

# Switch Statements 
```
switch test expression {
    case expression1:
        statements
    case expression2, expression3:
        statements
    default:
        statements
}

switch i:= 999; i {
    case 1:
        fmt.Println("First case")
    case 2 + 3, 2*i + 3:
        ...
    default:
        ...
}
```

# Loops 
```
for condition {
    ...
}

for initializer (i := 0); test (i < 10); post clause (i++) {

}

for key, value := range collection {}
for key := range collection {}
for _, value := range collection {}
```

# Functions 
```
func funcName(parameters) (return Values) {
    body
}

// Parameters
func greet(name string) {
    fmt.Println(name)
}
func greet(name1 string, name2 string) {
    fmt.Println(name1)
    fmt.Println(name2)
}
func greet(name1, name2 string) {
    fmt.Println(name1)
    fmt.Println(name2)
}
// Variadic recieved as a slice
// Names is a slice of strings
func greet(names ...string) {
    for _, n := range names {
        fmt.Println(n)
    }
}

// Returning 
func myFunc(name string, name2 *string) {
    *name2 = "foobar"
}

// returning 
func add(l, r int) int {
    return l + r
}
func divide(l, r int) (int, bool) {
    if r == 0 {
        return 0, false 
    } 
    return l/r, true
}
func divide(l, r int) (result int, ok bool) {
    if r == 0 {
        return 
    }

    result = 1/r 
    ok = true 
    return
}
```
