#ifndef CORE_BRUTEFORCE_H
#define CORE_BRUTEFORCE_H

#include "common.h"
#ifdef __cplusplus
	#include <cstdarg>
	#include <ctime>
#else
	#include <stdarg.h>
	#include <time.h>
#endif

// These are constants that must be set in the settings for each program.
extern const uint64_t GLOBAL_START_INTEGER, GLOBAL_NUMBER_OF_INTEGERS;
extern const int GLOBAL_NUMBER_OF_WORKERS;
extern const char *INPUT_FILEPATH, *OUTPUT_FILEPATH;
extern const bool TIME_PROGRAM;

// These are global variables that will by default be set to their constant equivalents, but can be modified as needed.
extern uint64_t localStartInteger, localNumberOfIntegers;
extern int localNumberOfWorkers;

// Macro for GLOBAL_NUMBER_OF_INTEGERS/localNumberOfIntegers
#ifndef CHECK_THIS_INTEGER_AND_FOLLOWING
	#define CHECK_THIS_INTEGER_AND_FOLLOWING(startInteger, integerWidth) ((((integerWidth) >= 0 && (integerWidth) < 64 ? UINT64_C(1) << integerWidth : 0) - UINT64_C(1)) - startInteger + (!!startInteger))
#endif

/* Macro for default "local*" variable initializations
   (This must be a macro because the compiler complains about GLOBAL_* being externs otherwise)*/
#ifndef DEFAULT_LOCALS_INITIALIZATION
	// TODO: Multi-line unbracketed macros are generally dangerous. Is there a way to rewrite this?
	#define DEFAULT_LOCALS_INITIALIZATION \
		uint64_t localStartInteger = GLOBAL_START_INTEGER, localNumberOfIntegers = GLOBAL_NUMBER_OF_INTEGERS; \
		int localNumberOfWorkers = GLOBAL_NUMBER_OF_WORKERS;
#endif



#ifdef __cplusplus
extern "C" {
#endif

// This is defined in the filtering program.
// It must initialize all global variables prior to any workers being created.
void initializeGlobals();
// This is defined in the filtering program.
// It must initialize a worker; it is also recommended it
// - call `getNextInteger(workerIndex, &integer)` once to fetch the first integer (or abort if the function returns false),
// - have a do-while loop check each integer against some conditions, calling `outputString()` if the conditions are met; and
// - have the condition for the do-while loop call `getNextInteger(NULL, &integer)` and abort if the function returns false,
// though custom implementations are still allowed if one wishes.
void runWorker(void *const workerIndex);

// This is defined in the main template by default, but can be overridden by #define-ing USE_CUSTOM_GET_NEXT_INTEGER and providing your own implementation.
// If workerIndex is not NULL, it returns the first integer for the specified worker.
// Otherwise if workerIndex is NULL, it returns the next integer in the sequence.
// Returns false when the end of the sequence is reached.
COMMON_NODISCARD bool getNextInteger(const void *const workerIndex, uint64_t *const integer);
// This is defined in the main template by default, but can be overridden by #define-ing USE_CUSTOM_OUTPUT_STRING and providing your own implementation.
// It prints the information for a single result given a setup identical to `printf()`.
void outputString(const char *const format, ...);

#ifdef __cplusplus
}
#endif

#endif