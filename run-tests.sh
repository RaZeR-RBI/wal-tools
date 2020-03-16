#!/bin/bash
shopt -s globstar
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
