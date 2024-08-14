Recursive function is the last function called
```
// An example of tail recursive function

void print(int n)
{
	if (n < 0)
		return;
	printf("%d ", n);

	// The last executed statement is recursive call
	print(n - 1);
}
```

- Better than non tail recursive:
	- Can be optimized by the compiler:

Consider non tail recursive function:
```javascript
function recsum(x) {
    if (x === 0) {
        return 0;
    } else {
        return x + recsum(x - 1);
    }
}
```

It would have to evaluate to the root before it can actually add elements:
```javascript
recsum(5)
5 + recsum(4)
5 + (4 + recsum(3))
5 + (4 + (3 + recsum(2)))
5 + (4 + (3 + (2 + recsum(1))))
5 + (4 + (3 + (2 + (1 + recsum(0)))))
5 + (4 + (3 + (2 + (1 + 0))))
5 + (4 + (3 + (2 + 1)))
5 + (4 + (3 + 3))
5 + (4 + 6)
5 + 10
15
```

With tail recursion, you can forget about the other data that would otherwise be stored on the stack:
```javascript
function tailrecsum(x, running_total = 0) {
    if (x === 0) {
        return running_total;
    } else {
        return tailrecsum(x - 1, running_total + x);
    }
}
```
```javascript
tailrecsum(5, 0)
tailrecsum(4, 5)
tailrecsum(3, 9)
tailrecsum(2, 12)
tailrecsum(1, 14)
tailrecsum(0, 15)
15
```