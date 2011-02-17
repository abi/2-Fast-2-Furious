#!/bin/bash

function check_error {
  if [[ $? -ne 0 ]]; then
    echo "ERROR"
    exit 1
  fi
    echo "done"
}

make sccfinder2
make scc

check_error

time ./sccfinder2 BIGGEST
time ./scc BIGGEST

time ./scc BIGGEST
time ./sccfinder2 BIGGEST

diff a b

if [[ $? -ne 0 ]]; then
  cat b
fi
rm a b
