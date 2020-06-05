CXXFLAGS=-std=c++17 -O3
SHELL=/bin/bash

sudoku-solver: sudoku-solver.cpp

check: sudoku-solver
	@exitval=0; \
	for ifile in tests/*.in; do \
	  echo "checking $$ifile"; \
	  ofile=$${ifile%.in}.out; \
	  if ! test -f $$ofile; then \
	    echo "results file $$ofile missing"; \
	  elif ! diff -u $$ofile <(./sudoku-solver <$$ifile); then \
	    echo "test $$ifile failed"; \
	    exitval=1; \
	  fi; \
	done; \
	exit $$exitval
