# IFJ Project

Implementation of IFJ20 compiler.
IFJ20 is a simplified version of the programming language Go.

## Authors

* Rebeka Cernianska, xcerni13
* Matej Jurik, xjurik12
* Marek Micek, xmicek08
* Peter Rucek, xrucek00

## Assignemnt

 * return param + return ma aj ine problemy (/suspected/5,6,7.go) - Wimko
 * for vo fore - Rebeka
 * zatieňovanie - Micko
 * for ;; - Petrik
 * assignment vo fore - Petrik

### Job pool
 * (5==5) >0
 * rekurizvne volanie

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


<!-- ## Bugs -->
