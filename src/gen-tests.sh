#!/bin/sh

funs=$(grep -e "void [a-zA-Z_]*(void)" $1 | cut -c6- | rev | cut -c7- | rev)

echo "int run_all_tests(void)"
echo "{"

for i in $funs
do
    echo "    tz_run($i);"
done

echo ""
echo "    tz_return();"
echo "}"
