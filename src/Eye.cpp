#include "Eye.h"
#include "EyesAnimation.h"

/**
 * @brief Constructor
 * @param baseX Center X coordinate of the eye
 * @param baseY Center Y coordinate of the eye
 * @param displayX X coordinate on the display
 * @param displayY Y coordinate on the display
 */
Eye::Eye(int16_t baseX, int16_t baseY, int16_t displayX, int16_t displayY) 
  : basePoint(baseX, baseY),
    displayOffset(displayX, displayY),
    pupilPosition(baseX, baseY),
    lastBlinkState(BlinkState::OPEN)
{
  // Create sprite with 1-bit color depth
  canvas.setColorDepth(1);
  canvas.createSprite(SPRITE_WIDTH, SPRITE_HEIGHT);
  clear();
}

/**
 * @brief Clear the eye
 */
void Eye::clear() {
  canvas.fillScreen(TFT_BLACK);
}

/**
 * @brief Draw the white part of the eye
 */
void Eye::drawWhite() {
  Point localCenter = toLocalCoordinates(basePoint);
  canvas.fillCircle(localCenter.x, localCenter.y, EYE_RADIUS, TFT_WHITE);
}

/**
 * @brief Reset pupil to initial position
 */
void Eye::resetPupil() {
  erasePupil();
  pupilPosition = basePoint;
  drawPupil();
}

/**
 * @brief Draw pupil in the center (with small random movements)
 * @param saccades Amount of small movements
 */
void Eye::drawCenterPupil(const Point& saccades) {
  Point newPosition = basePoint + saccades;
  updatePupil(newPosition);
}

/**
 * @brief Draw gaze-following pupil
 * @param targetPoint Target point of the gaze
 * @param saccades Amount of small movements
 */
void Eye::drawGazingPupil(const Point& targetPoint, const Point& saccades) {
  Point newPosition = computeGazingPosition(targetPoint, saccades);
  updatePupil(newPosition);
}

/**
 * @brief Draw dizzy effect pupil
 * @param degree Rotation angle
 * @param offsetDegree Angle offset (default value: 0.0F)
 */
void Eye::drawDizzyPupil(float degree, float offsetDegree) {
  // Calculate distance factor (closer to center as angle increases)
  float distanceFactor = 1.0F - (degree / EyesAnimation::DIZZY_DISTANCE_FACTOR);
  float distance = MAX_PUPIL_DISTANCE * distanceFactor;
  
  // Calculate position from angle (DEG_TO_RAD = M_PI / 180.0F)
  float angleRad = (degree + offsetDegree) * DEG_TO_RAD;
  Point newPosition(
    static_cast<int16_t>(distance * cos(angleRad)) + basePoint.x,
    static_cast<int16_t>(distance * sin(angleRad)) + basePoint.y
  );
  
  updatePupil(newPosition);
}

/**
 * @brief Draw blink
 * @param state Blink state
 */
void Eye::drawBlink(BlinkState state) {
  // Do nothing if state is the same as before
  if (state == lastBlinkState) {
    return;
  }
  
  // Display base Y coordinate in local coordinate system
  int16_t localBaseY = DISPLAY_BASE_Y - displayOffset.y;
  
  switch (state) {
    case BlinkState::HALF_CLOSED:
      // Half-closed state - draw black rectangles at top and bottom
      canvas.fillRect(
        0, 
        localBaseY, 
        SPRITE_WIDTH, 
        EyesAnimation::BLINK_HALF_CLOSED_TOP_HEIGHT, 
        TFT_BLACK
      );
      canvas.fillRect(
        0, 
        EyesAnimation::BLINK_HALF_CLOSED_BOTTOM_Y - displayOffset.y, 
        SPRITE_WIDTH, 
        EyesAnimation::BLINK_HALF_CLOSED_BOTTOM_HEIGHT, 
        TFT_BLACK
      );
      break;
      
    case BlinkState::CLOSED:
      // Completely closed state - fill entire screen with black (using fillScreen for optimization)
      canvas.fillScreen(TFT_BLACK);
      break;
      
    case BlinkState::OPEN:
      // Open state - redraw the white part of the eye
      drawWhite();
      drawPupil();
      break;
      
    default:
      // Do nothing for unknown states
      break;
  }
  
  // Update state
  lastBlinkState = state;
}

/**
 * @brief Render sprite to display
 * @param display Display object
 */
void Eye::render(M5GFX* display) {
  canvas.pushSprite(display, displayOffset.x, displayOffset.y);
}

/**
 * @brief Erase the pupil
 */
void Eye::erasePupil() {
  drawPupilWithColor(TFT_WHITE);
}

/**
 * @brief Draw the pupil
 */
void Eye::drawPupil() {
  drawPupilWithColor(TFT_BLACK);
}

/**
 * @brief Draw pupil with specified color
 * @param color Drawing color
 */
void Eye::drawPupilWithColor(uint16_t color) {
  Point localPupil = toLocalCoordinates(pupilPosition);
  canvas.fillCircle(localPupil.x, localPupil.y, PUPIL_RADIUS, color);
}

/**
 * @brief Update the pupil
 * @param newPosition New position of the pupil
 */
void Eye::updatePupil(const Point& newPosition) {
  // Do nothing if position hasn't changed
  if (pupilPosition.x == newPosition.x && pupilPosition.y == newPosition.y) {
    return;
  }
  
  erasePupil();
  pupilPosition = newPosition;
  drawPupil();
}

/**
 * @brief Convert global coordinates to local coordinates within the sprite
 * @param globalPoint Global coordinates
 * @return Local coordinates within the sprite
 */
Point Eye::toLocalCoordinates(const Point& globalPoint) const {
  return Point(
    globalPoint.x - displayOffset.x,
    globalPoint.y - displayOffset.y
  );
}

/**
 * @brief Calculate pupil position for gaze following
 * @param targetPoint Target point of the gaze
 * @param saccades Amount of small movements
 * @return New position of the pupil
 */
Point Eye::computeGazingPosition(const Point& targetPoint, const Point& saccades) {
  // Difference vector from eye center to target point
  Point diff = targetPoint - basePoint;
  
  // Calculate angle and distance
  double angle = atan2(diff.y, diff.x);
  double dist = std::hypot(diff.x, diff.y);
  
  Point result;
  if (dist > MAX_PUPIL_DISTANCE) {
    // If exceeding maximum distance, limit to maximum distance
    result.x = static_cast<int16_t>(MAX_PUPIL_DISTANCE * cos(angle)) + basePoint.x;
    result.y = static_cast<int16_t>(MAX_PUPIL_DISTANCE * sin(angle)) + basePoint.y;
  } else {
    // If within maximum distance, use as is
    result = basePoint + diff;
  }
  
  // Add small movements
  result = result + saccades;
  
  return result;
}
