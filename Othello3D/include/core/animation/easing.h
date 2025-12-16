#ifndef EASING_H
#define EASING_H

#include "utils/common.h"

/**
 * @file easing.h
 * @brief Common easing functions for animations.
 */

/**
 * @brief Linear easing (no acceleration or deceleration).
 * @param t Normalized time (0 to 1).
 * @return Linearly interpolated value.
 */
float easeLinear(float t);

/**
 * @brief Quadratic easing in (accelerates at start).
 * @param t Normalized time (0 to 1).
 * @return Eased value.
 */
float easeInQuad(float t);

/**
 * @brief Quadratic easing out (decelerates at end).
 * @param t Normalized time (0 to 1).
 * @return Eased value.
 */
float easeOutQuad(float t);

/**
 * @brief Quadratic easing in-out (accelerates in, decelerates out).
 * @param t Normalized time (0 to 1).
 * @return Eased value.
 */
float easeInOutQuad(float t);

/**
 * @brief Cubic easing in (stronger acceleration at start).
 * @param t Normalized time (0 to 1).
 * @return Eased value.
 */
float easeInCubic(float t);

/**
 * @brief Cubic easing out (stronger deceleration at end).
 * @param t Normalized time (0 to 1).
 * @return Eased value.
 */
float easeOutCubic(float t);

/**
 * @brief Cubic easing in-out (strong acceleration in, deceleration out).
 * @param t Normalized time (0 to 1).
 * @return Eased value.
 */
float easeInOutCubic(float t);

/**
 * @brief Bounce easing out (bounces at the end).
 * @param t Normalized time (0 to 1).
 * @return Eased value.
 */
float easeOutBounce(float t);

/**
 * @brief Sinusoidal easing in-out (smooth natural curve).
 * @param t Normalized time (0 to 1).
 * @return Eased value.
 */
float easeInOutSine(float t);

#endif