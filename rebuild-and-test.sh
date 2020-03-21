#!/bin/bash
set -e
shopt -s globstar
make clean
make
make tests
set +e

TESTS_OK="true"

cd tests
rm out.*
for file in *.out*
do
	echo "[====================] $file"
	./$file
	if [ $? -ne 0 ]; then
	TESTS_OK="false"
	fi
done
cd ../

if [ "$TESTS_OK" = "false" ]; then
	echo "Tests have errors, exiting"
	exit 1
fi

mkdir -p docs/coverage
lcov -c --no-external -d . -o docs/coverage/coverage.info
genhtml -o docs/coverage docs/coverage/coverage.info
