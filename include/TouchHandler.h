#pragma once

#include <M5Unified.h>

/**
 * @brief Structure representing coordinates
 */
struct Point {
  int16_t x;
  int16_t y;
  
  Point() : x(0), y(0) {}
  Point(int16_t _x, int16_t _y) : x(_x), y(_y) {}
  
  Point operator+(const Point& other) const {
    return Point(x + other.x, y + other.y);
  }
  
  Point operator-(const Point& other) const {
    return Point(x - other.x, y - other.y);
  }
};

/**
 * @brief Enumeration representing touch state
 */
enum class TouchState {
  NONE,       // No touch
  TOUCHING,   // Touching
  RELEASED,   // Touch released
  MULTI_TOUCH // Multiple touches
};

/**
 * @brief Class managing touch input
 */
class TouchHandler {
public:
  /**
   * @brief Constructor
   */
  TouchHandler();
  
  /**
   * @brief Update touch state
   * @return Current touch state
   */
  TouchState update();
  
  /**
   * @brief Get current touch position
   * @return Touch position
   */
  const Point& getTouchPoint() const;
  
  /**
   * @brief Get previous touch state
   * @return Previous touch state
   */
  TouchState getLastTouchState() const;
  
private:
  TouchState lastTouchState;  // Previous touch state
  Point touchPoint;           // Touch position
  
  /**
   * @brief Interpret M5Stack touch state
   * @param state M5Stack touch state
   * @return Interpreted touch state
   */
  TouchState interpretTouchState(uint8_t state);
};
