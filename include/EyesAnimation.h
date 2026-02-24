#pragma once

#include <M5Unified.h>
#include "TouchHandler.h"
#include "Eye.h"

/**
 * @brief Class managing eye animations
 */
class EyesAnimation {
public:
  // Dizzy effect settings
  static constexpr float ACCELERATION_THRESHOLD = 1.05F;
  static constexpr uint8_t NUM_OF_ROTATION = 3;
  static constexpr float DIZZY_ROTATION_SPEED = 15.0F;
  static constexpr float DIZZY_TOTAL_DEGREES = 360.0F * NUM_OF_ROTATION;
  static constexpr float DIZZY_DISTANCE_FACTOR = 1080.0F;
  
  // Blink settings
  static constexpr uint8_t BLINK_INITIAL_MAX = 20;
  static constexpr uint8_t BLINK_RANDOM_MIN = 10;
  static constexpr uint8_t BLINK_RANDOM_MAX = 200;
  static constexpr uint8_t BLINK_HALF_CLOSED_TOP_HEIGHT = 60;
  static constexpr uint8_t BLINK_HALF_CLOSED_BOTTOM_HEIGHT = 25;
  static constexpr uint8_t BLINK_HALF_CLOSED_BOTTOM_Y = 179;
  
  // Animation settings
  static constexpr uint8_t ANIMATION_DELAY_MS = 20;
  static constexpr uint8_t SACCADES_MAX = 11;
  static constexpr uint8_t SACCADES_DIVISOR = 10;
public:
  /**
   * @brief Constructor
   */
  EyesAnimation();
  
  /**
   * @brief Initialization process
   * @return Whether initialization was successful
   */
  bool setup();
  
  /**
   * @brief Main loop process
   */
  void loop();
  
  /**
   * @brief Get touch handler
   * @return Reference to touch handler
   */
  TouchHandler& getTouchHandler();
  
private:
  Eye leftEye;           // Left eye
  Eye rightEye;          // Right eye
  TouchHandler touchHandler; // Touch handler
  EyeState state;        // Eye state
  float degree;          // Dizzy effect angle
  uint8_t blinkCounter;  // Blink counter
  uint8_t blinkMaxCount; // Maximum blink count
  
  /**
   * @brief Reset eyes
   */
  void resetEyes();
  
  /**
   * @brief Draw pupils in the center
   */
  void drawCenterEyes();
  
  /**
   * @brief Draw gaze-following pupils
   * @param touchPoint Touch coordinates
   */
  void drawGazingEyes(const Point& touchPoint);
  
  /**
   * @brief Draw dizzy effect pupils
   */
  void drawDizzyEyes();
  
  /**
   * @brief Draw blink
   */
  void drawBlink();
  
  /**
   * @brief Redraw the white parts of the eyes
   */
  void redrawWhiteEyes();
  
  /**
   * @brief Check accelerometer values and determine whether to start dizzy effect
   * @return true if state changed
   */
  bool checkAccelerationForDizzy();
  
  /**
   * @brief Update eyes based on current state
   */
  void updateEyesBasedOnState();
  
  /**
   * @brief Handle normal state eyes
   */
  void handleNormalState();
  
  /**
   * @brief Render both eyes to display
   */
  void renderEyes();
  
  /**
   * @brief Determine current blink state
   * @return Current blink state
   */
  BlinkState determineBlinkState();
  
  /**
   * @brief Update blink counter
   */
  void updateBlinkCounter();
  
  /**
   * @brief Generate saccades (small eye movements)
   * @return Amount of small movements
   */
  Point generateSaccades();
};
