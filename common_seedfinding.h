#ifndef _COMMON_SEEDFINDING_H
#define _COMMON_SEEDFINDING_H

#include "C-C++-CUDA Support.h"
#include INCLUDE_STANDARD(stdarg)
#include INCLUDE_STANDARD(stdbool)
#include INCLUDE_STANDARD(time)

// These are constants that must be set in the settings for each program.
extern const uint64_t GLOBAL_START_SEED, GLOBAL_SEEDS_TO_CHECK;
extern const int GLOBAL_NUMBER_OF_WORKERS;
extern const char *INPUT_FILEPATH, *OUTPUT_FILEPATH;
extern const bool TIME_PROGRAM;

// These are global variables that will by default be set to their constant equivalents, but can be modified as needed.
extern uint64_t localStartSeed, localSeedsToCheck;
extern int localNumberOfWorkers;

// Macro for GLOBAL_SEEDS_TO_CHECK/localSeedsToCheck
#ifndef CHECK_THIS_SEED_AND_FOLLOWING
	#define CHECK_THIS_SEED_AND_FOLLOWING(startSeed) UINT64_MAX - startSeed + (!!startSeed)
#endif

/* Macro for default "local*" variable initializations
   (This must be a macro because the compiler complains about GLOBAL_* being externs otherwise)*/
#ifndef DEFAULT_LOCALS_INITIALIZATION
	#define DEFAULT_LOCALS_INITIALIZATION \
		uint64_t localStartSeed = GLOBAL_START_SEED, localSeedsToCheck = GLOBAL_SEEDS_TO_CHECK; \
		int localNumberOfWorkers = GLOBAL_NUMBER_OF_WORKERS;
#endif



#ifdef __cplusplus
extern "C" {
#endif

// This is defined in the filtering program.
// It must initialize all global variables prior to any workers being created.
void initGlobals();
// This is defined in the filtering program.
// It must initialize a worker; it is also recommended it
// - call `getNextSeed(workerIndex, &seed)` once to fetch the first seed (or abort if the function returns false),
// - have a do-while loop check each seed against some conditions, calling `outputValues()` if the conditions are met; and
// - have the condition for the do-while loop call `getNextSeed(NULL, &seed)` and abort if the function returns false,
// though custom implementations are still allowed if one wishes.
void *runWorker(void *workerIndex);

// This is defined in the main template by default, but can be overridden by #define-ing USE_CUSTOM_GET_NEXT_SEED and providing your own implementation.
// If workerIndex is not NULL, it returns the first seed for the specified worker.
// Otherwise if workerIndex is NULL, it returns the next seed in the sequence.
// Returns false when the end of the sequence is reached.
NO_DISCARD bool getNextSeed(const void* workerIndex, uint64_t *seed);
// This is defined in the main template by default, but can be overridden by #define-ing USE_CUSTOM_OUTPUT_VALUES and providing your own implementation.
// It prints the information for a single result given a setup identical to `printf()`.
void outputValues(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif