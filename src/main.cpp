#include <M5Unified.h>
#include "EyesAnimation.h"

/**
 * @brief Display settings
 */
static constexpr uint8_t DISPLAY_ROTATION = 1;      // Landscape orientation
static constexpr uint8_t DISPLAY_BRIGHTNESS = 128;  // Brightness (0-255)

/**
 * @brief Eye animation instance
 */
EyesAnimation eyes;

/**
 * @brief Initialization process
 */
void setup() {
  // Initialize M5Stack
  auto cfg = M5.config();
  cfg.internal_imu = true;  // Enable IMU (accelerometer)
  M5.begin(cfg);
  
  // Initialize display and optimize settings
  M5.Display.init();
  M5.Display.setRotation(DISPLAY_ROTATION);
  M5.Display.setBrightness(DISPLAY_BRIGHTNESS);
  M5.Display.setColorDepth(1);  // Set to 1-bit color depth

  // Initialize eye animation
  eyes.setup();

  // Start drawing (continuous drawing mode)
  M5.Display.startWrite();
}

/**
 * @brief Main loop process
 */
void loop() {
  eyes.loop();
}
