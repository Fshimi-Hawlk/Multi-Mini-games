#include "core/animation/easing.h"

// float easeLinear(float t) {
//     return t;
// }


// float easeInQuad(float t) {
//     return t * t;
// }

// float easeOutQuad(float t) {
//     return 1.0f - (1.0f - t) * (1.0f - t);
// }

// float easeInOutQuad(float t) {
//     if (t < 0.5f) {
//         return 2.0f * t * t;
//     }
//     return -1.0f + 4.0f * t - 2.0f * t * t;
// }


// float easeInCubic(float t) {
//     return t * t * t;
// }

// float easeOutCubic(float t) {
//     float t1 = t - 1.0f;
//     return 1.0f + t1 * t1 * t1;
// }

// float easeInOutCubic(float t) {
//     if (t < 0.5f) {
//         return 4.0f * t * t * t;
//     }
//     float t1 = t - 1.0f;
//     return 1.0f + 4.0f * t1 * t1 * t1;
// }


// float easeOutBounce(float t) {
//     if (t < 1.0f / 2.75f) {
//         return 7.5625f * t * t;
//     } else if (t < 2.0f / 2.75f) {
//         t -= 1.5f / 2.75f;
//         return 7.5625f * t * t + 0.75f;
//     } else if (t < 2.5f / 2.75f) {
//         t -= 2.25f / 2.75f;
//         return 7.5625f * t * t + 0.9375f;
//     } else {
//         t -= 2.625f / 2.75f;
//         return 7.5625f * t * t + 0.984375f;
//     }
// }


// float easeInOutSine(float t) {
//     return -0.5f * (cosf(M_PI * t) - 1.0f);
// }