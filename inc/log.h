#ifndef _LOG_H_
#define _LOG_H_

#include <color.h>
#include <stdio.h>
#include <stdlib.h>

#define SUCCESS(TEXT, ...) \
    fprintf(stderr, "[" COLOR_LIGHT_GREEN "+" COLOR_RESET "] " TEXT "\n", ##__VA_ARGS__)

#define ERROR(TEXT, ...) \
    fprintf(stderr, "[" COLOR_RED "x" COLOR_RESET "] " TEXT "\n", ##__VA_ARGS__); exit(EXIT_FAILURE)

#define DIE(FUNCTION) \
    perror(FUNCTION); exit(EXIT_FAILURE)

#define WARNING(TEXT, ...) \
    fprintf(stderr, "[" COLOR_ORANGE "!" COLOR_RESET "]" TEXT "\n", ##__VA_ARGS__);

#define INFO(TEXT, ...) \
    fprintf(stderr, "[" COLOR_BLUE "*" COLOR_RESET "]" TEXT "\n", ##__VA_ARGS__);

#endif
