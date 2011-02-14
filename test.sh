#!/bin/bash

function check_error {
  if [[ $? -ne 0 ]]; then
    echo "ERROR"
    exit 1
  fi
    echo "done"
}

function validate_results {
  if [[ $? -ne 0 ]]; then
    echo "FAILED"
    return
  fi
    echo "passed"
}

make
check_error

for (( i=1; i<=10; i++ ))
do
  rm resultfile

  time ./sccfinder input/network$i.txt > resultfile
  diff resultfile correctoutput/network$i"_output.txt"
  validate_results
done

