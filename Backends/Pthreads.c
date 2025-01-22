#include "../bruteforce.h"
#if defined(__has_include) && !__has_include(<pthread.h>)
	#error "'core/Backends/Pthreads.c: Pthreads (POSIX threads) are not installed on the current device. \
		While support for generic multithreading backends will (hopefully) be added in the future, \
		for the time being this can be fixed by installing MSYS2 or WSL onto your system."
#else
	#include <pthread.h>

	FILE *inputFile = NULL, *outputFile = NULL;
	pthread_mutex_t nextIntegerMutex, outputMutex;

	#ifndef USE_CUSTOM_GET_NEXT_INTEGER
	NO_DISCARD bool getNextInteger(const void *workerIndex, uint64_t *integer) {
		if (INPUT_FILEPATH) {
			// fscanf seems to be non-thread-safe.
			pthread_mutex_lock(&nextIntegerMutex);
			// TODO: Support arbitrary characters following entires in input list, as fault tolerance?
			// TODO: Also support scanning unsigned 64-bit integers?
			int valuesRead = fscanf(inputFile, " %" SCNdFAST64 " \n", REINTERPRET_CAST(int_fast64_t *, integer));
			pthread_mutex_unlock(&nextIntegerMutex);
			return valuesRead == 1;
		}
		*integer = workerIndex ? STATIC_CAST(uint64_t, *STATIC_CAST(const int *, workerIndex)) + localStartInteger : *integer + STATIC_CAST(uint64_t, localNumberOfWorkers);
		return *integer - localStartInteger < localNumberOfIntegers;
	}
	#endif

	#ifndef USE_CUSTOM_OUTPUT_STRING
	void outputString(const char *format, ...) {
		/* vfprintf is thread-safe when printing to stdout, but not when printing to a file.
		TODO: Is there any way to do this without mutexes? */
		if (OUTPUT_FILEPATH) pthread_mutex_lock(&outputMutex);
		va_list args;
		va_start(args, format);
		// if (OUTPUT_FILEPATH) pthread_mutex_lock(&outputMutex);
		vfprintf(OUTPUT_FILEPATH ? outputFile : stdout, format, args);
		// TODO: Does stdout need to be flushed?
		/* if (OUTPUT_FILEPATH) { */
			fflush(OUTPUT_FILEPATH ? outputFile : stdout);
			// pthread_mutex_unlock(&outputMutex);
		/* } */
		va_end(args);
		if (OUTPUT_FILEPATH) pthread_mutex_unlock(&outputMutex);
	}
	#endif

	int main() {
		initializeGlobals();
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
		pthread_t threads[GLOBAL_NUMBER_OF_WORKERS]; // TODO: Not ISO-C++-supported
		int data[GLOBAL_NUMBER_OF_WORKERS]; // TODO: Not ISO-C++-supported
		for (int i = 0; i < GLOBAL_NUMBER_OF_WORKERS; ++i) {
			data[i] = i;
			pthread_create(&threads[i], NULL, runWorker, &data[i]);
		}
		for (int i = 0; i < GLOBAL_NUMBER_OF_WORKERS; ++i) pthread_join(threads[i], NULL);
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
#endif