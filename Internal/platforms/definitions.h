#ifndef CORE_INTERNAL_PLATFORMS_DEFINITIONS_H_
#define CORE_INTERNAL_PLATFORMS_DEFINITIONS_H_

#include "../languages/definitions.h"


// Enumeration of supported execution platforms, for threads, mutexes, etc.
// Unknown
#ifdef COMMON_PLATFORM_UNKNOWN
	#undef COMMON_PLATFORM_UNKNOWN
#endif
#define COMMON_PLATFORM_UNKNOWN -1
// C Standard Library
#ifdef COMMON_PLATFORM_C_STDLIB
	#undef COMMON_PLATFORM_C_STDLIB
#endif
#define COMMON_PLATFORM_C_STDLIB 0
// C++ Standard Library
#ifdef COMMON_PLATFORM_CPP_STDLIB
	#undef COMMON_PLATFORM_CPP_STDLIB
#endif
#define COMMON_PLATFORM_CPP_STDLIB 1
// POSIX
#ifdef COMMON_PLATFORM_POSIX
	#undef COMMON_PLATFORM_POSIX
#endif
#define COMMON_PLATFORM_POSIX 2
// Windows
#ifdef COMMON_PLATFORM_WINDOWS
	#undef COMMON_PLATFORM_WINDOWS
#endif
#define COMMON_PLATFORM_WINDOWS 3


// Get current platform
#ifndef COMMON_CURRENT_PLATFORM
	#ifndef __has_include
		#warning "Your compiler does not support __has_include, meaning this module has no way to determine your current execution platform (POSIX, Windows, etc.). Certain features will therefore become unavailable."
		/* Normally we'd make the assumption C11+ probably has the standard library installed.
		   However it seems multiple environments--e.g. MSYS2--still don't support <threads.h>.*/
		// #if COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_C && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_C_11 && !defined(__STDC_NO_THREADS__)
		// 	#define COMMON_CURRENT_PLATFORM COMMON_PLATFORM_C_STDLIB

		// Make the assumption C++ probably has the standard library installed
		// #elif COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP
		#if COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP
			#define COMMON_CURRENT_PLATFORM COMMON_PLATFORM_CPP_STDLIB
		// Otherwise treat as unknown
		#else
			#define COMMON_CURRENT_PLATFORM COMMON_PLATFORM_UNKNOWN
		#endif
	#elif COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_C && __has_include(<threads.h>) && !defined(__STDC_NO_THREADS__)
		// C Standard Library
		#define COMMON_CURRENT_PLATFORM COMMON_PLATFORM_C_STDLIB
	#elif COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP && __has_include(<mutex>) && __has_include(<thread>)
		// C++ Standard Library
		#define COMMON_CURRENT_PLATFORM COMMON_PLATFORM_CPP_STDLIB
		// #define COMMON_CURRENT_PLATFORM_VERSION __cplusplus
	#elif __has_include(<pthread.h>)
		// POSIX
		#define COMMON_CURRENT_PLATFORM COMMON_PLATFORM_POSIX
	#elif __has_include(<windows.h>)
		// Windows
		#define COMMON_CURRENT_PLATFORM COMMON_PLATFORM_WINDOWS
	#else
		// Unknown
		#warning "Execution platform could not be determined."
		#define COMMON_CURRENT_PLATFORM COMMON_PLATFORM_UNKNOWN
	#endif
#endif

#endif // CORE_INTERNAL_PLATFORMS_DEFINITIONS_H_
