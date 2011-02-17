#!/bin/bash

function check_error {
  if [[ $? -ne 0 ]]; then
    echo "ERROR"
    exit 1
  fi
    echo "PASSED"
}

rm a b

make opt
make random
./random 10000 50000 tinybig
python pythscc.py tinybig > a
./scc tinybig > b
diff a b
check_error

rm a b tinybig
