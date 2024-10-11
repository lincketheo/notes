- Just had some good ideas, I want a tool that will help me spin those thoughts back up because it's easy to loose them. 
- Capture the "state" of my mind and condense it into all the meaningful tidbits of information.
- Work your way back and repeat the knowledge that you just obtained.
- What is a "meaningful thought" - you could quantify this:
    - Example: A tidbit worth saving is formulating a problem to solve.
    - I formulate a problem to solve that is relevant to the thought I had and I word it so that later, when my mind is in a powerful "solving" state, I can spend time to solve it.
    - Good questions to ask 
    - Reduce duplicate information in your knowledge base - every note is unique
    - but you also need it so that every note is meaningful 
    - A sentence with extra fluff can trigger better responses than a pure sentence (see e.g. mathematics vs other sciences)

https://arxiv.org/abs/2306.11644

Idea:
    - Rank a sentence based on how much "meaningfulness" it has in it 
        - Note: It would be cool if you could actually reduce the whole sentence to first order logic, but that's hard 
    - Use that ranking as a weight to a machine learning algorithm

Thought Experiment
1. The cat eats the dog, therefore the dog is dead
2. The sheep eats the cow, therefore the cow is dead

Deductions:
1. There is a cat, dog, sheep, cow (4 facts)
5. The BLANK 1 eats the BLANK 2, therefore the BLANK 2 is dead
If you see it twice, make it a rule
If the rule breaks, make another rule

Given:
a b c d e 
a j c k e

Try to generate new facts:
a l c m e?
a z c p e?

ab 

If a then b 
If b then c 
Therefore if a then c

> I am a ghost. All ghosts wear hats. Do I wear a hat?
> Valid(I am a ghost. All ghosts wear hats. I wear a hat)
> Valid((exists ghost(I). For all X st ghost(X), wearshats(X). wearshats(I)))



