/* ----------- C/C++ vs. CUDA support ----------- */
#ifndef CORE_INTERNAL_EXTENSIONS_SUPPORT_H_
#define CORE_INTERNAL_EXTENSIONS_SUPPORT_H_

#include "platform_support.h"

#if COMMON_CURRENT_EXTENSIONS & COMMON_EXTENSIONS_CUDA
	#include <cuda.h>
#endif

// Cross-extension cudaError_t
#ifdef COMMON_CUDAERROR_T
	#undef COMMON_CUDAERROR_T
#endif
#if COMMON_CURRENT_EXTENSIONS & COMMON_EXTENSIONS_CUDA
	// CUDA
	#define COMMON_CUDAERROR_T cudaError_t
#else
	// Non-CUDA
	#define COMMON_CUDAERROR_T int
#endif

// Cross-extension cudaErrorInvalidValue
#ifdef COMMON_CUDA_ERROR_INVALID_VALUE
	#undef COMMON_CUDA_ERROR_INVALID_VALUE
#endif
#if COMMON_CURRENT_EXTENSIONS & COMMON_EXTENSIONS_CUDA
	// CUDA
	#define COMMON_CUDA_ERROR_INVALID_VALUE cudaErrorInvalidValue
#else
	// Non-CUDA
	#define COMMON_CUDA_ERROR_INVALID_VALUE 1
#endif

// Cross-extension cudaStream_t
#ifdef COMMON_CUDASTREAM_T
	#undef COMMON_CUDASTREAM_T
#endif
#if COMMON_CURRENT_EXTENSIONS & COMMON_EXTENSIONS_CUDA
	// CUDA
	#define COMMON_CUDASTREAM_T cudaStream_t
#else
	// Non-CUDA
	#define COMMON_CUDASTREAM_T int
#endif

// Cross-extension cudaSuccess
#ifdef COMMON_CUDA_SUCCESS
	#undef COMMON_CUDA_SUCCESS
#endif
#if COMMON_CURRENT_EXTENSIONS & COMMON_EXTENSIONS_CUDA
	// CUDA
	#define COMMON_CUDA_SUCCESS cudaSuccess
#else
	// Non-CUDA
	#define COMMON_CUDA_SUCCESS 0
#endif

// Cross-extension __device__
#ifdef COMMON_DEVICE
	#undef COMMON_DEVICE
#endif
#if COMMON_CURRENT_EXTENSIONS & COMMON_EXTENSIONS_CUDA
	// CUDA
	#define COMMON_DEVICE __device__
#else
	// Non-CUDA
	#define COMMON_DEVICE
#endif

// Cross-extension __global__
#ifdef COMMON_GLOBAL
	#undef COMMON_GLOBAL
#endif
#if COMMON_CURRENT_EXTENSIONS & COMMON_EXTENSIONS_CUDA
	// CUDA
	#define COMMON_GLOBAL __global__
#else
	// Non-CUDA
	#define COMMON_GLOBAL
#endif

// Cross-extension __host__
#ifdef COMMON_HOST
	#undef COMMON_HOST
#endif
#if COMMON_CURRENT_EXTENSIONS & COMMON_EXTENSIONS_CUDA
	// CUDA
	#define COMMON_HOST __host__
#else
	// Non-CUDA
	#define COMMON_HOST
#endif

// Cross-extension __managed__
#ifdef COMMON_MANAGED
	#undef COMMON_MANAGED
#endif
#if COMMON_CURRENT_EXTENSIONS & COMMON_EXTENSIONS_CUDA
	// CUDA
	#define COMMON_MANAGED __managed__
#else
	// Non-CUDA
	#define COMMON_MANAGED
#endif

