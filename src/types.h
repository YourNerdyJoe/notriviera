#pragma once
//#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>

//#define alignof _Alignof
//#define alignas _Alignas

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

typedef bool b8;
typedef u32  b32;

#define cast(type, v) (*(type*)&v)
#define arrlen(arr) (sizeof(arr)/sizeof(*(arr)))

#define for_each_arr(type, it, arr) for(type it = (arr); it < (arr) + arrlen(arr); it++)
#define for_each_arr_rev(type, it, arr) for(type it = (arr) + arrlen(arr) - 1; it >= (arr); it--)

#ifdef __cplusplus
#define BEGIN_C_DECLS extern "C" {
#define END_C_DECLS }
#else
#define BEGIN_C_DECLS
#define END_C_DECLS
#endif
