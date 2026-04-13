#ifndef C_COMPAT_H
#define C_COMPAT_H

/* Shared extern-"C" compatibility macros for headers included from both C and C++. */

#ifdef __cplusplus
#define SCL_EXTERN_C_BEGIN extern "C" {
#define SCL_EXTERN_C_END }
#else
#define SCL_EXTERN_C_BEGIN
#define SCL_EXTERN_C_END
#endif

#endif
