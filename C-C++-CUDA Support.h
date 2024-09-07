/* Attempts to provide support between certain C, C++, and CUDA constructs.
   Some constructs are emulated, but others that have no corresponding language equivalent are simply hidden. */

#ifndef _C_CPP_CUDA_SUPPORT_H
#define _C_CPP_CUDA_SUPPORT_H

/* ----------- C vs. C++ support ----------- */

// Noation for importing standard library files
#ifndef INCLUDE_STANDARD
	#ifndef __cplusplus
		// C: Use <F.h> notation
		#define INCLUDE_STANDARD(F) <F.h>
	#else
		// C++: Use <cF> notation
		#define INCLUDE_STANDARD(F) <c##F>
	#endif
#endif

// Import libraries this file needs
#include INCLUDE_STANDARD(inttypes) // uint32_t, uint64_t
#include INCLUDE_STANDARD(stdio)    // fprintf, stderr
#include INCLUDE_STANDARD(stdlib)   // malloc
#include INCLUDE_STANDARD(string)   // memcpy

// Substitute/ignore C++ keywords that don't exist in C
#if !defined(__cplusplus) && !defined(constexpr)
	// C
	#define constexpr static inline
	#define noexcept
#endif

// Cross-platform maybe_unused attribute
#ifndef MAYBE_UNUSED
	/* TODO: [[maybe_unused]] seems to require placement in argument list, while mtvec's workaround seems to require placement outside of argument list.
	Need to figure out how to handle that difference first before [[maybe_unused]] can be "properly" implemented.*/
	/* #if !defined(__cplusplus) && __STDC_VERSION__ < 202300L
		// C17-: workaround from mtvec on Stack Overflow (https://stackoverflow.com/a/3599170)
		#define MAYBE_UNUSED(parameter) (void)(parameter)
	#else
		//C2x+/C++
		#define MAYBE_UNUSED(parameter) [[maybe_unused]] parameter
	#endif */
	#define MAYBE_UNUSED(parameter) (void)(parameter)
#endif

// Cross-platform nodiscard attribute
#ifndef NO_DISCARD
	#if !defined(__cplusplus) && __STDC_VERSION__ < 202300L
		// C17-
		#define NO_DISCARD
	#else
		// C2x+/C++
		#define NO_DISCARD [[nodiscard]]
	#endif
#endif

// Cross-platform casting
#ifndef REINTERPRET_CAST
	#ifndef __cplusplus
		// C: only normal casting exists
		#define REINTERPRET_CAST(A, B) ((A)(B))
	#else
		// C++: specialized casting exists
		#define REINTERPRET_CAST(A, B) (reinterpret_cast<A>(B))
	#endif
#endif

#ifndef STATIC_CAST
	#ifndef __cplusplus
		// C: only normal casting exists
		#define STATIC_CAST(A, B) ((A)(B))
	#else
		// C++: specialized casting exists
		#define STATIC_CAST(A, B) (static_cast<A>(B))
	#endif
#endif

// Cross-platform standard library references (for common C/C++ functions and variables, at least)
#ifndef STD
	#ifndef __cplusplus
		// C: doesn't exist
		#define STD
	#else
		// C++
		#define STD std::
	#endif
#endif

/* Cross-platform enums, structs, and unions
   (Normally C requires the enum/struct/union keyword when invoking them, while C++ doesn't)
   From Cubitect (https://github.com/Cubitect/cubiomes/blob/0af31b4e7eeb14a58c2bd9a4c4c68b97b4a7d6e8/rng.h#L27)*/
#ifndef ENUM
	#define ENUM(E) typedef enum E E; enum E
#endif
#ifndef STRUCT
	#define STRUCT(S) typedef struct S S; struct S
#endif
#ifndef UNION
	#define UNION(U) typedef union U U; union U
#endif

// ----------------------------------------------

/* ----------- C/C++ vs. CUDA support ----------- */

// TODO: #include <cuda.h> directly without causing errors if C/C++.
// For now <cuda.h> has to be #include-d in the host program before "common.h".

