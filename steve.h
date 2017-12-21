#ifndef _sao_h
#define _sao_h

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define logInfo(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define logError(M, ...) fprintf(stderr, "[ERROR] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif