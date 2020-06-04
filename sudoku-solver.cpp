// Sudoku solver with input/outputs suggested by:
// https://practice.geeksforgeeks.org/problems/solve-the-sudoku/0

// Copyright 2020 David Hull.  All rights reserved.
// Licensed under the MIT license, as described in the accompanying LICENSE file.

#include <iostream>
#include <assert.h>

constexpr int n = 9;
constexpr int nn = 3;

// The (partial) solution.  Each cell contains 1-9 if the digit is known or 0
// if it is unknown.
typedef char grid_t[n][n];      // 0..9.
// A bitmask 1 << (n-1) if n is a possible digit for the cell.
typedef unsigned short notes_t[n][n];   // 0..0x1ff.

long guesses = 0;

class sudoku {
    grid_t grid;
    notes_t notes;
public:
    sudoku();
    bool solve();
    void set_cell(int i, int j, int v);
    bool search(int i, int j);
    void print_grid() const;
    void print_notes() const;
};

sudoku::sudoku() : grid {0} {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            notes[i][j] = 0x1ff;
        }
    }
}

// Helper for sudoku notes: when a notes entry has a single bit set, return
// that bit.  Otherwise, zero or more than one bit is set, return 0.
class findbit {
    char setbit[1 << n];
public:
    findbit() : setbit {0} {
        for (int i = 0; i < n; ++i) {
            setbit[1 << i] = i + 1;
        }
    }
    // Returns the 1-based bit that is set if there is a single bit, 0
    // otherwise.
    int operator()(int i) const {
        assert(i < sizeof setbit);
        return setbit[i];
    }
};

findbit findbit;

int
unknown_count(const grid_t &grid) {
    int unknowns = 0;
    for (int i = 0; i < n * n; i++) {
        if (grid[i / n][i % n] == 0) {
            ++unknowns;
        }
    }
    return unknowns;
}

void
sudoku::print_grid() const {
    for (int i = 0; i < n * n; ++i) {
        std::cout << static_cast<int>(grid[i / n][i % n]) << (i % n == n-1 ? '\n' : ' ');
    }
    std::cout << '\n';
}

void
print_grid_linear(const grid_t &grid) {
    for (int i = 0; i < n * n; ++i) {
        std::cout << static_cast<int>(grid[i / n][i % n]) << ' ';
    }
    std::cout << '\n';
}

void
sudoku::print_notes() const {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            assert(notes[i][j] < 0x200);
            for (int k = 0; k < 9; k++) {
                std::cout << static_cast<char>((notes[i][j] & (1 << k)) ? '1' + k : ' ');
            }
            std::cout << (j == n-1 ? "\n" : " | ");
        }
        if (i % nn == nn - 1 && i != n - 1) {
            std::cout << "----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+----------\n";
        }
    }
    std::cout << '\n';
}

inline int valmask(int v) { return 1 << (v - 1); }

void
sudoku::set_cell(int i, int j, int v) {
    if (v == 0) { return; }

    // Update the cell value.
    grid[i][j] = v;
    notes[i][j] = 0;

    // a and b are top-left corner of square containing i, j
    int a = (i / nn) * nn;
    int b = (j / nn) * nn;

    // Update the notes for the cells row, column, and square.
    int vmask = ~ valmask(v) & 0x1ff;
    for (int k = 0; k < n; ++k) {
        // std::printf("  vmask: %x %x -> %x\n", vmask, notes[i][k], notes[i][k] & vmask);
        notes[i][k] &= vmask;                   // propagate to row.
        notes[k][j] &= vmask;                   // propagate to column.
        notes[a + (k % nn)][b + (k / nn)] &= vmask;     // propagate to square.
    }
}


bool
sudoku::search(int i, int j) {
    // Find a blank cell starting from grid[i][j].
    for ( ; i < n; ++i) {
        for ( ; j < n; ++j) {
            if (grid[i][j] == 0) {
                goto found_blank;
            }
        }
        j = 0;
    }
    // If we get here the sudoku is solved.
    return true;

found_blank:
    int bits = notes[i][j];

    // Fast fail: if bits is 0 then we have found a blank cell but there are
    // no allowed candidates for it.
    if (bits == 0) {
        return false;
    }

    // For each of the allowed candidates for this cell, set its value to the
    // candidate and then attempt to solve the rest of the puzzle.
    for (int k = 0; k < n; ++k) {
        if (bits & (1 << k)) {
            sudoku search_state = *this;
            int v = k + 1;
            search_state.set_cell(i, j, v);
            if (search_state.search(i, j)) {
                *this = search_state;
                return true;
            }
            ++guesses;
        }
    }
    return false;
}


bool
sudoku::solve() {

#if 1
    std::printf("unknowns before constraint propagation: %d\n", unknown_count(grid));
#endif

    // Make one or more passes attempting to update the grid with values we
    // are certain about based on initial notes.  For puzzles that are not too
    // difficult this will find some values and reduce the amount of searching
    // we have to do.
    int found_update = 1;
    while (found_update) {
        found_update = 0;

        // print_notes();

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (grid[i][j] == 0) {
                    int v = findbit(notes[i][j]);
                    if (v > 0) {
                        set_cell(i, j, v);
                        found_update = 1;
                    }
                }
            }
        }
    }

    // print_grid();

#if 1
    std::printf("unknowns after constraint propagation: %d\n", unknown_count(grid));
#endif

    // Now do search to find missing values.
    bool found = search(0, 0);

    return found;
}


int main(void) {
    int numtests = 0;
    std::cin >> numtests;

    for (int t = 0; t != numtests; ++t) {
        guesses = 0;
        sudoku sudoku;
        for (int i = 0; i < n * n; ++i) {
            int v;
            std::cin >> v;
            if (v < 0 || v > 9) {
                std::printf("Bad grid[%d][%d] value %d\n", i / n, i % n, v);
                exit(1);
            }
            sudoku.set_cell(i / n, i % n, v);
        }

        // sudoku.print_grid();
        sudoku.solve();
        sudoku.print_grid();
        // sudoku.print_grid_linear();
        std::printf("guesses: %ld\n", guesses);
    }
    return 0;
}
