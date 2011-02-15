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

rm sccfinder2

make sccfinder2
check_error

rm resultfile
./sccfinder2 input/mytest.txt > resultfile

diff resultfile correctoutput/mytestout.txt
validate_results

for (( i=1; i<=10; i++ ))
do
  rm resultfile

  time ./sccfinder2 "input/network${i}.txt" > resultfile
  diff resultfile "correctoutput/network${i}_output.txt"
  validate_results
done

