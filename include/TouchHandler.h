#pragma once

#include <M5Unified.h>

/**
 * @brief 座標を表す構造体
 */
struct Point {
  int16_t x;
  int16_t y;
  
  Point() : x(0), y(0) {}
  Point(int16_t _x, int16_t _y) : x(_x), y(_y) {}
  
  Point operator+(const Point& other) const {
    return Point(x + other.x, y + other.y);
  }
  
  Point operator-(const Point& other) const {
    return Point(x - other.x, y - other.y);
  }
};

/**
 * @brief タッチ状態を表す列挙型
 */
enum class TouchState {
  NONE,       // タッチなし
  TOUCHING,   // タッチ中
  RELEASED,   // タッチ終了
  MULTI_TOUCH // 複数タッチ
};

/**
 * @brief タッチ入力を管理するクラス
 */
class TouchHandler {
public:
  /**
   * @brief コンストラクタ
   */
  TouchHandler();
  
  /**
   * @brief タッチ状態を更新する
   * @return 現在のタッチ状態
   */
  TouchState update();
  
  /**
   * @brief 現在のタッチ位置を取得する
   * @return タッチ位置
   */
  const Point& getTouchPoint() const;
  
  /**
   * @brief 前回のタッチ状態を取得する
   * @return 前回のタッチ状態
   */
  TouchState getLastTouchState() const;
  
private:
  TouchState lastTouchState;  // 前回のタッチ状態
  Point touchPoint;           // タッチ位置
  
  /**
   * @brief M5Stackのタッチ状態を解釈する
   * @param state M5Stackのタッチ状態
   * @return 解釈されたタッチ状態
   */
  TouchState interpretTouchState(uint8_t state);
};
