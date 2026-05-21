#ifdef __has_include
	#if !__has_include(<windows.h>)
		#error "'core/Backends/Windows Threads.c: Windows threads are not installed on the current device.
	#endif
#endif

#define COMMON_CURRENT_PLATFORM COMMON_PLATFORM_WINDOWS
#include "../bruteforce.h"
#include <windows.h>

FILE *inputFile = NULL, *outputFile = NULL;
COMMON_MUTEX_TYPE nextIntegerMutex, outputMutex;

DWORD WINAPI runWorkerWrapper(LPVOID workerIndex) {
	// if (!workerIndex) return 1;
	runWorker(workerIndex);
	return 0;
}

#ifndef USE_CUSTOM_GET_NEXT_INTEGER
COMMON_NODISCARD bool getNextInteger(const void *const workerIndex, uint64_t *const integer) {
	if (INPUT_FILEPATH) {
		// fscanf seems to be non-thread-safe.
		COMMON_MUTEX_LOCK(nextIntegerMutex);
		// TODO: Support arbitrary characters following entires in input list, as fault tolerance?
		// TODO: Also support scanning unsigned 64-bit integers?
		int valuesRead = fscanf(inputFile, " %" SCNdFAST64 " \n", COMMON_REINTERPRET_CAST(int_fast64_t *const, integer));
		COMMON_MUTEX_UNLOCK(nextIntegerMutex);
		return valuesRead == 1;
	}
	*integer = workerIndex ? COMMON_STATIC_CAST(uint64_t, *COMMON_STATIC_CAST(const int *const, workerIndex)) + localStartInteger : *integer + COMMON_STATIC_CAST(uint64_t, localNumberOfWorkers);
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
		if (!inputFile) RAISE_EXCEPTION_OR_QUIT("core/Backends/Pthreads.c: int main(): inputFile = fopen(INPUT_FILEPATH, \"r\"): Failed to open %s.\n", INPUT_FILEPATH);
	}
	if (OUTPUT_FILEPATH) {
		outputFile = fopen(OUTPUT_FILEPATH, "w");
		if (!outputFile) RAISE_EXCEPTION_OR_QUIT("core/Backends/Pthreads.c: int main(): outputFile = fopen(OUTPUT_FILEPATH, \"w\"): Failed to open %s.\n", OUTPUT_FILEPATH);
	}
	struct timespec startTime, endTime;
	if (TIME_PROGRAM) clock_gettime(CLOCK_MONOTONIC, &startTime);
	HANDLE threads[GLOBAL_NUMBER_OF_WORKERS]; // TODO: Not ISO-C++-supported
	int *data[GLOBAL_NUMBER_OF_WORKERS]; // TODO: Not ISO-C++-supported
	for (int i = 0; i < GLOBAL_NUMBER_OF_WORKERS; ++i) {
		*data[i] = i;
		CreateThread(NULL, 0, runWorkerWrapper, &data[i], 0, threads[i]);
	}
	WaitForMultipleObjects(GLOBAL_NUMBER_OF_WORKERS, threads, true, INFINITE);
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