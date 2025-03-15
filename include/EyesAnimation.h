#pragma once

#include <M5Unified.h>
#include "TouchHandler.h"
#include "Eye.h"

/**
 * @brief 目のアニメーションを管理するクラス
 */
class EyesAnimation {
public:
  // めまい効果の設定
  static constexpr float ACCELERATION_THRESHOLD = 1.4F;
  static constexpr uint8_t NUM_OF_ROTATION = 3;
  static constexpr float DIZZY_ROTATION_SPEED = 15.0F;
  static constexpr float DIZZY_TOTAL_DEGREES = 360.0F * NUM_OF_ROTATION;
  static constexpr float DIZZY_DISTANCE_FACTOR = 1080.0F;
  
  // まばたきの設定
  static constexpr uint8_t BLINK_INITIAL_MAX = 20;
  static constexpr uint8_t BLINK_RANDOM_MIN = 10;
  static constexpr uint8_t BLINK_RANDOM_MAX = 200;
  static constexpr uint8_t BLINK_HALF_CLOSED_TOP_HEIGHT = 60;
  static constexpr uint8_t BLINK_HALF_CLOSED_BOTTOM_HEIGHT = 7;
  static constexpr uint8_t BLINK_HALF_CLOSED_BOTTOM_Y = 179;
  
  // アニメーションの設定
  static constexpr uint8_t ANIMATION_DELAY_MS = 20;
  static constexpr uint8_t SACCADES_MAX = 11;
  static constexpr uint8_t SACCADES_DIVISOR = 10;
public:
  /**
   * @brief コンストラクタ
   */
  EyesAnimation();
  
  /**
   * @brief 初期化処理
   * @return 初期化が成功したかどうか
   */
  bool setup();
  
  /**
   * @brief メインループ処理
   */
  void loop();
  
  /**
   * @brief タッチハンドラーを取得する
   * @return タッチハンドラーへの参照
   */
  TouchHandler& getTouchHandler();
  
private:
  Eye leftEye;           // 左目
  Eye rightEye;          // 右目
  TouchHandler touchHandler; // タッチハンドラー
  EyeState state;        // 目の状態
  float degree;          // めまい効果の角度
  uint8_t blinkCounter;  // まばたきカウンター
  uint8_t blinkMaxCount; // まばたきの最大カウント
  
  /**
   * @brief 目をリセットする
   */
  void resetEyes();
  
  /**
   * @brief 中央に瞳孔を描画する
   */
  void drawCenterEyes();
  
  /**
   * @brief 視線追従の瞳孔を描画する
   * @param touchPoint タッチされた座標
   */
  void drawGazingEyes(const Point& touchPoint);
  
  /**
   * @brief めまい効果の瞳孔を描画する
   */
  void drawDizzyEyes();
  
  /**
   * @brief まばたきを描画する
   */
  void drawBlink();
  
  /**
   * @brief 白目部分を再描画する
   */
  void redrawWhiteEyes();
  
  /**
   * @brief 加速度センサーの値を確認し、めまい効果を開始するかどうかを判断する
   * @return 状態が変化した場合はtrue
   */
  bool checkAccelerationForDizzy();
  
  /**
   * @brief 現在の状態に基づいて目を更新する
   */
  void updateEyesBasedOnState();
  
  /**
   * @brief 通常状態の目の処理
   */
  void handleNormalState();
  
  /**
   * @brief 両目をディスプレイに描画する
   */
  void renderEyes();
  
  /**
   * @brief 現在のまばたき状態を決定する
   * @return 現在のまばたき状態
   */
  BlinkState determineBlinkState();
  
  /**
   * @brief まばたきカウンターを更新する
   */
  void updateBlinkCounter();
  
  /**
   * @brief サッケード（微小な目の動き）を生成する
   * @return 微小な動きの量
   */
  Point generateSaccades();
};
