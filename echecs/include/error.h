#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

#define error(fmt, ...) fprintf(stderr, "[ERROR] : " fmt, ##__VA_ARGS__)

#endif