/* ----------- Cross-Language Support ----------- */

#ifndef CORE_INTERNAL_LANGUAGE_SUPPORT_H
#define CORE_INTERNAL_LANGUAGE_SUPPORT_H

#include "definitions.h"

// Import language-specific libraries this file will need
/* TODO: Maybe try falling back on <*.h> if <c*> is unfindable?*/
#if COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_C
	#include <inttypes.h> // uint32_t, uint64_t
	#include <stdbool.h>
	#include <stdio.h>    // fprintf, stderr
	#include <stdlib.h>   // malloc
	#include <string.h>   // memcpy
#elif COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP
	#include <cinttypes> // uint32_t, uint64_t
	#include <cstdio>    // fprintf, stderr
	#include <cstdlib>   // malloc
	#include <cstring>   // memcpy
	#include <stdexcept>
#else 
	#error "Invalid current language."
#endif

// Substitute/ignore keywords that don't exist in all languages
// Cross-version const_cast
#ifdef COMMON_CONST_CAST
	#undef COMMON_CONST_CAST
#endif
// TODO: Concretely verify when this was added to C++
#if COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_CPP_98
	// C++98
	#define COMMON_CONST_CAST(TYPE, EXPRESSION) (const_cast<TYPE>(EXPRESSION))
#else
	// C/C++ pre-98: only normal casting exists
	#define COMMON_CONST_CAST(TYPE, EXPRESSION) ((TYPE)(EXPRESSION))
#endif

// Cross-version constexpr for functions
#ifdef COMMON_CONSTEXPR_FUNCTION
	#undef COMMON_CONSTEXPR_FUNCTION
#endif
#if (COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_C && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_C_23) || (COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_CPP_11)
	// C++11/C23
	#define COMMON_CONSTEXPR_FUNCTION constexpr
#else
	// C++98/C20
	#define COMMON_CONSTEXPR_FUNCTION static inline
#endif

// Cross-version constexpr for variables
#ifdef COMMON_CONSTEXPR_VARIABLE
	#undef COMMON_CONSTEXPR_VARIABLE
#endif
#if (COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_C && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_C_23) || (COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_CPP_11)
	// C++11/C23
	#define COMMON_CONSTEXPR_VARIABLE constexpr
#else
	// C++98/C20
	// TODO: const or static const?
	#define COMMON_CONSTEXPR_VARIABLE static const
#endif

// Cross-version [[deprecated]]
#ifdef COMMON_DEPRECATED
	#undef COMMON_DEPRECATED
#endif
#if (COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_C && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_C_23) || (COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_CPP_14)
	// C++14/C23
	#define COMMON_DEPRECATED(MESSAGE) [[deprecated(MESSAGE)]]
#else
	// C++11/C20
	#define COMMON_DEPRECATED
#endif

// Cross-language [[maybe_unused]]
// For versions not supporting [[maybe_unused]], a workaround by mtvec https://stackoverflow.com/a/3599170)
// exists to still silence compilers. However it must be placed solely in the function body, while
// [[maybe_unused]] can exist in either the function body or the argument list. Therefore we define two
// maybe_unused types.
#ifdef COMMON_MAYBE_UNUSED_FOR_ARGUMENT_LIST
	#undef COMMON_MAYBE_UNUSED_FOR_ARGUMENT_LIST
#endif
#ifdef COMMON_MAYBE_UNUSED_FOR_FUNCTION_BODY
	#undef COMMON_MAYBE_UNUSED_FOR_FUNCTION_BODY
#endif
#if (COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_C && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_C_23) || (COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_CPP_17)
	// C23/C++17
	#define COMMON_MAYBE_UNUSED_FOR_ARGUMENT_LIST(VARIABLE) [[maybe_unused]] VARIABLE
	#define COMMON_MAYBE_UNUSED_FOR_FUNCTION_BODY(VARIABLE)
#else
	// C20/C++11: workaround
	#define COMMON_MAYBE_UNUSED_FOR_ARGUMENT_LIST(VARIABLE) VARIABLE
	#define COMMON_MAYBE_UNUSED_FOR_FUNCTION_BODY(VARIABLE) (void)(VARIABLE)
#endif

#ifdef COMMON_NOEXCEPT
	#undef COMMON_NOEXCEPT
#endif
#if COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_CPP_11
	// C++11
	#define COMMON_NOEXCEPT(CONDITIONS) noexcept(CONDITIONS)
#else
	// C/C++98
	#define COMMON_NOEXCEPT(CONDITIONS)
#endif

// Cross-language [[nodiscard]]
#ifdef COMMON_NODISCARD
	#undef COMMON_NODISCARD
#endif
#if (COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_C && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_C_23) || (COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_CPP_17)
	// C23/C++17
	#define COMMON_NODISCARD [[nodiscard]]
#else
	// C17/C++11
	#define COMMON_NODISCARD
#endif

// Cross-version reinterpret_cast
#ifdef COMMON_REINTERPRET_CAST
	#undef COMMON_REINTERPRET_CAST
#endif
// TODO: Concretely verify when this was added to C++
#if COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_CPP_98
	// C++98
	#define COMMON_REINTERPRET_CAST(TYPE, EXPRESSION) (reinterpret_cast<TYPE>(EXPRESSION))
#else
	// C/C++ pre-98: only normal casting exists
	#define COMMON_REINTERPRET_CAST(TYPE, EXPRESSION) ((TYPE)(EXPRESSION))
#endif

// Cross-version static_cast
#ifdef COMMON_STATIC_CAST
	#undef COMMON_STATIC_CAST
#endif
// TODO: Concretely verify when this was added to C++
#if COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_CPP_98
	// C++98
	#define COMMON_STATIC_CAST(TYPE, EXPRESSION) (static_cast<TYPE>(EXPRESSION))
#else
	// C/C++ pre-98: only normal casting exists
	#define COMMON_STATIC_CAST(TYPE, EXPRESSION) ((TYPE)(EXPRESSION))
#endif

// Cross-language standard library references (for duplicate functions/variables in both languages, at least)
#ifdef COMMON_STD
	#undef COMMON_STD
#endif
#if COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP
	// C++
	#define COMMON_STD std::
#else
	// C: doesn't exist
	#define COMMON_STD
#endif

/* Cross-platform enums, structs, and unions
   (Normally C requires the enum/struct/union keyword when invoking them, while C++ doesn't)
   From Cubitect (https://github.com/Cubitect/cubiomes/blob/0af31b4e7eeb14a58c2bd9a4c4c68b97b4a7d6e8/rng.h#L27)*/
// TODO: Not currently ISO-C compatible
// #ifdef COMMON_ENUM
// 	#undef COMMON_ENUM
// #endif
// #define COMMON_ENUM(E) typedef enum E E; enum E

#ifdef COMMON_STRUCT
	#undef COMMON_STRUCT
#endif
#define COMMON_STRUCT(S) typedef struct S S; struct S

#ifdef COMMON_UNION
	#undef COMMON_UNION
#endif
#define COMMON_UNION(U) typedef union U U; union U

#endif