#!/bin/bash

make cleanobj

make EFFECTIVE=on 2> build.log

cat build.log | grep -v boost | grep -vE "^ *from" | less
