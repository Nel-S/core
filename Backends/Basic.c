#include "../bruteforce.h"

FILE *inputFile = NULL, *outputFile = NULL;

#ifndef USE_CUSTOM_GET_NEXT_INTEGER
COMMON_NODISCARD bool getNextInteger(const void *const workerIndex, uint64_t *const integer) {
	// Normally fscanf is non-threadsafe, but we're only using one process anyways
	// TODO: Support arbitrary characters following entires in input list, as fault tolerance?
	// TODO: Also support scanning unsigned 64-bit integers?
	if (INPUT_FILEPATH) return fscanf(inputFile, " %" SCNdFAST64 " \n", COMMON_REINTERPRET_CAST(int_fast64_t *, integer)) == 1;
	*integer = workerIndex ? COMMON_STATIC_CAST(uint64_t, *COMMON_STATIC_CAST(const int *, workerIndex)) + localStartInteger : *integer + COMMON_STATIC_CAST(uint64_t, localNumberOfWorkers);
	return *integer - localStartInteger < localNumberOfIntegers;
}
#endif

#ifndef USE_CUSTOM_OUTPUT_STRING
void outputString(const char *format, ...) {
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
	initializeGlobals();
	if (INPUT_FILEPATH) {
		inputFile = fopen(INPUT_FILEPATH, "r");
		if (!inputFile) RAISE_EXCEPTION_OR_QUIT("core/Backends/Basic.c: main(): fopen(INPUT_FILEPATH, \"r\"): Failed to open %s.\n", INPUT_FILEPATH);
	}
	if (OUTPUT_FILEPATH) {
		outputFile = fopen(OUTPUT_FILEPATH, "a");
		if (!outputFile) RAISE_EXCEPTION_OR_QUIT("core/Backends/Basic.c: main(): fopen(OUTPUT_FILEPATH, \"w\"): Failed to open %s.\n", OUTPUT_FILEPATH);
	}
	struct timespec startTime, endTime;
	if (TIME_PROGRAM) clock_gettime(CLOCK_MONOTONIC, &startTime);
	int threadIndex = 0;
	runWorker(&threadIndex);
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