/* ----------- C/C++ vs. CUDA support ----------- */
#ifndef CORE_INTERNAL_EXTENSIONS_SUPPORT_H_
#define CORE_INTERNAL_EXTENSIONS_SUPPORT_H_

#include "../languages/definitions.h"
#include "../platforms/definitions.h"


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

#endif // CORE_INTERNAL_EXTENSIONS_SUPPORT_H_
