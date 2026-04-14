/**
    @file utils.h
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief utils.h implementation/header file
*/
#ifndef UTILS_UTILS_H
#define UTILS_UTILS_H

#include "utils/common.h"

/**
    @brief Description for compareColor
    @param[in,out] c1 The c1 parameter
    @param[in,out] c2 The c2 parameter
    @return Success/failure or the result of the function
*/
bool compareColor(Color c1, Color c2);
/**
    @brief Description for isColorsEqual
    @param[in,out] c1 The c1 parameter
    @param[in,out] c2 The c2 parameter
    @return Success/failure or the result of the function
*/
bool isColorsEqual(Color c1, Color c2);

#endif