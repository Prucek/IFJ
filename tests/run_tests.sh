#!/usr/bin/env bash


###########################################
# TODO
#
# output failures in run_tests.sh
# globalize a testlib.h interface to enforce standardized test suites
###########################################

# Config & funcs
IFS=$'\n' # Internal Field Separator

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

    outfile="./${test:6}.out"
    $test 2>$outfile # Execute test

    # Split to array elements on linefeeds
    curr_errors=(`grep TESTERROR $outfile`) 

    if [[ ${#curr_errors[@]} -eq 0 ]]; then

        curr_fails=(`grep TESTFAIL $outfile`)

        if [[ ${#curr_fails[@]} -eq 0 ]]; then 
            # Test ran successfully
            print $green "OK!"
        else
            # Test failed
            print $red "FAILED! (${#curr_fails[@]} failures)"
            # Print current test's failures
            for fail in ${curr_fails[@]}; do
                printf "  "
                echo $fail
            done
        fi

    else
        # An error occured inside of test
        print $red "ERROR!"
        # Print current test's errors
        for error in ${curr_errors[@]}; do
                printf "  "
                echo $error
        done
    fi
done

make -s clean
rm -f _*.out

# Exit out of ./tests
cd ..

# Reset IFS to spaces
unset IFS 
