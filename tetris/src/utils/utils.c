#include "utils/utils.h"

bool compareColor(Color c1, Color c2) {
    return memcmp(&c1, &c2, sizeof(Color));
}

bool isColorsEqual(Color c1, Color c2) {
    return compareColor(c1, c2) == 0;
}