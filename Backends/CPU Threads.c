// #define COMMON_CURRENT_PLATFORM COMMON_PLATFORM_C_STDLIB
// #define COMMON_CURRENT_PLATFORM COMMON_PLATFORM_CPP_STDLIB
// #define COMMON_CURRENT_PLATFORM COMMON_PLATFORM_POSIX
// #define COMMON_CURRENT_PLATFORM COMMON_PLATFORM_WINDOWS

#include "../bruteforce.h"

#if COMMON_CURRENT_PLATFORM == COMMON_PLATFORM_UNKNOWN
	#error "The Core library could not automatically detect your current execution platform, or the execution platform you specified was unsupported. As a result, it could not find the libraries necessary to support mutexes and threads."
#endif

FILE *inputFile = NULL, *outputFile = NULL;
COMMON_MUTEX_TYPE nextIntegerMutex, outputMutex;

COMMON_CPU_THREAD_FUNCTION_TYPE runWorkerWrapper(void *const workerIndex) {
	if (!workerIndex) return COMMON_CPU_THREAD_FUNCTION_RETURN;
	runWorker(workerIndex);
	return COMMON_CPU_THREAD_FUNCTION_RETURN;
}

#ifndef USE_CUSTOM_GET_NEXT_INTEGER
COMMON_NODISCARD bool getNextInteger(const void *const workerIndex, uint64_t *const integer) {
	if (INPUT_FILEPATH) {
		// fscanf seems to be non-thread-safe.
		COMMON_MUTEX_LOCK(nextIntegerMutex);
		// TODO: Support arbitrary characters following entires in input list, as fault tolerance?
		// TODO: Also support scanning unsigned 64-bit integers?
		int valuesRead = fscanf(inputFile, " %" SCNdFAST64 " \n", COMMON_REINTERPRET_CAST(int_fast64_t *, integer));
		COMMON_MUTEX_UNLOCK(nextIntegerMutex);
		return valuesRead == 1;
	}
	*integer = workerIndex ? COMMON_STATIC_CAST(uint64_t, *COMMON_STATIC_CAST(const int *, workerIndex)) + localStartInteger : *integer + COMMON_STATIC_CAST(uint64_t, localNumberOfWorkers);
	return *integer - localStartInteger < localNumberOfIntegers;
}
#endif

#ifndef USE_CUSTOM_OUTPUT_STRING
void outputString(const char *format, ...) {
	/* vfprintf is thread-safe when printing to stdout, but not when printing to a file.
	TODO: Is there any way to do this without mutexes? */
	if (OUTPUT_FILEPATH) COMMON_MUTEX_LOCK(outputMutex);
	va_list args;
	va_start(args, format);
	// if (OUTPUT_FILEPATH) COMMON_MUTEX_LOCK(outputMutex);
	vfprintf(OUTPUT_FILEPATH ? outputFile : stdout, format, args);
	// TODO: Does stdout need to be flushed?
	/* if (OUTPUT_FILEPATH) { */
		fflush(OUTPUT_FILEPATH ? outputFile : stdout);
		// COMMON_MUTEX_UNLOCK(outputMutex);
	/* } */
	va_end(args);
	if (OUTPUT_FILEPATH) COMMON_MUTEX_UNLOCK(outputMutex);
}
#endif

int main() {
	initializeGlobals();
	COMMON_MUTEX_CREATE(nextIntegerMutex);
	COMMON_MUTEX_CREATE(outputMutex);
	if (INPUT_FILEPATH) {
		inputFile = fopen(INPUT_FILEPATH, "r");
		if (!inputFile) RAISE_EXCEPTION_OR_QUIT("core/Backends/CPU Threads.c: int main(): inputFile = fopen(INPUT_FILEPATH, \"r\"): Failed to open %s.\n", INPUT_FILEPATH);
	}
	if (OUTPUT_FILEPATH) {
		outputFile = fopen(OUTPUT_FILEPATH, "a");
		if (!outputFile) RAISE_EXCEPTION_OR_QUIT("core/Backends/CPU Threads.c: int main(): outputFile = fopen(OUTPUT_FILEPATH, \"w\"): Failed to open %s.\n", OUTPUT_FILEPATH);
	}
	struct timespec startTime, endTime;
	if (TIME_PROGRAM) clock_gettime(CLOCK_MONOTONIC, &startTime);
	COMMON_CPU_THREAD_TYPE threads[GLOBAL_NUMBER_OF_WORKERS]; // TODO: Not ISO-C++-supported
	COMMON_CPU_THREAD_INDEX_TYPE threadIndices[GLOBAL_NUMBER_OF_WORKERS]; // TODO: Not ISO-C++-supported
	for (int i = 0; i < GLOBAL_NUMBER_OF_WORKERS; ++i) COMMON_CPU_THREAD_INDEX_ACCESS(threadIndices, i) = i;
	COMMON_CPU_THREAD_LAUNCH(threads, GLOBAL_NUMBER_OF_WORKERS, threadIndices, runWorkerWrapper);
	COMMON_CPU_THREAD_JOIN(threads, GLOBAL_NUMBER_OF_WORKERS);
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