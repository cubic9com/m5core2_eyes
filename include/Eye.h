#pragma once

#include <M5Unified.h>
#include "TouchHandler.h"

/**
 * @brief 目の状態を表す列挙型
 */
enum class EyeState {
  NORMAL,    // 通常状態
  GAZING,    // 視線追従状態
  DIZZY      // めまい状態
};

/**
 * @brief まばたきの状態を表す列挙型
 */
enum class BlinkState {
  OPEN,           // 開いている
  HALF_CLOSED,    // 半分閉じている
  CLOSED          // 完全に閉じている
};


/**
 * @brief 単一の目を管理するクラス
 */
class Eye {
public:
  // 目の設定
  static constexpr uint8_t EYE_RADIUS = 65;
  static constexpr uint8_t PUPIL_RADIUS = 13;
  static constexpr uint8_t MAX_PUPIL_DISTANCE = 40;
  
  // スプライトの設定
  static constexpr uint8_t SPRITE_WIDTH = 160;
  static constexpr uint8_t SPRITE_HEIGHT = 131;
  
  // 目の位置
  static constexpr uint8_t EYE_BASE_Y = 120;
  static constexpr uint8_t EYE_LEFT_X = 80;
  static constexpr uint8_t EYE_RIGHT_X = 240;
  static constexpr uint8_t DISPLAY_BASE_Y = 55;
  
public:
  /**
   * @brief コンストラクタ
   * @param baseX 目の中心X座標
   * @param baseY 目の中心Y座標
   * @param displayX ディスプレイ上のX座標
   * @param displayY ディスプレイ上のY座標
   */
  Eye(int16_t baseX, int16_t baseY, int16_t displayX, int16_t displayY);
  
  /**
   * @brief 目をクリアする
   */
  void clear();
  
  /**
   * @brief 白目を描画する
   */
  void drawWhite();
  
  /**
   * @brief 瞳孔を初期位置に戻す
   */
  void resetPupil();
  
  /**
   * @brief 瞳孔を中央に描画する（微小なランダム動きあり）
   * @param saccades 微小な動きの量
   */
  void drawCenterPupil(const Point& saccades);
  
  /**
   * @brief 視線追従の瞳孔を描画する
   * @param targetPoint 視線の目標点
   * @param saccades 微小な動きの量
   */
  void drawGazingPupil(const Point& targetPoint, const Point& saccades);
  
  /**
   * @brief めまい効果の瞳孔を描画する
   * @param degree 回転角度
   * @param offsetDegree 角度オフセット
   */
  void drawDizzyPupil(float degree, float offsetDegree = 0.0F);
  
  /**
   * @brief まばたきを描画する
   * @param state まばたきの状態
   */
  void drawBlink(BlinkState state);
  
  /**
   * @brief スプライトをディスプレイに表示する
   * @param display ディスプレイオブジェクト
   */
  void render(M5GFX* display);
  
private:
  Point basePoint;       // 目の中心座標
  Point displayOffset;   // ディスプレイ上のオフセット
  Point pupilPosition;   // 瞳孔の現在位置
  BlinkState lastBlinkState; // 前回のまばたき状態
  M5Canvas canvas;       // 描画用キャンバス
  
  /**
   * @brief 瞳孔を消去する
   */
  void erasePupil();
  
  /**
   * @brief 瞳孔を描画する
   */
  void drawPupil();
  
  /**
   * @brief 瞳孔を更新する
   * @param newPosition 新しい瞳孔の位置
   */
  void updatePupil(const Point& newPosition);
  
  /**
   * @brief 視線追従の瞳孔位置を計算する
   * @param targetPoint 視線の目標点
   * @param saccades 微小な動きの量
   * @return 新しい瞳孔の位置
   */
  Point computeGazingPosition(const Point& targetPoint, const Point& saccades);
  
  /**
   * @brief グローバル座標をスプライト内のローカル座標に変換する
   * @param globalPoint グローバル座標
   * @return スプライト内のローカル座標
   */
  Point toLocalCoordinates(const Point& globalPoint) const;
  
  /**
   * @brief 指定した色で瞳孔を描画する
   * @param color 描画色
   */
  void drawPupilWithColor(uint16_t color);
};
