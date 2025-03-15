#include "EyesAnimation.h"

// まばたきの状態遷移用定数
static constexpr uint8_t BLINK_HALF_CLOSED_FRAME1 = 1;
static constexpr uint8_t BLINK_CLOSED_FRAME_START = 2;
static constexpr uint8_t BLINK_CLOSED_FRAME_END = 5;
static constexpr uint8_t BLINK_HALF_CLOSED_FRAME2 = 6;

/**
 * @brief コンストラクタ
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
 * @brief 初期化処理
 * @return 初期化が成功したかどうか
 */
bool EyesAnimation::setup() {
  try {    
    // 初期描画
    resetEyes();
    
    // IMUの初期化確認
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
 * @brief メインループ処理
 */
void EyesAnimation::loop() {
  static uint32_t lastFrameTime = 0;
  static uint32_t lastAccelCheckTime = 0;
  static bool needsRedraw = true;
  uint32_t currentTime = millis();
  
  // フレームレート制御（前回の描画から一定時間経過していない場合はスキップ）
  if (currentTime - lastFrameTime < ANIMATION_DELAY_MS) {
    return;
  }
  
  lastFrameTime = currentTime;
  
  // 加速度センサーの確認（10ms間隔）
  if (currentTime - lastAccelCheckTime > 10) {
    lastAccelCheckTime = currentTime;
    if (checkAccelerationForDizzy()) {
      needsRedraw = true;
    }
  }
  
  // 状態に応じた瞳孔の描画（必要な場合のみ白目を再描画）
  if (needsRedraw) {
    redrawWhiteEyes();
    needsRedraw = false;
  }
  
  updateEyesBasedOnState();
  
  // スプライトの表示（両目を一度に更新）
  renderEyes();
}

/**
 * @brief 白目部分を再描画する
 */
void EyesAnimation::redrawWhiteEyes() {
  leftEye.clear();
  rightEye.clear();
  leftEye.drawWhite();
  rightEye.drawWhite();
}

/**
 * @brief 加速度センサーの値を確認し、めまい効果を開始するかどうかを判断する
 * @return 状態が変化した場合はtrue
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
 * @brief 現在の状態に基づいて目を更新する
 */
void EyesAnimation::updateEyesBasedOnState() {
  if (state == EyeState::DIZZY) {
    drawDizzyEyes();
  } else {
    handleNormalState();
  }
}

/**
 * @brief 通常状態の目の処理
 */
void EyesAnimation::handleNormalState() {
  static TouchState lastTouchState = TouchState::NONE;
  static uint32_t lastBlinkTime = 0;
  uint32_t currentTime = millis();
  
  // タッチ入力の処理
  TouchState touchState = touchHandler.update();
  
  // タッチ状態が変化した場合のみ処理
  if (touchState != lastTouchState) {
    lastTouchState = touchState;
    
    if (touchState == TouchState::RELEASED) {
      resetEyes();
      return;
    }
  }
  
  // まばたきの状態を取得
  BlinkState currentBlinkState = determineBlinkState();
  
  // まばたき中でない場合のみ視線処理
  if (currentBlinkState == BlinkState::OPEN) {
    // タッチ中は視線追従
    if (touchState == TouchState::TOUCHING) {
      drawGazingEyes(touchHandler.getTouchPoint());
    } else {
      // タッチなしの場合は中央視線
      drawCenterEyes();
    }
  }
  
  // まばたきの更新（50ms間隔）
  if (currentTime - lastBlinkTime >= 50) {
    lastBlinkTime = currentTime;
    drawBlink();
  }
}

/**
 * @brief 両目をディスプレイに描画する
 */
void EyesAnimation::renderEyes() {
  leftEye.render(&M5.Display);
  rightEye.render(&M5.Display);
}

/**
 * @brief タッチハンドラーを取得する
 * @return タッチハンドラーへの参照
 */
TouchHandler& EyesAnimation::getTouchHandler() {
  return touchHandler;
}

/**
 * @brief 目をリセットする
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
 * @brief 中央に瞳孔を描画する
 */
void EyesAnimation::drawCenterEyes() {
  Point saccades = generateSaccades();
  leftEye.drawCenterPupil(saccades);
  rightEye.drawCenterPupil(saccades);
}

/**
 * @brief 視線追従の瞳孔を描画する
 * @param touchPoint タッチされた座標
 */
void EyesAnimation::drawGazingEyes(const Point& touchPoint) {
  Point saccades = generateSaccades();
  leftEye.drawGazingPupil(touchPoint, saccades);
  rightEye.drawGazingPupil(touchPoint, saccades);
}

/**
 * @brief めまい効果の瞳孔を描画する
 */
void EyesAnimation::drawDizzyEyes() {
  // 左目と右目で異なる角度オフセットを使用
  leftEye.drawDizzyPupil(degree);
  rightEye.drawDizzyPupil(degree, 180.0F);
  
  // めまい効果の進行と終了判定
  degree += DIZZY_ROTATION_SPEED;
  if (degree >= DIZZY_TOTAL_DEGREES) {
    degree = 0.0F;
    state = EyeState::NORMAL;
  }
}

/**
 * @brief まばたきを描画する
 */
void EyesAnimation::drawBlink() {
  BlinkState blinkState = determineBlinkState();
  
  leftEye.drawBlink(blinkState);
  rightEye.drawBlink(blinkState);
  
  updateBlinkCounter();
}

/**
 * @brief 現在のまばたき状態を決定する
 * @return 現在のまばたき状態
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
 * @brief まばたきカウンターを更新する
 */
void EyesAnimation::updateBlinkCounter() {
  blinkCounter++;
  if (blinkCounter > blinkMaxCount) {
    blinkCounter = 0;
    blinkMaxCount = random(BLINK_RANDOM_MIN, BLINK_RANDOM_MAX);
  }
}

/**
 * @brief サッケード（微小な目の動き）を生成する
 * @return 微小な動きの量
 */
Point EyesAnimation::generateSaccades() {
  static uint32_t lastSaccadeTime = 0;
  static Point lastSaccade(0, 0);
  uint32_t currentTime = millis();
  
  // サッケードの更新頻度を制限（50ms間隔）
  if (currentTime - lastSaccadeTime < 50) {
    return lastSaccade;
  }
  
  lastSaccadeTime = currentTime;
  lastSaccade.x = random(SACCADES_MAX) / SACCADES_DIVISOR;
  lastSaccade.y = random(SACCADES_MAX) / SACCADES_DIVISOR;
  
  return lastSaccade;
}
