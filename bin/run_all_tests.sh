#!/bin/sh

# check for type of test
if [ $# -ne 1 ]; then
    echo "Usage: ./run_all_tests.sh [type]"
    echo "Types: scanner, parser, printer"
    exit 1
fi
TYPE=$1
passed=0
failed=0

# set command
if [ $TYPE = "all" ]; then
    ./bin/run_all_tests.sh scanner
    ((passed += $?))
    ./bin/run_all_tests.sh parser
    ((passed += $?))
    ./bin/run_all_tests.sh printer
    ((passed += $?))
    ./bin/run_all_tests.sh typecheck
    ((passed += $?))
    ./bin/run_all_tests.sh codegen
    ((passed += $?))
    echo "Total passed: $passed"
    exit 0
elif [ $TYPE = "scanner" ]; then
    BMINOR="./bminor -scan"
elif [ $TYPE = "parser" ]; then
    BMINOR="./bminor -parse"
elif [ $TYPE = "printer" ]; then
    BMINOR="./bminor -print"
elif [ $TYPE = "resolver" ]; then
    BMINOR="./bminor -resolve"
elif [ $TYPE = "typecheck" ]; then
    BMINOR="./bminor -typecheck"
elif [ $TYPE="codegen" ]; then
    count=0
    for testfile in ./tests/$TYPE/good*.bminor
    do
        ((count++))
        if ./bin/bmake.sh $testfile > /dev/null 2>&1 ; then
            echo "Good Test $count Success!"
            ((passed++))
        else 
            echo "$testfile: Failure!"
            ((failed++))
        fi
    done
    rm out.s prog
    exit $passed
fi

# run tests
count=0
for testfile in ./tests/$TYPE/good*.bminor
do
    ((count++))
    if $BMINOR $testfile > output.txt 2>&1; then
		if [ $TYPE = "printer" ]; then
            $BMINOR output.txt > output2.txt
            if cmp -s output2.txt output.txt > /dev/null 2>&1; then
                echo "Good Test $count success (as expected)"
                ((passed++))
            else
                echo "Good Test $count failure (INCORRECT) ($testfile)"
                ((failed++))
            fi
            rm output2.txt
        else
            echo "Good Test $count success (as expected)"
            ((passed++))
        fi
	else
		echo "Good Test $count failure (INCORRECT) ($testfile)"
        ((failed++))
	fi
done

count=0
for testfile in ./tests/$TYPE/bad*.bminor
do
    ((count++))
	if $BMINOR $testfile > output.txt 2>&1; then
        echo "Bad Test $count success (INCORRECT) ($testfile)"
        ((failed++))
	else
		echo "Bad Test $count failure (as expected)"
        ((passed++))
	fi
done
rm output.txt
exit $passed

