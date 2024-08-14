# Category:

## Morphisms:
- "Arrows" between categories
- "functions"
```
B f(A a);
C g(B b);
C g_after_b(A a) {
    return g(f(a));
}
```

## Properties of composition:
1. Composition is associative:
- f((g(h(x)))) = f(g(h(x))) - notice the parenthases around g in the first
2. For every object A there is a unit morphism 
- f(x) = x
- Universally polymorphic (applies to all types). Lets you do:
- `template <class T> T id(T x) { return x; }`

## Composition is the Essence of Programming
- Square cube law:
    - We want to write functions in a way so that as we add few small functions, our program behavior grows faster