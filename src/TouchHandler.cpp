#include "TouchHandler.h"

/**
 * @brief Constructor
 */
TouchHandler::TouchHandler() : lastTouchState(TouchState::NONE), touchPoint(0, 0) {
}

/**
 * @brief Update touch state
 * @return Current touch state
 */
TouchState TouchHandler::update() {
  static uint32_t lastUpdateTime = 0;
  uint32_t currentTime = millis();
  
  // Limit touch update frequency (25ms interval)
  if (currentTime - lastUpdateTime < 25) {
    return lastTouchState;
  }
  
  lastUpdateTime = currentTime;
  M5.update();
  auto touch = M5.Touch.getDetail();
  
  TouchState currentState = interpretTouchState(touch.state);
  
  // Update touch position (only when touching)
  if (currentState == TouchState::TOUCHING) {
    touchPoint.x = touch.x;
    touchPoint.y = touch.y;
  }
  
  // Save previous state
  lastTouchState = currentState;
  
  return currentState;
}

/**
 * @brief Get current touch position
 * @return Touch position
 */
const Point& TouchHandler::getTouchPoint() const {
  return touchPoint;
}

/**
 * @brief Get previous touch state
 * @return Previous touch state
 */
TouchState TouchHandler::getLastTouchState() const {
  return lastTouchState;
}

/**
 * @brief Interpret M5Stack touch state
 * @param state M5Stack touch state
 * @return Interpreted touch state
 */
TouchState TouchHandler::interpretTouchState(uint8_t state) {
  // Determine touch state
  switch (state) {
    case m5::touch_state_t::touch:
    case m5::touch_state_t::touch_begin:
    case m5::touch_state_t::hold:
    case m5::touch_state_t::hold_begin:
    case m5::touch_state_t::flick:
    case m5::touch_state_t::flick_begin:
    case m5::touch_state_t::drag:
      return TouchState::TOUCHING;
    case m5::touch_state_t::mask_change :
    case m5::touch_state_t::hold_end:
    case m5::touch_state_t::flick_end:
    case m5::touch_state_t::drag_end:
      return TouchState::RELEASED;
    case m5::touch_state_t::none:
    default:
      return TouchState::NONE;
  }
}
