#include "EyesAnimation.h"

// Constants for blink state transitions
static constexpr uint8_t BLINK_HALF_CLOSED_FRAME1 = 1;
static constexpr uint8_t BLINK_CLOSED_FRAME_START = 2;
static constexpr uint8_t BLINK_CLOSED_FRAME_END = 5;
static constexpr uint8_t BLINK_HALF_CLOSED_FRAME2 = 6;

/**
 * @brief Constructor
 */
EyesAnimation::EyesAnimation() 
  : leftEye(
      Eye::EYE_LEFT_X, 
      Eye::EYE_BASE_Y, 
      0, 
      Eye::DISPLAY_BASE_Y
    ),
    rightEye(
      Eye::EYE_RIGHT_X, 
      Eye::EYE_BASE_Y, 
      Eye::SPRITE_WIDTH, 
      Eye::DISPLAY_BASE_Y
    ),
    state(EyeState::NORMAL),
    degree(0.0F),
    blinkCounter(0),
    blinkMaxCount(BLINK_INITIAL_MAX)
{
}

/**
 * @brief Initialization process
 * @return Whether initialization was successful
 */
bool EyesAnimation::setup() {
  try {    
    // Initial drawing
    resetEyes();
    
    // Check IMU initialization
    float ax, ay, az;
    if (!M5.Imu.getAccel(&ax, &ay, &az)) {
      Serial.println("Warning: IMU initialization failed. Dizzy effect may not work.");
    }
    
    return true;
  } catch (const std::exception& e) {
    Serial.printf("Setup error: %s\n", e.what());
    return false;
  } catch (...) {
    Serial.println("Unknown setup error occurred");
    return false;
  }
}

/**
 * @brief Main loop process
 */
void EyesAnimation::loop() {
  static uint32_t lastFrameTime = 0;
  static uint32_t lastAccelCheckTime = 0;
  static bool needsRedraw = true;
  uint32_t currentTime = millis();
  
  // Frame rate control (skip if not enough time has passed since last drawing)
  if (currentTime - lastFrameTime < ANIMATION_DELAY_MS) {
    return;
  }
  
  lastFrameTime = currentTime;
  
  // Check accelerometer (10ms interval)
  if (currentTime - lastAccelCheckTime > 10) {
    lastAccelCheckTime = currentTime;
    if (checkAccelerationForDizzy()) {
      needsRedraw = true;
    }
  }
  
  // Draw pupils according to state (redraw white parts only if necessary)
  if (needsRedraw) {
    redrawWhiteEyes();
    needsRedraw = false;
  }
  
  updateEyesBasedOnState();
  
  // Display sprites (update both eyes at once)
  renderEyes();
}

/**
 * @brief Redraw the white parts of the eyes
 */
void EyesAnimation::redrawWhiteEyes() {
  leftEye.clear();
  rightEye.clear();
  leftEye.drawWhite();
  rightEye.drawWhite();
}

/**
 * @brief Check accelerometer values and determine whether to start dizzy effect
 * @return true if state changed
 */
bool EyesAnimation::checkAccelerationForDizzy() {
  float ax, ay, az;
  if (M5.Imu.getAccel(&ax, &ay, &az)) {
    float totalAcc = sqrt(ax * ax + ay * ay + az * az);
    if (totalAcc > ACCELERATION_THRESHOLD && state != EyeState::DIZZY) {
      state = EyeState::DIZZY;
      degree = 0.0F;
      return true;
    }
  }
  return false;
}

/**
 * @brief Update eyes based on current state
 */
void EyesAnimation::updateEyesBasedOnState() {
  if (state == EyeState::DIZZY) {
    drawDizzyEyes();
  } else {
    handleNormalState();
  }
}

/**
 * @brief Handle normal state eyes
 */