#ifndef CUDA_VERSION
	// C/C++

	enum {cudaErrorMemoryAllocation};

	/* WARNING: PROGRAMS CANNOT RELY ON PTHREADS HAVING BEEN INCLUDED IF THEY SUPPORT CUDA.
	   (Will be obsolete when generic multithreading support is implemented)*/
	
	// Hack in the meantime:
	#if defined(__has_include) && !__has_include(<pthread.h>)
		#warning "core/C-C++-CUDA Support.h": The current implementation of this library relies on pthreads (POSIX threads) \
		to implement certain non-CUDA fallback functions. \
		Since that is not installed on the current device, those fallback functions have been skipped. \
		\
		While support for generic multithreading backends will (hopefully) be added in the future, \
		for the time being this can be fixed by installing MSYS2 or WSL onto your system, \
		or by simply ignoring this if you do not need CUDA vs. non-CUDA interoperability.
	#else
		#include <pthread.h>

		// Replacements for some built-in CUDA functions
		static pthread_mutex_t __mutex;
		// Atomically adds the specified value to the value stored in the specified address. Returns the original value in the address.
		static inline unsigned long long atomicAdd(unsigned long long *address, int value) {
			unsigned long long __temp = *address;
			pthread_mutex_lock(&__mutex);
			*address += COMMON_STATIC_CAST(unsigned long long, value);
			pthread_mutex_unlock(&__mutex);
			return __temp;
		}

		// Atomically ORs the specified value to the value stored in the specified address. Returns the original value in the address.
		static inline unsigned long long atomicOr(uint32_t *address, uint32_t value) {
			uint32_t __temp = *address;
			pthread_mutex_lock(&__mutex);
			*address |= value;
			pthread_mutex_unlock(&__mutex);
			return __temp;
		}

		static pthread_t *threads;
		/* Should this and bruteforce.h be directly integrated by setting this to GLOBAL_NUMBER_OF_WORKERS?
		   (It currently needs to be set manually otherwise)*/
		static size_t __numberOfThreads;
		static inline COMMON_CUDAERROR_T cudaDeviceSynchronize(void) {
			for (size_t i = 0; i < __numberOfThreads; ++i) pthread_join(threads[i], NULL);
			return COMMON_CUDA_SUCCESS;
		}
	#endif

	static inline const char *cudaGetErrorString(COMMON_MAYBE_UNUSED_FOR_ARGUMENT_LIST(COMMON_CUDAERROR_T error)) {
		COMMON_MAYBE_UNUSED_FOR_FUNCTION_BODY(error);
		return "[CUDA error fetching not yet supported]";
	}

	static inline COMMON_CUDAERROR_T cudaGetLastError(void) {
		return COMMON_CUDA_SUCCESS;
	}

	static inline COMMON_CUDAERROR_T cudaGetSymbolAddress(void** devPtr, void* symbol) {
		if (!devPtr || !symbol) return COMMON_CUDA_ERROR_INVALID_VALUE;
		*devPtr = symbol;
		COMMON_STD memcpy(*devPtr, symbol, sizeof(void*));
		return COMMON_CUDA_SUCCESS;
	}

	static inline COMMON_CUDAERROR_T cudaMalloc(void **devPtr, size_t size) {
		if (!devPtr) return COMMON_CUDA_ERROR_INVALID_VALUE;
		*devPtr = COMMON_STD malloc(size);
		return *devPtr ? COMMON_CUDA_SUCCESS : cudaErrorMemoryAllocation;
	}

	static inline COMMON_CUDAERROR_T cudaFree(void *devPtr) {
		COMMON_STD free(devPtr);
		return COMMON_CUDA_SUCCESS;
	}

	// ENUM(cudaMemcpyKind) {cudaMemcpyHostToHost, cudaMemcpyHostToDevice, cudaMemcpyDeviceToHost, cudaMemcpyDeviceToDevice, cudaMemcpyDefault};
	enum cudaMemcpyKind {cudaMemcpyHostToHost, cudaMemcpyHostToDevice, cudaMemcpyDeviceToHost, cudaMemcpyDeviceToDevice, cudaMemcpyDefault};
	static inline COMMON_CUDAERROR_T cudaMemcpy(void *dst, const void *src, size_t count, COMMON_MAYBE_UNUSED_FOR_ARGUMENT_LIST(enum cudaMemcpyKind kind)) {
		COMMON_MAYBE_UNUSED_FOR_FUNCTION_BODY(kind);
		COMMON_STD memcpy(dst, src, count);
		return COMMON_CUDA_SUCCESS;
	}

	static inline COMMON_CUDAERROR_T cudaMemsetAsync(void* devPtr, int value, size_t count, COMMON_MAYBE_UNUSED_FOR_ARGUMENT_LIST(COMMON_CUDASTREAM_T stream)
	#ifdef __cplusplus
		// C++: supports default values for arguments
		= 0
	#endif
	) {
		COMMON_MAYBE_UNUSED_FOR_FUNCTION_BODY(stream);
		COMMON_STD memset(devPtr, value, count);
		return COMMON_CUDA_SUCCESS;
	}
