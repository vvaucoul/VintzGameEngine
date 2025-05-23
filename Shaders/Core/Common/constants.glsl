#ifndef CONSTANTS_GLSL
#define CONSTANTS_GLSL

// ============================================================================
// MATHEMATICAL CONSTANTS
// ============================================================================

const float PI = 3.14159265359;
const float PI_OVER_2 = 1.57079632679; // PI / 2
const float PI_OVER_4 = 0.78539816339; // PI / 4
const float TWO_PI = 6.28318530718;    // 2 * PI
const float INV_PI = 0.31830988618;    // 1 / PI
const float INV_TWO_PI = 0.15915494309; // 1 / (2 * PI)
const float SQRT_2 = 1.41421356237;    // Square root of 2
const float INV_SQRT_2 = 0.70710678118; // 1 / sqrt(2)

// ============================================================================
// NUMERICAL CONSTANTS
// ============================================================================

// Small value to avoid division by zero, handle floating-point inaccuracies,
// or act as a threshold in comparisons. Use cautiously to prevent bias.
const float EPSILON = 1e-5; // 0.00001

// A slightly larger epsilon, sometimes useful for less sensitive comparisons.
const float LOW_EPSILON = 1e-4; // 0.0001

// A very small epsilon, for high precision needs.
const float HIGH_EPSILON = 1e-6; // 0.000001

// ============================================================================
// RENDERING CONSTANTS (Example - Add specific ones as needed)
// ============================================================================

// Example: Maximum reflection bounces (if doing ray tracing/path tracing)
// const int MAX_REFLECTION_BOUNCES = 4;

// Example: Default IOR (Index of Refraction) for air
// const float IOR_AIR = 1.0;


#endif // CONSTANTS_GLSL