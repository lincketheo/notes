[[0 5 0 0]
 [0 0 0 8]
 [3 0 0 0]
 [0 6 0 0]]

## COO (Coordinate Format)

-   **Data**:
    -   Row = [0, 1, 2, 3]
    -   Col = [1, 3, 0, 1]
    -   Values = [5, 8, 3, 6]

### Case Study to Motivate CS
    - row = [0, 0, 0, 1, 1...] = number of elements
    - col = [0, 3, 4, 1, 4...] = number of elements
    - Can we also compress 0, 0, 0?
    - alternative:
        - col = [0, 3, 4, 1, 4...] = number of elements
        - row = [0, 3, ...] = number of rows + 1
        - Need one last row because bottom rows could be empty and there's no columns / values

## CSR (Compressed Sparse Row Format)

-   **Data**:
    -   Values = [5, 8, 3, 6]
    -   Col Indices = [1, 3, 0, 1]
    -   Row Ptr = [0, 1, 2, 3, 4]
-   **Notes**:
    -   Efficient for row based operations

## CSC (Compressed Sparse Column Format)

-   **Data**:
    -   Values = [3, 5, 6, 8]
    -   Row Indices = [2, 0, 3, 1]
    -   Col Ptr = [0, 1, 3, 3, 4]
-   **Notes**:
    -   Efficient for column based operations

## Diagonal Storage Format

-   **Data**:
    -   Main diagonal (d0) = [0, 0, 0, 0]
    -   Diagonal below main (d-1) = [3, 6, 0]
    -   Diagonal above main (d1) = [5, 8]

## Block Sparse Row (BSR) Format

-   **Data** (2x2 block size):
    -   Block Values = [[0, 5], [0, 0]], [[0, 0], [0, 8]], [[3, 0], [0, 6]]
    -   Block Indices = [0, 1, 0]
    -   Block Ptr = [0, 2, 3]

## List of Lists (LIL) Format

-   **Data**:
    -   Row 0: [(1, 5)]
    -   Row 1: [(3, 8)]
    -   Row 2: [(0, 3)]
    -   Row 3: [(1, 6)]

## Dictionary of Keys (DOK) Format

-   **Data**:
    -   {(0, 1): 5, (1, 3): 8, (2, 0): 3, (3, 1): 6}

## Hybrid (HYB) Format

-   **Data**:
    -   ELL:
        -   Values = [[5, 0], [8, 0], [3, 0], [6, 0]]
        -   Col Indices = [[1, -1], [3, -1], [0, -1], [1, -1]]
    -   COO (for overflow): Empty in this example.

## Quadtree Format

-   **Data**:
    -   Divide into quadrants:
        -   Top-left: [[0, 0], [0, 0]]
        -   Top-right: [[5, 0], [0, 8]]
        -   Bottom-left: [[3, 0], [0, 6]]
        -   Bottom-right: [[0, 0], [0, 0]]
