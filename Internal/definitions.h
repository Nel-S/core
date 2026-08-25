#ifndef CORE_INTERNAL_DEFINITIONS_H_
#define CORE_INTERNAL_DEFINITIONS_H_

// Enumeration of supported languages
// C
#ifdef COMMON_LANGUAGE_C
	#undef COMMON_LANGUAGE_C
#endif
#define COMMON_LANGUAGE_C 0
// C++
#ifdef COMMON_LANGUAGE_CPP
	#undef COMMON_LANGUAGE_CPP
#endif
#define COMMON_LANGUAGE_CPP 1

// Get current language if not already specified
#ifndef COMMON_CURRENT_LANGUAGE
	#ifndef __cplusplus
		// C
		#define COMMON_CURRENT_LANGUAGE COMMON_LANGUAGE_C
	#else
		// C++
		#define COMMON_CURRENT_LANGUAGE COMMON_LANGUAGE_CPP
	#endif
#endif

// Enumeration of supported language versions
// (From https://stackoverflow.com/a/78581958)
// C94
#ifdef COMMON_LANGUAGE_VERSION_C_94
	#undef COMMON_LANGUAGE_VERSION_C_94
#endif
#define COMMON_LANGUAGE_VERSION_C_94 199409L
// C99
#ifdef COMMON_LANGUAGE_VERSION_C_99
	#undef COMMON_LANGUAGE_VERSION_C_99
#endif
#define COMMON_LANGUAGE_VERSION_C_99 199901L
// C11
#ifdef COMMON_LANGUAGE_VERSION_C_11
	#undef COMMON_LANGUAGE_VERSION_C_11
#endif
#define COMMON_LANGUAGE_VERSION_C_11 201112L
// C17
#ifdef COMMON_LANGUAGE_VERSION_C_17
	#undef COMMON_LANGUAGE_VERSION_C_17
#endif
#define COMMON_LANGUAGE_VERSION_C_17 201710L
// C23
#ifdef COMMON_LANGUAGE_VERSION_C_23
	#undef COMMON_LANGUAGE_VERSION_C_23
#endif
#define COMMON_LANGUAGE_VERSION_C_23 202311L
// (From https://stackoverflow.com/a/7132549)
// C++pre-98
#ifdef COMMON_LANGUAGE_VERSION_CPP_PRE_98
	#undef COMMON_LANGUAGE_VERSION_CPP_PRE_98
#endif
#define COMMON_LANGUAGE_VERSION_CPP_PRE_98 1
// C++98
#ifdef COMMON_LANGUAGE_VERSION_CPP_98
	#undef COMMON_LANGUAGE_VERSION_CPP_98
#endif
#define COMMON_LANGUAGE_VERSION_CPP_98 199711L
// C++11
#ifdef COMMON_LANGUAGE_VERSION_CPP_11
	#undef COMMON_LANGUAGE_VERSION_CPP_11
#endif
#define COMMON_LANGUAGE_VERSION_CPP_11 201103L
// C++14
#ifdef COMMON_LANGUAGE_VERSION_CPP_14
	#undef COMMON_LANGUAGE_VERSION_CPP_14
#endif
#define COMMON_LANGUAGE_VERSION_CPP_14 201402L
// C++17
#ifdef COMMON_LANGUAGE_VERSION_CPP_17
	#undef COMMON_LANGUAGE_VERSION_CPP_17
#endif
#define COMMON_LANGUAGE_VERSION_CPP_17 201703L
// C++14
#ifdef COMMON_LANGUAGE_VERSION_CPP_14
	#undef COMMON_LANGUAGE_VERSION_CPP_14
#endif
#define COMMON_LANGUAGE_VERSION_CPP_14 201402L
// C++17
#ifdef COMMON_LANGUAGE_VERSION_CPP_17
	#undef COMMON_LANGUAGE_VERSION_CPP_17
#endif
#define COMMON_LANGUAGE_VERSION_CPP_17 201703L
// C++20
#ifdef COMMON_LANGUAGE_VERSION_CPP_20
	#undef COMMON_LANGUAGE_VERSION_CPP_20
#endif
#define COMMON_LANGUAGE_VERSION_CPP_20 202002L
// C++23
#ifdef COMMON_LANGUAGE_VERSION_CPP_23
	#undef COMMON_LANGUAGE_VERSION_CPP_23
#endif
#define COMMON_LANGUAGE_VERSION_CPP_23 202302L

// Get current language version if not already specified
#ifndef COMMON_CURRENT_LANGUAGE_VERSION
	#if COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_C
		// C
		#define COMMON_CURRENT_LANGUAGE_VERSION __STDC_VERSION__
	#elif COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP
		// C++
		#ifdef _MSVC_LANG
			// Microsoft Visual Compiler version (per https://stackoverflow.com/a/7132549)
			#define COMMON_CURRENT_LANGUAGE_VERSION _MSVC_LANG
		#else
			// Standard C++ version
			#define COMMON_CURRENT_LANGUAGE_VERSION __cplusplus
		#endif
	#else
		#error "Invalid language specified."
	#endif
#endif

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

// Enumeration of supported extensions
// None
#ifdef COMMON_EXTENSIONS_NONE
	#undef COMMON_EXTENSIONS_NONE
#endif
#define COMMON_EXTENSIONS_NONE 0 // 0b0
// CUDA
#ifdef COMMON_EXTENSIONS_CUDA
	#undef COMMON_EXTENSIONS_CUDA
#endif
#define COMMON_EXTENSIONS_CUDA 1 // 0b1
// #warning support
#ifdef COMMON_EXTENSIONS_WARNING_SUPPORT
	#undef COMMON_EXTENSIONS_WARNING_SUPPORT
#endif
#define COMMON_EXTENSIONS_WARNING_SUPPORT 2 // 0b10


// Get current extension, if not already specified
#ifndef COMMON_CURRENT_EXTENSIONS
	#ifndef __has_include
		#warning "..."
		#define COMMON_CURRENT_EXTENSIONS COMMON_EXTENSIONS_NONE
	#elif __has_include(<cuda.h>) && defined(__CUDACC__)
		// CUDA
		#define COMMON_CURRENT_EXTENSIONS COMMON_EXTENSIONS_CUDA
	#else
		// None
		#define COMMON_CURRENT_EXTENSIONS COMMON_EXTENSIONS_NONE
	#endif
#endif

#endif // CORE_INTERNAL_DEFINITIONS_H_
