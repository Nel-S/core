# Core

These files act as a foundation for C/C++ bruteforce programs involving (up to 64-bit) integers. They implement many common program tasks (multithreading, iterating over integers to check, writing results to a desired output, etc.) so the programs do not need to implement them themselves.

## Usage Instructions
1. If using Git, clone the repository via `git clone`. Otherwise, click Code -> Download ZIP to download the repository, then extract the resultant compressed file.
2. In your bruteforce program, add the header `#include "core/bruteforce.h"`, and implement the constants, global variables, and functions listed below.
3. When compiling your program, in addition to your other filenames and flags, add `"core/Backends/<the backend to run your program on>" -I "<your/system's/path/to/the/downloaded/core/directory>"`.

## Provisions
This library assumes your program implements the following constants, global variables, and functions.
- `const uint64_t GLOBAL_START_INTEGER` is the (up to 64-bit) integer the program as a whole should begin searching from, if an incremental search is desired. (The value is unused if reading from a file.)
- `const uint64_t GLOBAL_NUMBER_OF_INTEGERS` is the number of integers the program as a whole should check, if an incremental search is desired. (The value is unused if reading from a file.)
- `const int GLOBAL_NUMBER_OF_WORKERS` is the number of workers the program should use while running. Depending on one's backend, these can take the form of threads or processes.
- `const char *INPUT_FILEPATH` is the filepath to retrieve (signed 64-bit) integers to check from, or `NULL` if an incremental search is desired instead.
- `const char *OUTPUT_FILEPATH` is the filepath to write outputs to, or `NULL` if outputs should be written to stdout instead. (Note that if a filepath already exists, it will be overwritten.)
- `const bool TIME_PROGRAM` is `true` if the program's execution should be timed and outputted to `stderr` before the program terminates, or `false` otherwise.
- `uint64_t localStartInteger`, `uint64_t localNumberOfIntegers`, and `int localNumberOfWorkers` are variable equivalents of `GLOBAL_START_INTEGER`, `GLOBAL_NUMBER_OF_INTEGERS`, and `GLOBAL_NUMBER_OF_WORKERS` (respectively) that are assigned to each worker individually, meaning they are customizable per worker. While these can be set manually, the keyword `DEFAULT_LOCALS_INITIALIZATION` will set them to `GLOBAL_START_INTEGER`, `GLOBAL_NUMBER_OF_INTEGERS`, and `GLOBAL_NUMBER_OF_WORKERS`, respectively, for all workers.
- `int initializeGlobals()` will be called prior to spawning any workers. Its intended use case is for initializing any runtime-dependent global variables. It should return `0` upon success, or a non-zero error code otherwise.
- `void *runWorker(const void *const workerIndex)` will be called to begin running the specified code for each worker.

In return, the library provides the following helper functions or preprocessor keywords:
- `bool getNextInteger(const void *const workerIndex, uint64_t *const integer)` will attempt to fetch the next value for a worker to check. If `workerIndex` is not `NULL`, the very first value for the worker of that index will be saved to `integer`; otherwise `integer` will be updated with the next value in sequence or the next value from the input file. Returns `false` if no more values to check exist for that worker, or `true` otherwise.
- `void outputString(const char *const format, ...)` accepts the same input format as `printf()`, but instead outputs the values to either `stdout` or `OUTPUT_FILEPATH` as specified above.
- `CHECK_THIS_INTEGER_AND_FOLLOWING(integer, width)`, when given a starting `integer`, will expand into the number of integers remaining in the unsigned `width`-bit integer space.
- `USE_CUSTOM_GET_NEXT_INTEGER`, if `#define`d, will redirect calls made to `getNextInteger(const void *, uint64_t *)` to a custom implementation defined by your program instead.
- `USE_CUSTOM_OUTPUT_STRING`, if `#define`d, will redirect calls made to `outputString(const char *, ...)` to a custom implementation defined by your program instead.

# License
This repository is offered under [my (NelS') general license](./LICENSE). Please read and abide by that text if you have any wishes of referencing, distributing, selling, etc. this repository or its code.

Otherwise, any and all good-faith contributions are appreciated.