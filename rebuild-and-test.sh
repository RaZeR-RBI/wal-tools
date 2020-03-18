#!/bin/bash
set -e
shopt -s globstar
make clean
make
make tests
gcc --version
gcov --version
for file in tests/*.out*
do
	echo "[====================] $file"
	$file
done

mkdir -p docs/coverage
lcov -c --no-external -d . -o docs/coverage/coverage.info
genhtml -o docs/coverage docs/coverage/coverage.info
