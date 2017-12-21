#ifndef _sao_h
#define _sao_h

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define logInfo(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define logError(M, ...) fprintf(stderr, "[ERROR] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LEN(e) (sizeof(e)/sizeof(e[0]))

typedef float    f32;
typedef double   f64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

// This is like super barebones stuff that's useful everywhere.
// I would really like to have a lot more libraries that I build up and use for things.
// I should continue to try and pull stuff into here as ofted as possible.

// Things I would like good solutions for.
// - http requests (with ssl)
// - web servers
// - threading stuff
// - command line parsing

// naming
// moduleTypeAction is probably best
// so that would be like arenaCreate
// sometimes the module is the same as it's main type so just drop the first bit.

#endif