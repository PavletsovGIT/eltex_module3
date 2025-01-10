#!/bin/bash

clean_trash=$(rm -rf /tmp/sem-* ./shared-*)

$clean_trash

make clean
make

./prog.out

$clean_trash

./prog.out

$clean_trash