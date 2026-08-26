/* ----------- Cross-Language Support ----------- */

#ifndef CORE_INTERNAL_LANGUAGES_SUPPORT_H_
#define CORE_INTERNAL_LANGUAGES_SUPPORT_H_

#include "definitions.h"

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
#if COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_CPP_11
	// C++11
	#define COMMON_CONSTEXPR_FUNCTION constexpr
#else
	// C/C++98
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

// Cross-version [[maybe_unused]]
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

// Cross-version namespace
#ifdef COMMON_NAMESPACE
	#undef COMMON_NAMESPACE
#endif
#if COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_CPP_98
	// C++98
	#define COMMON_NAMESPACE(NAME, EXPRESSIONS) namespace NAME { \
		EXPRESSIONS \
	};
#else
	// C/C++ pre-98
	#define COMMON_NAMESPACE(NAME, EXPRESSIONS) EXPRESSIONS
#endif

// Cross-version namespace identifiers (e.g. std::)
#ifdef COMMON_NAMESPACE_IDENTIFIER
	#undef COMMON_NAMESPACE_IDENTIFIER
#endif
#if COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_CPP && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_CPP_98
	// C++98
	#define COMMON_NAMESPACE_IDENTIFIER(NAMESPACE) NAMESPACE::
#else
	// C/C++ pre-98
	#define COMMON_NAMESPACE_IDENTIFIER(NAMESPACE)
#endif

// Cross-version [[noexcept]]
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

// Cross-version [[nodiscard]]
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

// Cross-version restrict
#ifdef COMMON_RESTRICT
	#undef COMMON_RESTRICT
#endif
#if COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_C && COMMON_CURRENT_LANGUAGE_VERSION >= COMMON_LANGUAGE_VERSION_C_99
	// C99
	#define COMMON_RESTRICT restrict
#elif defined(__restrict)
	#define COMMON_RESTRICT __restrict
#elif defined(__restrict__)
	#define COMMON_RESTRICT __restrict__
#else
	// C89/C++
	#define COMMON_RESTRICT
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

/* Cross-platform enums, structs, and unions
   (Normally C requires the enum/struct/union keyword when invoking them, while C++ doesn't)
   From Cubitect (https://github.com/Cubitect/cubiomes/blob/0af31b4e7eeb14a58c2bd9a4c4c68b97b4a7d6e8/rng.h#L27)*/
// TODO: Not currently ISO-C compatible
// #ifdef COMMON_ENUM
// 	#undef COMMON_ENUM
// #endif
// #define COMMON_ENUM(E) typedef enum E E; enum E
// #ifdef COMMON_ENUM_TYPE
// 	#undef COMMON_ENUM_TYPE
// #endif
// #if COMMON_CURRENT_LANGUAGE == COMMON_LANGUAGE_C
// 	#define COMMON_ENUM_TYPE(E) int
// #else
// 	#define COMMON_ENUM_TYPE(E) enum E
// #endif

#ifdef COMMON_STRUCT
	#undef COMMON_STRUCT
#endif
#define COMMON_STRUCT(S) typedef struct S S; struct S

#ifdef COMMON_UNION
	#undef COMMON_UNION
#endif
#define COMMON_UNION(U) typedef union U U; union U

#endif // CORE_INTERNAL_LANGUAGES_SUPPORT_H_
