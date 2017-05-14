#!/bin/sh

files=

for f in `ls *.c`
do
    files+="-file-${f} "
done

/usr/local/bin/gc $files

chmod +r *.c

