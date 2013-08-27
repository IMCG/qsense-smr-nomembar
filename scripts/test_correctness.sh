#!/bin/bash

source scripts/lock_exec;
source scripts/config;

for bin in $(ls ./bin/*);
do
    echo "Testing: $bin";
    $bin -n$max_cores -l4 | grep "expected";
    $bin -n$max_cores -l4 -i32 -r64 | grep "expected";
    $bin -n$max_cores -l2 -i16 -r32 -u100 | grep "expected";
done;

source scripts/unlock_exec;
