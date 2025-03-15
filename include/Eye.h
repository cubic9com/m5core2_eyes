#pragma once

#include <M5Unified.h>
#include "TouchHandler.h"

/**
 * @brief Enumeration representing eye state
 */
enum class EyeState {
  NORMAL,    // Normal state
  GAZING,    // Gaze following state
  DIZZY      // Dizzy state
};

/**
 * @brief Enumeration representing blink state
 */
enum class BlinkState {
  OPEN,           // Open
  HALF_CLOSED,    // Half closed
  CLOSED          // Completely closed
};


/**
 * @brief Class managing a single eye
 */
class Eye {
public:
  // Eye settings
  static constexpr uint8_t EYE_RADIUS = 65;
  static constexpr uint8_t PUPIL_RADIUS = 13;
  static constexpr uint8_t MAX_PUPIL_DISTANCE = 40;
  
  // Sprite settings
  static constexpr uint8_t SPRITE_WIDTH = 160;
  static constexpr uint8_t SPRITE_HEIGHT = 131;
  
  // Eye position
  static constexpr uint8_t EYE_BASE_Y = 120;
  static constexpr uint8_t EYE_LEFT_X = 80;
  static constexpr uint8_t EYE_RIGHT_X = 240;
  static constexpr uint8_t DISPLAY_BASE_Y = 55;
  
public:
  /**
   * @brief Constructor
   * @param baseX Center X coordinate of the eye
   * @param baseY Center Y coordinate of the eye
   * @param displayX X coordinate on the display
   * @param displayY Y coordinate on the display
   */
  Eye(int16_t baseX, int16_t baseY, int16_t displayX, int16_t displayY);
  
  /**
   * @brief Clear the eye
   */
  void clear();
  
  /**
   * @brief Draw the white part of the eye
   */
  void drawWhite();
  
  /**
   * @brief Reset pupil to initial position
   */
  void resetPupil();
  
  /**
   * @brief Draw pupil in the center (with small random movements)
   * @param saccades Amount of small movements
   */
  void drawCenterPupil(const Point& saccades);
  
  /**
   * @brief Draw gaze-following pupil
   * @param targetPoint Target point of the gaze
   * @param saccades Amount of small movements
   */
  void drawGazingPupil(const Point& targetPoint, const Point& saccades);
  
  /**
   * @brief Draw dizzy effect pupil
   * @param degree Rotation angle
   * @param offsetDegree Angle offset
   */
  void drawDizzyPupil(float degree, float offsetDegree = 0.0F);
  
  /**
   * @brief Draw blink
   * @param state Blink state
   */
  void drawBlink(BlinkState state);
  
  /**
   * @brief Render sprite to display
   * @param display Display object
   */
  void render(M5GFX* display);
  
private:
  Point basePoint;       // Center coordinates of the eye
  Point displayOffset;   // Offset on the display
  Point pupilPosition;   // Current position of the pupil
  BlinkState lastBlinkState; // Previous blink state
  M5Canvas canvas;       // Canvas for drawing
  
  /**
   * @brief Erase the pupil
   */
  void erasePupil();
  
  /**
   * @brief Draw the pupil
   */
  void drawPupil();
  
  /**
   * @brief Update the pupil
   * @param newPosition New position of the pupil
   */
  void updatePupil(const Point& newPosition);
  
  /**
   * @brief Calculate pupil position for gaze following
   * @param targetPoint Target point of the gaze
   * @param saccades Amount of small movements
   * @return New position of the pupil
   */
  Point computeGazingPosition(const Point& targetPoint, const Point& saccades);
  
  /**
   * @brief Convert global coordinates to local coordinates within the sprite
   * @param globalPoint Global coordinates
   * @return Local coordinates within the sprite
   */
  Point toLocalCoordinates(const Point& globalPoint) const;
  
  /**
   * @brief Draw pupil with specified color
   * @param color Drawing color
   */
  void drawPupilWithColor(uint16_t color);
};
