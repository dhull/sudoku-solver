<!-- -*- mode:gfm; word-wrap:nil; indent-tabs-mode:nil -*- github-flavored markdown -->

# Sudoku Solver

This is a simple [sudoku](https://en.wikipedia.org/wiki/Sudoku) solver that I
wrote as a solution to [a Geeks for Geeks
problem](https://practice.geeksforgeeks.org/problems/solve-the-sudoku/0) and
then cleaned up a bit to publish here.  It takes input in the form

```
N
A1 A2 A3 A4 A5 A6 A7 A8 A9
B1 B2 B3 B4 B5 B6 B7 B8 B9
C1 C2 C3 C4 C5 C6 C7 C8 C9
D1 D2 D3 D4 D5 D6 D7 D8 D9
E1 E2 E3 E4 E5 E6 E7 E8 E9
F1 F2 F3 F4 F5 F6 F7 F8 F9
G1 G2 G3 G4 G5 G6 G7 G8 G9
H1 H2 H3 H4 H5 H6 H7 H8 H9
I1 I2 I3 I4 I5 I6 I7 I8 I9
```

where N is the number of test cases and each test case has a list of cells A-I
1-9 with a value `1`-`9`, or `0` if the cell is blank.

Comments in the input that begin with `#` and extend to the end of the line
are ignored.

## Sample Output

```
$ time ./sudoku-solver -v <<'EOF'
> 1
> # Puzzle from https://en.wikipedia.org/wiki/Mathematics_of_Sudoku#Constraints_of_clue_geometry
> 0 0 6 7 0 3 5 0 0
> 0 0 0 0 4 0 0 0 0
> 5 0 0 0 0 0 0 0 2
> 9 0 0 0 0 0 0 0 7
> 0 3 0 0 0 0 0 4 0
> 8 0 0 0 0 0 0 0 1
> 1 0 0 0 0 0 0 0 4
> 0 0 0 0 0 0 0 0 0
> 0 5 9 2 6 7 3 1 0
> EOF
unknowns before constraint propagation: 59
unknowns after constraint propagation: 52
guesses: 18777
2 4 6 7 1 3 5 8 9
7 9 8 5 4 2 1 6 3
5 1 3 6 8 9 4 7 2
9 2 4 1 5 6 8 3 7
6 3 1 9 7 8 2 4 5
8 7 5 3 2 4 6 9 1
1 6 7 8 3 5 9 2 4
3 8 2 4 9 1 7 5 6
4 5 9 2 6 7 3 1 8

real    0m0.006s
user    0m0.002s
sys     0m0.002s
```
