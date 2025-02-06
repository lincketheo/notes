## Arrays
```
var a [4]int // init to 0
a[0] = 1
i := a[0]
i == 1
b := [2]string{"foo", "bar"}
c := a // copies all of a into c
d := [...]string{"foo", "bar"}
```

- Inflexible
## Slices
- Declared the same as an array, but leave out the element count 
```
letters := []string{"a", "b", "c"}
foo := make([]byte, 5, 5) // make([]T, len, cap) []T
len(foo) == 5
cap(foo) == 5
b := []byte{1, 2, 3, 4, 5, 6, 7}
b[1:4] == []byte{2, 3, 4} // share storage
```

- When you assign slices to stuff, they point to that stuff (arrays for ex)
- When you create a slice it makes a call to malloc

```
s := s[:cap(s)]
s := s[:cap(s) + 1] // runtime error
```

### Growing Slices
```
t := make([]bytes, )
```

#### Copy
```
t := make([]byte, len(s), (cap(s)+1)*2) // +1 in case cap(s) == 0
for i := range s {
        t[i] = s[i]
}
s = t
```
Is the same as
```
t := make([]byte, len(s), (cap(s)+1)*2)
copy(t, s)
s = t
```

#### Append
```
func AppendByte(slice []byte, data ...byte) []byte {
    m := len(slice)
    n := m + len(data)
    if n > cap(slice) { // if necessary, reallocate
        // allocate double what's needed, for future growth.
        newSlice := make([]byte, (n+1)*2)
        copy(newSlice, slice)
        slice = newSlice
    }
    slice = slice[0:n]
    copy(slice[m:n], data)
    return slice
}
```

```
a := make([]int, 1)
// a == []int{0}
a = append(a, 1, 2, 3)
// a == []int{0, 1, 2, 3}
```