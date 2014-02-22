#ifndef XGLOBAL_H
#define XGLOBAL_H

#include <stdint.h>
#include <cstddef>

#if defined(_WIN32) || defined(_WIN64)
#define X_WIN
#endif

#if defined(__APPLE__)
#define X_MAC
#endif

#ifdef _MSC_VER
#define X_MSVC
#endif

#ifdef X_WIN
# define X_DECL_EXPORT __declspec(dllexport)
# define X_DECL_IMPORT __declspec(dllimport)
#else
# define X_DECL_EXPORT
# define X_DECL_IMPORT
#endif

#ifndef EKSCORE_STATIC
# ifdef EKSCORE_BUILD
#  define EKSCORE_EXPORT X_DECL_EXPORT
# else
#  define EKSCORE_EXPORT X_DECL_IMPORT
# endif
#else
# define EKSCORE_EXPORT
#endif

#ifdef X_DEBUG
# define X_ASSERTS_ENABLED 1

# if !(defined(X_ARCH_ARM) || defined(X_MAC))
#  define X_ENABLE_HEAP_CHECK
# endif

#endif

#ifndef X_EVENT_LOGGING_ENABLED
# define X_EVENT_LOGGING_ENABLED 0
#endif

#ifdef X_ENABLE_HEAP_CHECK
# define X_HEAP_CHECK { xAssert(_heapchk() == _HEAPOK); xAssert(_CrtCheckMemory()); }
#else
# define X_HEAP_CHECK
#endif

#ifdef X_ARCH_ARM
# define X_ADVANCED_ASSERT 0
# define X_COMPILE_ARRAY_MATH 0
# define X_VECTOR_IS_QVECTOR 0
# define X_QCOLOUR_COMPATIBILITY 0
# define X_QT_INTEROP 0
# define X_STL_INTEROP 0
#else
# define X_ADVANCED_ASSERT 1
# define X_COMPILE_ARRAY_MATH 1
# define X_VECTOR_IS_QVECTOR 1
# define X_QCOLOUR_COMPATIBILITY 1
# define X_QT_INTEROP 1
# define X_STL_INTEROP 1
#endif

#if defined(X_DEBUG) && !defined(X_ARCH_ARM)
# define X_ENABLE_STACK_WALKING 1
#else
# define X_ENABLE_STACK_WALKING 0
#endif


#define X_ASSERT_VALUE_HANDLING (X_QT_INTEROP && X_ADVANCED_ASSERT)

typedef int8_t xint8;         /* 8 bit signed */
typedef uint8_t xuint8;       /* 8 bit unsigned */
typedef int16_t xint16;       /* 16 bit signed */
typedef uint16_t xuint16;     /* 16 bit unsigned */
typedef int32_t xint32;       /* 32 bit signed */
typedef uint32_t xuint32;     /* 32 bit unsigned */
typedef int64_t xint64;       /* 64 bit signed */
typedef uint64_t xuint64;     /* 46 bit unsigned */
typedef std::size_t xsize;
typedef std::ptrdiff_t xptrdiff;

#if 0
#define X_INT64_C(c) static_cast<long long>(c ## LL)     /* signed 64 bit constant */
#define X_UINT64_C(c) static_cast<unsigned long long>(c ## ULL) /* unsigned 64 bit constant */

#define X_SIZE_SENTINEL ((xsize)-1)
#define X_UINT64_SENTINEL ((xuint64)-1)
#define X_UINT32_SENTINEL ((xuint32)-1)
#define X_UINT16_SENTINEL ((xuint16)-1)
#define X_UINT8_SENTINEL ((xuint8)-1)
#define X_INT16_MAX ((xint16)32767)
#define X_INT16_MIN ((xint16)-32768)
#define X_INT64_MAX (X_INT64_C(9223372036854775807LL))
#define X_INT64_MIN (X_INT64_C(-9223372036854775808))

#endif

#define X_OVERRIDE override
#define X_FINAL final
#define X_CONST_EXPR //Q_DECL_CONSTEXPR

#define X_HACK(level, why, suggestion)

#define X_ARRAY_COUNT(x) (sizeof(x)/sizeof(x[0]))

#endif // XGLOBAL_H