#ifndef CUDA_VERSION
	// C/C++

	// WARNING: PROGRAMS CANNOT RELY ON PTHREADS HAVING BEEN INCLUDED IF THEY SUPPORT CUDA.
	#include <pthread.h>

	// CUDA keywords that don't exist in C/C++
	#ifndef __device__
		#define __device__
	#endif
	#ifndef __host__
		#define __host__
	#endif
	#ifndef __global__
		#define __global__
	#endif
	#ifndef __managed__
		#define __managed__
	#endif
	#ifndef cudaError_t
		#define cudaError_t int
	#endif
	#ifndef cudaStream_t
		#define cudaStream_t int
	#endif

	enum {cudaSuccess, cudaErrorInvalidValue, cudaErrorMemoryAllocation};

	// Replacements for some built-in CUDA functions
	pthread_mutex_t __mutex;
	// Atomically adds the specified value to the value stored in the specified address. Returns the original value in the address.
	unsigned long long atomicAdd(unsigned long long *address, int value) {
		unsigned long long __temp = *address;
		pthread_mutex_lock(&__mutex);
		*address += STATIC_CAST(unsigned long long, value);
		pthread_mutex_unlock(&__mutex);
		return __temp;
	}

	// Atomically ORs the specified value to the value stored in the specified address. Returns the original value in the address.
	unsigned long long atomicOr(uint32_t *address, uint32_t value) {
		uint32_t __temp = *address;
		pthread_mutex_lock(&__mutex);
		*address |= value;
		pthread_mutex_unlock(&__mutex);
		return __temp;
	}

	pthread_t *threads;
	size_t __numberOfThreads;
	cudaError_t cudaDeviceSynchronize() {
		for (size_t i = 0; i < __numberOfThreads; ++i) pthread_join(threads[i], NULL);
		return cudaSuccess;
	}

	const char *cudaGetErrorString(cudaError_t error) {
		MAYBE_UNUSED(error);
		return "[CUDA error fetching not yet supported]";
	}

	cudaError_t cudaGetLastError() {
		return cudaSuccess;
	}

	cudaError_t cudaGetSymbolAddress(void** devPtr, void* symbol) {
		if (!devPtr || !symbol) return cudaErrorInvalidValue;
		*devPtr = symbol;
		STD memcpy(*devPtr, symbol, sizeof(void*));
		return cudaSuccess;
	}

	cudaError_t cudaMalloc(void **devPtr, size_t size) {
		if (!devPtr) return cudaErrorInvalidValue;
		*devPtr = STD malloc(size);
		return *devPtr ? cudaSuccess : cudaErrorMemoryAllocation;
	}

	cudaError_t cudaFree(void *devPtr) {
		STD free(devPtr);
		return cudaSuccess;
	}

	ENUM(cudaMemcpyKind) {cudaMemcpyHostToHost, cudaMemcpyHostToDevice, cudaMemcpyDeviceToHost, cudaMemcpyDeviceToDevice, cudaMemcpyDefault};
	cudaError_t cudaMemcpy(void *dst, const void *src, size_t count, cudaMemcpyKind kind) {
		MAYBE_UNUSED(kind);
		STD memcpy(dst, src, count);
		return cudaSuccess;
	}

	cudaError_t cudaMemsetAsync(void* devPtr, int value, size_t count, cudaStream_t stream
	#ifdef __cplusplus
		// C++: supports default values for arguments
		= 0
	#endif
	) {
		MAYBE_UNUSED(stream);
		STD memset(devPtr, value, count);
		return cudaSuccess;
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
		#define RAISE_EXCEPTION_OR_QUIT(...) throw STD invalid_argument(__VA_ARGS__)
	#else
		/* CUDA: device code does not support exception throwing or printing to stderr.
		TODO: Check if device code even supports exit() or abort() (otherwise will have to use return, which is suboptimal)*/
		#define RAISE_EXCEPTION_OR_QUIT(...) exit(1)
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
	void __tryCuda(cudaError_t error, const char *file, uint64_t line) {
		if (error == cudaSuccess) return;
		RAISE_EXCEPTION_OR_QUIT("%s error at %s:%" PRIu64 ": %s\n",
		#ifdef CUDA_VERSION
			"CUDA"
		#elif defined(__cplusplus)
			"C++"
		#else
			"C"
		#endif
		, file, line, cudaGetErrorString(error));
	}

	#define TRY_CUDA(expression) __tryCuda(expression, __FILE__, __LINE__)
#endif

// ----------------------------------------------------------

#endif