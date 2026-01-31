#pragma once

#include <Arduino.h>

/**
 * @brief Fast math functions using lookup tables
 * 
 * This namespace provides optimized trigonometric functions using
 * pre-calculated lookup tables to improve performance on embedded systems.
 */
namespace FastMath {
  // Lookup tables for sin/cos (360 degrees, 1 degree per entry)
  extern const float SIN_TABLE[360];
  extern const float COS_TABLE[360];
  
  /**
   * @brief Fast cosine function using lookup table
   * @param degrees Angle in degrees
   * @return Cosine value
   */
  inline float fastCos(float degrees) {
    // Normalize angle to 0-359 range
    int angle = static_cast<int>(degrees) % 360;
    if (angle < 0) angle += 360;
    return COS_TABLE[angle];
  }
  
  /**
   * @brief Fast sine function using lookup table
   * @param degrees Angle in degrees
   * @return Sine value
   */
  inline float fastSin(float degrees) {
    // Normalize angle to 0-359 range
    int angle = static_cast<int>(degrees) % 360;
    if (angle < 0) angle += 360;
    return SIN_TABLE[angle];
  }
  
  /**
   * @brief Fast atan2 approximation
   * @param y Y coordinate
   * @param x X coordinate
   * @return Angle in radians
   */
  inline float fastAtan2(float y, float x) {
    // Use standard atan2 for now as it's called less frequently
    // and the approximation complexity doesn't justify the minimal gain
    return atan2(y, x);
  }
  
  /**
   * @brief Fast hypot calculation (Euclidean distance)
   * @param x X coordinate
   * @param y Y coordinate
   * @return Distance
   */
  inline float fastHypot(float x, float y) {
    // Use direct calculation which is faster than std::hypot on ESP32
    return sqrtf(x * x + y * y);
  }
  
  /**
   * @brief Convert radians to degrees
   * @param radians Angle in radians
   * @return Angle in degrees
   */
  inline float radiansToDegrees(float radians) {
    return radians * (180.0F / PI);
  }
  
  /**
   * @brief Convert degrees to radians
   * @param degrees Angle in degrees
   * @return Angle in radians
   */
  inline float degreesToRadians(float degrees) {
    return degrees * (PI / 180.0F);
  }
}
