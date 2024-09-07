#include "../common_seedfinding.h"

FILE *inputFile = NULL, *outputFile = NULL;

#ifndef USE_CUSTOM_GET_NEXT_SEED
NO_DISCARD bool getNextSeed(const void* workerIndex, uint64_t *seed) {
	// Normally fscanf is non-threadsafe, but we're only using one process anyways
	// TODO: Support arbitrary characters following entires in seedlist, as fault tolerance?
	if (INPUT_FILEPATH) return fscanf(inputFile, " %" SCNdFAST64 " \n", STATIC_CAST(int_fast64_t *, seed)) == 1;
	*seed = workerIndex ? *STATIC_CAST(int *, workerIndex) + localStartSeed : *seed + localNumberOfWorkers;
	return *seed - localStartSeed < localSeedsToCheck;
}
#endif

#ifndef USE_CUSTOM_OUTPUT_VALUES
void outputValues(const char *format, ...) {
	va_list args;
	va_start(args, format);
	// Normally vfprintf is non-threadsafe, but we're only using one process anyways
	vfprintf(OUTPUT_FILEPATH ? outputFile : stdout, format, args);
	// TODO: Does stdout need to be flushed?
	/* if (OUTPUT_FILEPATH) */ fflush(OUTPUT_FILEPATH ? outputFile : stdout);
	va_end(args);
}
#endif

int main() {
	initGlobals();
	if (INPUT_FILEPATH) {
		inputFile = fopen(INPUT_FILEPATH, "r");
		if (!inputFile) RAISE_EXCEPTION_OR_QUIT("Backends/Basic.c: main(): fopen(INPUT_FILEPATH, \"r\"): Failed to open %s.\n", INPUT_FILEPATH);
	}
	if (OUTPUT_FILEPATH) {
		outputFile = fopen(OUTPUT_FILEPATH, "w");
		if (!outputFile) RAISE_EXCEPTION_OR_QUIT("Backends/Basic.c: main(): fopen(OUTPUT_FILEPATH, \"w\"): Failed to open %s.\n", OUTPUT_FILEPATH);
	}
	struct timespec startTime, endTime;
	if (TIME_PROGRAM) clock_gettime(CLOCK_MONOTONIC, &startTime);
	int data = 0;
	runWorker(&data);
	if (INPUT_FILEPATH) fclose(inputFile);
	if (OUTPUT_FILEPATH) {
		fflush(outputFile);
		fclose(outputFile);
	}
	if (TIME_PROGRAM) {
		clock_gettime(CLOCK_MONOTONIC, &endTime);
		fprintf(stderr, "(%.9g seconds)\n", endTime.tv_sec - startTime.tv_sec + (endTime.tv_nsec - startTime.tv_nsec)/1e9);
	}
	return 0;
}