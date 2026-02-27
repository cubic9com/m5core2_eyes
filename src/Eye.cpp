#include "Eye.h"
#include "EyesAnimation.h"
#include "MathLookup.h"

/**
 * @brief Constructor
 * @param baseX Center X coordinate of the eye
 * @param baseY Center Y coordinate of the eye
 * @param displayX X coordinate on the display
 * @param displayY Y coordinate on the display
 * @param pupilOffsetX X offset for pupil drawing (default: 0)
 * @param pupilOffsetY Y offset for pupil drawing (default: 0)
 */
Eye::Eye(int16_t baseX, int16_t baseY, int16_t displayX, int16_t displayY,
         int16_t pupilOffsetX, int16_t pupilOffsetY) 
  : basePoint(baseX, baseY),
    displayOffset(displayX, displayY),
    pupilPosition(baseX, baseY),
    pupilDrawOffset(pupilOffsetX, pupilOffsetY),
    lastBlinkState(BlinkState::OPEN)
{
  // Create sprite with 1-bit color depth
  canvas.setPsram(false);
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
  canvas.fillEllipse(localCenter.x, localCenter.y, EYE_RADIUS_X, EYE_RADIUS_Y, TFT_WHITE);
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
  Point diff = saccades;
  float dist = FastMath::fastHypot(diff.x, diff.y);
  
  Point result;
  if (dist > 0) {
    // Calculate angle
    float angleDeg = FastMath::radiansToDegrees(FastMath::fastAtan2(diff.y, diff.x));
    float maxDist = getMaxPupilDistanceAtAngle(angleDeg);
    
    if (dist > maxDist) {
      // Scale to fit within ellipse
      float scale = maxDist / dist;
      result.x = static_cast<int16_t>(diff.x * scale) + basePoint.x;
      result.y = static_cast<int16_t>(diff.y * scale) + basePoint.y;
    } else {
      result = basePoint + saccades;
    }
  } else {
    result = basePoint;
  }
  
  updatePupil(result);
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
  
  // Calculate position from angle using fast lookup table
  float angleDegrees = degree + offsetDegree;
  float maxDist = getMaxPupilDistanceAtAngle(angleDegrees);
  Point newPosition(
    static_cast<int16_t>(maxDist * distanceFactor * FastMath::fastCos(angleDegrees)) + basePoint.x,
    static_cast<int16_t>(maxDist * distanceFactor * FastMath::fastSin(angleDegrees)) + basePoint.y
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
  // Apply pupil draw offset for drawing only
  canvas.fillEllipse(localPupil.x + pupilDrawOffset.x, localPupil.y + pupilDrawOffset.y, 
                      PUPIL_RADIUS_X, PUPIL_RADIUS_Y, color);
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
 * @brief Get maximum pupil distance at a given angle
 * @param angleDeg Angle in degrees
 * @return Maximum distance pupil center can move at this angle
 */
float Eye::getMaxPupilDistanceAtAngle(float angleDeg) const {
  float cosA = FastMath::fastCos(angleDeg);
  float sinA = FastMath::fastSin(angleDeg);
  
  // Pupil movement range (radius of the white of the eye - radius of the pupil)
  float a = EYE_RADIUS_X - PUPIL_RADIUS_X;
  float b = EYE_RADIUS_Y - PUPIL_RADIUS_Y;
  
  // Ellipse radius in polar coordinates: r = ab / sqrt((b*cos)² + (a*sin)²)
  float denominator = FastMath::fastHypot(b * cosA, a * sinA);
  float maxDist = (a * b) / denominator;
  
  // Apply a margin factor to prevent the pupil from clinging to the outline of the white of the eye
  return maxDist * 0.80F;
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
  
  // Calculate angle
  float angleDeg = FastMath::radiansToDegrees(FastMath::fastAtan2(diff.y, diff.x));
  
  // Get maximum distance at this angle
  float maxDist = getMaxPupilDistanceAtAngle(angleDeg);
  
  // Calculate distance from center
  float dist = FastMath::fastHypot(diff.x, diff.y);
  
  Point result;
  if (dist > maxDist) {
    // Scale to fit within ellipse
    float scale = maxDist / dist;
    result.x = static_cast<int16_t>(diff.x * scale) + basePoint.x;
    result.y = static_cast<int16_t>(diff.y * scale) + basePoint.y;
  } else {
    // Within boundary, use as is
    result = basePoint + diff;
  }
  
  // Add small movements
  result = result + saccades;
  
  // Check again after adding saccades to ensure we're still within bounds
  Point finalDiff = result - basePoint;
  float finalDist = FastMath::fastHypot(finalDiff.x, finalDiff.y);
  if (finalDist > maxDist) {
    float scale = maxDist / finalDist;
    result.x = static_cast<int16_t>(finalDiff.x * scale) + basePoint.x;
    result.y = static_cast<int16_t>(finalDiff.y * scale) + basePoint.y;
  }
  
  return result;
}
