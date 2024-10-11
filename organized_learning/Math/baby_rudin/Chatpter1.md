# Cool Idea: Generate sentences from theorems.
# Table it for now
{
    let A: Set
    let x: A
    then "x is a member of A"
    then "x is an element of A"
    # Example:
    {
        let b: Foo 
        let c: b
        c is a member of b
    }
}
{
    let A: Set
    let x!: A 
    "x is not a member of A"
}
{ 
    A: Set 
    for all x: Any  
        x!: A
    then "A has no elements"
}
{
    A: Set
    A has no elements # References above
    "A is the empty set"
}
{
    let A: Set 
    let B: Set
    for all x: A 
        x: B
    then "A is a subset of B", "A \subset B"
    exists x: B
        x!: A
    then "A is a proper subset of B"
}

{
    let A: Set 
    let B: Set 
    A is a subset of B 
    B is a subset of A 
    then "A = B"
    not then "A != B"
}

theorem:
{
    for all A: Set 
        A is a subset of A
    proof:
    {
        for all x: A 
            x: A
    }
}


















