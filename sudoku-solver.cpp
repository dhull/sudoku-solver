// Sudoku solver with input/outputs suggested by:
// https://practice.geeksforgeeks.org/problems/solve-the-sudoku/0

// Copyright 2020 David Hull.  All rights reserved.
// Licensed under the MIT license, as described in the accompanying LICENSE file.

#include <iostream>
#include <string.h>

constexpr int n = 9;
constexpr int nn = 3;
typedef short grid_t[n][n];

long guesses = 0;

typedef struct {
    grid_t grid;
    grid_t notes;
} state_t;

// Find the bit that is set if there is a single bit, 0 otherwise.
typedef char findbit_t[0x200]; // == [1 << n].
findbit_t findbit;

void
initialize_findbit() {
    for (int i = 0; i < n; ++i) {
        findbit[1 << i] = i + 1;
    }
}

void
print_grid(const grid_t &grid) {
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
print_notes(const grid_t &notes) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (notes[i][j] >= 0x200) {
                std::printf("bad notes[%d][%d] = %x\n", i, j, notes[i][j]);
            }
            for (int k = 0; k < 9; k++) {
                if (notes[i][j] & (1 << k)) { std::cout << (k+1); } else { std::cout << ' '; }
            }
            std::cout << (j == n-1 ? "\n" : " | ");
        }
        if (i % nn == nn - 1 && i != n - 1) {
            std::cout << "----------+----------------------------------------------------------------------------------------------\n";
        }
    }
    std::cout << '\n';
}

inline int valmask(int v) { return 1 << (v - 1); }

void
update_notes(grid_t &notes, int i, int j, int v) {
    // a and b are top-left corner of square containing i, j
    int a = (i / nn) * nn;
    int b = (j / nn) * nn;

    int vmask = ~ valmask(v) & 0x1ff;
    // std::printf("  vmask[%d][%d] = %d -> %x\n", i, j, v, vmask);

    for (int k = 0; k < n; ++k) {
        // std::printf("  vmask: %x %x -> %x\n", vmask, notes[i][k], notes[i][k] & vmask);
        notes[i][k] &= vmask;                   // propagate to row.
        notes[k][j] &= vmask;                   // propagate to column.
        notes[a + (k % nn)][b + (k / nn)] &= vmask;     // propagate to square.
    }
}

void
initialize_notes(grid_t &notes, const grid_t &grid) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            notes[i][j] = grid[i][j] > 0 ? 0 : 0x1ff;
        }
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (grid[i][j] > 0) {
                update_notes(notes, i, j, grid[i][j]);
            }
        }
    }
}

bool
search(state_t &state, int i, int j) {
    for ( ; i < n; ++i) {
        for ( ; j < n; ++j) {
            if (state.grid[i][j] == 0) {
                goto found_blank;
            }
        }
        j = 0;
    }
    // If we get here the sudoku is solved.
    return true;

found_blank:
    int bits = state.notes[i][j];

#if 1
    int bits_set = __builtin_popcount(bits);
    if (bits_set > 1) {
        guesses += bits_set;
    }
#endif

    for (int k = 0; k < n; ++k) {
        if (bits & (1 << k)) {
            state_t search_state = state;
            int v = k + 1;
            search_state.grid[i][j] = v;
            update_notes(search_state.notes, i, j, v);
            if (search(search_state, i, j)) {
                state = search_state;
                return true;
            }
        }
    }
    return false;
}


void
solve(grid_t &grid) {
    grid_t notes;

    initialize_notes(notes, grid);

    // Update grid based on notes.
    int found_update = 1;
    while (found_update) {
        found_update = 0;

        // print_notes(notes);

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (grid[i][j] == 0) {
                    int v = findbit[notes[i][j]];
                    // std::printf("  grid[%d][%d] = %d has notes:%x bit:%d\n", i, j, grid[i][j], notes[i][j], v);
                    if (v > 0) {
                        // std::printf("setting %d %d to %d for %x\n", i, j, v, notes[i][j]);
                        grid[i][j] = v;
                        update_notes(notes, i, j, v);
                        found_update = 1;
                    }
                }
            }
        }
    }

    // print_grid(grid);

    // Now do search to find missing values.
    state_t state;
    memcpy(&state.grid, &grid, sizeof grid);
    memcpy(&state.notes, &notes, sizeof notes);
    search(state, 0, 0);

    memcpy(&grid, &state.grid, sizeof grid);
}


int main(void) {
    initialize_findbit();

    int numtests = 0;
    std::cin >> numtests;

    for (int t = 0; t != numtests; ++t) {
        guesses = 0;
        grid_t grid;
        for (int i = 0; i < n * n; ++i) {
            int v;
            std::cin >> v;
            grid[i / n][i % n] = v;
        }

        // print_grid(grid);
        solve(grid);
        print_grid(grid);
        // print_grid_linear(grid);
        std::printf("guesses: %ld\n", guesses);
    }
    return 0;
}
