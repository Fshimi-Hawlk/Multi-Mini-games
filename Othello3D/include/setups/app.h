#ifndef SETUPS_APP_H
#define SETUPS_APP_H

#include "utils/common.h"
#include "utils/types.h"

s64_t initFonts(void);
void freeFonts(void);

s64_t initApp(void);
void freeApp(void);

#endif // SETUPS_APP_H