#endif

// Exception raising, variant one: raise an exception if supported, quit if not.
#ifndef RAISE_EXCEPTION_OR_QUIT
	#ifndef __cplusplus
		/* C: has no exception handling.
		From Andrew (https://github.com/Gaider10/TreeCracker/blob/bedb5e995500fc86dcd5382ef16397e00a1da461/src/second.cu#L11)*/
		#define RAISE_EXCEPTION_OR_QUIT(...) { \
			fprintf(stderr, __VA_ARGS__); \
			exit(1); \
		}
	#elif !defined(CUDA_VERSION)
		// C++: has proper exception handling
		#define RAISE_EXCEPTION_OR_QUIT(...) { \
			fprintf(stderr, __VA_ARGS__); \
			exit(1); \
		}
			// Should be replaced with throw std::invalid_argument one day, but that doesn't support variadic arguments...
			// size_t __strLength = std::string __str = std::to_string(__VA_ARGS__);
			// throw std::invalid_argument(__str.c_str());
	#else
		/* CUDA: device code does not support exception throwing or printing to stderr.
		TODO: Check if device code even supports exit() or abort() (otherwise will have to use return, which is suboptimal)*/
		#define RAISE_EXCEPTION_OR_QUIT(...) exit(1);
	#endif
#endif

// Exception raising, variant two: raise an exception if supported, return a provided default value if not.
#ifndef RAISE_EXCEPTION_OR_RETURN_DEFAULT_VALUE
	#if !defined(__cplusplus) || defined(CUDA_VERSION)
		// C: has no exception handling. CUDA: device code does not support exception throwing.
		#define RAISE_EXCEPTION_OR_RETURN_DEFAULT_VALUE(DEFAULT_VALUE, ...) return DEFAULT_VALUE
	#else
		// C++: has proper exception handling
		#define RAISE_EXCEPTION_OR_RETURN_DEFAULT_VALUE(DEFAULT_VALUE, ...) RAISE_EXCEPTION_OR_QUIT(__VA_ARGS__)
	#endif
#endif

// Device return statements
#ifndef DEVICE_RETURN
	#ifndef CUDA_VERSION
		// C/C++: use pthreads to emulate multithreaded "device" code (really still just host code), which have a return type of void*
		#define DEVICE_RETURN return NULL
	#else
		// CUDA: use __global__ functions
		// TODO: Must __global__ functions necessarily return void? If not, need to add argument
		#define DEVICE_RETURN return
	#endif
#endif

/* CUDA try mechanism.
   From Andrew (https://github.com/Gaider10/TreeCracker/blob/bedb5e995500fc86dcd5382ef16397e00a1da461/src/second.cu#L16)*/
#ifndef TRY_CUDA
	static inline void __tryCuda(COMMON_CUDAERROR_T error, const char *file, uint64_t line) {
		if (error == COMMON_CUDA_SUCCESS) return;
		#ifdef CUDA_VERSION
			/* Currently unused in CUDA's RAISE_EXCEPTION_OR_QUIT implementation */
			// const char *__VARIANT = "CUDA";
		#elif defined(__cplusplus)
			const char *__VARIANT = "C++";
		#else
			const char *__VARIANT = "C";
		#endif
		RAISE_EXCEPTION_OR_QUIT("%s error at %s:%" PRIu64 ": %s\n", __VARIANT, file, line, cudaGetErrorString(error));
	}

	#define TRY_CUDA(expression) __tryCuda(expression, __FILE__, __LINE__)
#endif

#endif // CORE_INTERNAL_EXTENSIONS_SUPPORT_H_