void EyesAnimation::handleNormalState() {
  static TouchState lastTouchState = TouchState::NONE;
  static uint32_t lastBlinkTime = 0;
  uint32_t currentTime = millis();
  
  // Process touch input
  TouchState touchState = touchHandler.update();
  
  // Process only if touch state has changed
  if (touchState != lastTouchState) {
    lastTouchState = touchState;
    
    if (touchState == TouchState::RELEASED) {
      resetEyes();
      return;
    }
  }
  
  // Get blink state
  BlinkState currentBlinkState = determineBlinkState();
  
  // Process gaze only when not blinking
  if (currentBlinkState == BlinkState::OPEN) {
    // Follow gaze while touching
    if (touchState == TouchState::TOUCHING) {
      drawGazingEyes(touchHandler.getTouchPoint());
    } else {
      // Center gaze when not touching
      drawCenterEyes();
    }
  }
  
  // Update blink (50ms interval)
  if (currentTime - lastBlinkTime >= 50) {
    lastBlinkTime = currentTime;
    drawBlink();
  }
}

/**
 * @brief Render both eyes to display
 */
void EyesAnimation::renderEyes() {
  leftEye.render(&M5.Display);
  rightEye.render(&M5.Display);
}

/**
 * @brief Get touch handler
 * @return Reference to touch handler
 */
TouchHandler& EyesAnimation::getTouchHandler() {
  return touchHandler;
}

/**
 * @brief Reset eyes
 */
void EyesAnimation::resetEyes() {
  leftEye.clear();
  rightEye.clear();
  leftEye.drawWhite();
  rightEye.drawWhite();
  leftEye.resetPupil();
  rightEye.resetPupil();
  state = EyeState::NORMAL;
}

/**
 * @brief Draw pupils in the center
 */
void EyesAnimation::drawCenterEyes() {
  Point saccades = generateSaccades();
  leftEye.drawCenterPupil(saccades);
  rightEye.drawCenterPupil(saccades);
}

/**
 * @brief Draw gaze-following pupils
 * @param touchPoint Touch coordinates
 */
void EyesAnimation::drawGazingEyes(const Point& touchPoint) {
  Point saccades = generateSaccades();
  leftEye.drawGazingPupil(touchPoint, saccades);
  rightEye.drawGazingPupil(touchPoint, saccades);
}

/**
 * @brief Draw dizzy effect pupils
 */
void EyesAnimation::drawDizzyEyes() {
  // Use different angle offsets for left and right eyes
  leftEye.drawDizzyPupil(degree);
  rightEye.drawDizzyPupil(degree, 180.0F);
  
  // Progress and end determination of dizzy effect
  degree += DIZZY_ROTATION_SPEED;
  if (degree >= DIZZY_TOTAL_DEGREES) {
    degree = 0.0F;
    state = EyeState::NORMAL;
  }
}

/**
 * @brief Draw blink
 */
void EyesAnimation::drawBlink() {
  BlinkState blinkState = determineBlinkState();
  
  leftEye.drawBlink(blinkState);
  rightEye.drawBlink(blinkState);
  
  updateBlinkCounter();
}

/**
 * @brief Determine current blink state
 * @return Current blink state
 */
BlinkState EyesAnimation::determineBlinkState() {
  if (blinkCounter == BLINK_HALF_CLOSED_FRAME1 || 
      blinkCounter == BLINK_HALF_CLOSED_FRAME2) {
    return BlinkState::HALF_CLOSED;
  } else if (blinkCounter >= BLINK_CLOSED_FRAME_START && 
             blinkCounter <= BLINK_CLOSED_FRAME_END) {
    return BlinkState::CLOSED;
  }
  
  return BlinkState::OPEN;
}

/**
 * @brief Update blink counter
 */
void EyesAnimation::updateBlinkCounter() {
  blinkCounter++;
  if (blinkCounter > blinkMaxCount) {
    blinkCounter = 0;
    blinkMaxCount = random(BLINK_RANDOM_MIN, BLINK_RANDOM_MAX);
  }
}

/**
 * @brief Generate saccades (small eye movements)
 * @return Amount of small movements
 */
Point EyesAnimation::generateSaccades() {
  static uint32_t lastSaccadeTime = 0;
  static Point lastSaccade(0, 0);
  uint32_t currentTime = millis();
  
  // Limit saccade update frequency (50ms interval)
  if (currentTime - lastSaccadeTime < 50) {
    return lastSaccade;
  }
  
  lastSaccadeTime = currentTime;
  lastSaccade.x = random(SACCADES_MAX) / SACCADES_DIVISOR;
  lastSaccade.y = random(SACCADES_MAX) / SACCADES_DIVISOR;
  
  return lastSaccade;
}
