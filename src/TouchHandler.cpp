#include "TouchHandler.h"

/**
 * @brief コンストラクタ
 */
TouchHandler::TouchHandler() : lastTouchState(TouchState::NONE), touchPoint(0, 0) {
}

/**
 * @brief タッチ状態を更新する
 * @return 現在のタッチ状態
 */
TouchState TouchHandler::update() {
  static uint32_t lastUpdateTime = 0;
  uint32_t currentTime = millis();
  
  // タッチ更新の頻度を制限（25ms間隔）
  if (currentTime - lastUpdateTime < 25) {
    return lastTouchState;
  }
  
  lastUpdateTime = currentTime;
  M5.update();
  auto touch = M5.Touch.getDetail();
  
  TouchState currentState = interpretTouchState(touch.state);
  
  // タッチ位置の更新（タッチ中の場合のみ）
  if (currentState == TouchState::TOUCHING) {
    touchPoint.x = touch.x;
    touchPoint.y = touch.y;
  }
  
  // 前回の状態を保存
  lastTouchState = currentState;
  
  return currentState;
}

/**
 * @brief 現在のタッチ位置を取得する
 * @return タッチ位置
 */
const Point& TouchHandler::getTouchPoint() const {
  return touchPoint;
}

/**
 * @brief 前回のタッチ状態を取得する
 * @return 前回のタッチ状態
 */
TouchState TouchHandler::getLastTouchState() const {
  return lastTouchState;
}

/**
 * @brief M5Stackのタッチ状態を解釈する
 * @param state M5Stackのタッチ状態
 * @return 解釈されたタッチ状態
 */
TouchState TouchHandler::interpretTouchState(uint8_t state) {
  // タッチ状態の判定
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
