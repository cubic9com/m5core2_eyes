#include <M5Unified.h>
#include "EyesAnimation.h"

/**
 * @brief ディスプレイの設定
 */
static constexpr uint8_t DISPLAY_ROTATION = 1;      // 横向き
static constexpr uint8_t DISPLAY_BRIGHTNESS = 128;  // 明るさ（0-255）

/**
 * @brief 目のアニメーションインスタンス
 */
EyesAnimation eyes;

/**
 * @brief 初期化処理
 */
void setup() {
  // M5Stackの初期化
  auto cfg = M5.config();
  cfg.internal_imu = true;  // IMU（加速度センサー）を有効化
  M5.begin(cfg);
  
  // ディスプレイの初期化と高速化設定
  M5.Display.init();
  M5.Display.setRotation(DISPLAY_ROTATION);
  M5.Display.setBrightness(DISPLAY_BRIGHTNESS);
  M5.Display.setColorDepth(1);  // 1ビット色深度に設定

  // 目のアニメーションの初期化
  eyes.setup();

  // 描画開始（連続描画モード）
  M5.Display.startWrite();
}

/**
 * @brief メインループ処理
 */
void loop() {
  eyes.loop();
}
