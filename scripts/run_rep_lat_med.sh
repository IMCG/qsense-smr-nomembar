#!/bin/bash

source scripts/config;

reps=$1;
shift;
prog="$1";
shift;
params="$@";

un=$(uname -n);
tmp=data/run_rep_lat_max.${un}.tmp
printf "" > $tmp;

for r in $(seq 1 1 $reps);
do
    out=$(timeout 20 $run_script ./$prog $params);
    lat=$(echo "$out" | grep "#thread" -A1 | tail -n1 | awk '{$1=""; print}');
    sum=$(echo "0${lat}" | sed 's/\ /\+/g' | bc); 
    printf "%-10d%-10d%-10d%-10d%-10d%-10d%-10d\n" $sum $lat >> $tmp;
done;

HEAD=head;
TAIL=tail;
if [ "$(uname -n)" = ol-collab1 ];
then
    HEAD=/usr/gnu/bin/head
    TAIL=/usr/gnu/bin/tail
fi;

med_idx=$(echo "1 + $reps/2" | bc);
sort -n $tmp | ${HEAD} -${med_idx} | ${TAIL} -n1 | awk '{$1=""; print}';

