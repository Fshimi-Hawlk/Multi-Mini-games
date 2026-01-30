#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>

#include "logger.h"

#include "nob/dynamicArray.h"
#include "nob/stringBuilder.h"

#include "baseTypes.h"
#include "configs.h"

#endif // COMMON_H