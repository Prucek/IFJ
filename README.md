# IFJ Project

Implementation of IFJ20 compiler.
IFJ20 is a simplified version of the programming language Go.

## Authors

* Rebeka Cernianska, xcerni13
* Matej Jurik, xjurik12
* Marek Micek, xmicek08
* Peter Rucek, xrucek00

## Testing

All module tests are stored inside of `~/tests` subfolder

To run all tests, use:

```bash
make compiler-test
```

*note that using `./run_tests.sh` instead using the command above won't work

### Testing Conventions

* Do not create any files starting with `test_` inside of the `~/tests` subfolder
* In cases like failed malloc, allways use `testerror(TESTERROR_*)` to exit testing
* To represent a failing test, always use `testfail(TESTFAIL_*)` where `TESTFAIL_*` represents a failure message
* Add customized test failure messages by defining a new `TESTFAIL_*` macros
* Add customized test cases by implementing new `test_*` functions inside of respective test suites


## Job Pool

 * return param - Wimko
 * i,j = j+1, i+1 - Petrik
 * for vo fore - Rebeka
 * zatieňovanie - Micko
## Be free to choose
 * (5==5) >0
 * rekurizvne volanie
 * assignment vo fore
 * inicialization of arr in MACRO
 * for ;; 

<!-- ## Bugs -->
