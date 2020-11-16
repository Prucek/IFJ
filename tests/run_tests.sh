#!/usr/bin/env bash


###########################################
# TODO
#
# output failures in run_tests.sh
# globalize a testlib.h interface to enforce standardized test suites
###########################################



# Config & funcs

green="[1;32m"
red="[1;31m"

print() {
    printf "\033$1"
    echo $2
    printf "\033[0m"
}

# Change process working dir to ./tests 
cd ./tests 

make -s clean
make -s

# Test execution & parsing output
tests=(`find ./test_* -type f -print`)
echo "Running module tests: ${tests[@]}"
echo

for test in ${tests[@]}; do
    echo -n "Testing ${test:2}: "

    outfile="./$test.out"
    $test 2>$outfile # Execute test

    curr_errors=`grep TESTERROR $outfile`
    if [[ -z $curr_errors ]]; then

        curr_fails=`grep TESTFAIL $outfile`
        if [[ -z $curr_fails ]]; then 
            # Test ran successfully
            print $green "OK!"
        else
            # Test failed
            print $red "FAILED!"
            # Print current test's failures
            echo -ne "\t"
            echo $curr_fails
        fi

    else
        # An error occured inside of test
        print $red "FAILED!"
        # Print current test's errors
        echo -ne "\t"
        echo $curr_errors
    fi
done 

make -s clean
rm -f test_*.out

# Exit out of ./tests
cd ..
