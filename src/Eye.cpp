#include "Eye.h"
#include "EyesAnimation.h"

/**
 * @brief コンストラクタ
 * @param baseX 目の中心X座標
 * @param baseY 目の中心Y座標
 * @param displayX ディスプレイ上のX座標
 * @param displayY ディスプレイ上のY座標
 */
Eye::Eye(int16_t baseX, int16_t baseY, int16_t displayX, int16_t displayY) 
  : basePoint(baseX, baseY),
    displayOffset(displayX, displayY),
    pupilPosition(baseX, baseY),
    lastBlinkState(BlinkState::OPEN)
{
  // 1ビット色深度でスプライトを作成
  canvas.setColorDepth(1);
  canvas.createSprite(SPRITE_WIDTH, SPRITE_HEIGHT);
  clear();
}

/**
 * @brief 目をクリアする
 */
void Eye::clear() {
  canvas.fillScreen(TFT_BLACK);
}

/**
 * @brief 白目を描画する
 */
void Eye::drawWhite() {
  Point localCenter = toLocalCoordinates(basePoint);
  canvas.fillCircle(localCenter.x, localCenter.y, EYE_RADIUS, TFT_WHITE);
}

/**
 * @brief 瞳孔を初期位置に戻す
 */
void Eye::resetPupil() {
  erasePupil();
  pupilPosition = basePoint;
  drawPupil();
}

/**
 * @brief 瞳孔を中央に描画する（微小なランダム動きあり）
 * @param saccades 微小な動きの量
 */
void Eye::drawCenterPupil(const Point& saccades) {
  Point newPosition = basePoint + saccades;
  updatePupil(newPosition);
}

/**
 * @brief 視線追従の瞳孔を描画する
 * @param targetPoint 視線の目標点
 * @param saccades 微小な動きの量
 */
void Eye::drawGazingPupil(const Point& targetPoint, const Point& saccades) {
  Point newPosition = computeGazingPosition(targetPoint, saccades);
  updatePupil(newPosition);
}

/**
 * @brief めまい効果の瞳孔を描画する
 * @param degree 回転角度
 * @param offsetDegree 角度オフセット（デフォルト値: 0.0F）
 */
void Eye::drawDizzyPupil(float degree, float offsetDegree) {
  // 距離係数の計算（角度が大きくなるほど中心に近づく）
  float distanceFactor = 1.0F - (degree / EyesAnimation::DIZZY_DISTANCE_FACTOR);
  float distance = MAX_PUPIL_DISTANCE * distanceFactor;
  
  // 角度から位置を計算（DEG_TO_RAD = M_PI / 180.0F）
  float angleRad = (degree + offsetDegree) * DEG_TO_RAD;
  Point newPosition(
    static_cast<int16_t>(distance * cos(angleRad)) + basePoint.x,
    static_cast<int16_t>(distance * sin(angleRad)) + basePoint.y
  );
  
  updatePupil(newPosition);
}

/**
 * @brief まばたきを描画する
 * @param state まばたきの状態
 */
void Eye::drawBlink(BlinkState state) {
  // 前回と同じ状態なら何もしない
  if (state == lastBlinkState) {
    return;
  }
  
  // ローカル座標系でのディスプレイベースY座標
  int16_t localBaseY = DISPLAY_BASE_Y - displayOffset.y;
  
  switch (state) {
    case BlinkState::HALF_CLOSED:
      // 半分閉じた状態 - 上部と下部に黒い矩形を描画
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
      // 完全に閉じた状態 - 全体を黒で塗りつぶす（高速化のためfillScreenを使用）
      canvas.fillScreen(TFT_BLACK);
      break;
      
    case BlinkState::OPEN:
      // 開いた状態は白目を再描画
      drawWhite();
      drawPupil();
      break;
      
    default:
      // 未知の状態の場合は何もしない
      break;
  }
  
  // 状態を更新
  lastBlinkState = state;
}

/**
 * @brief スプライトをディスプレイに表示する
 * @param display ディスプレイオブジェクト
 */
void Eye::render(M5GFX* display) {
  canvas.pushSprite(display, displayOffset.x, displayOffset.y);
}

/**
 * @brief 瞳孔を消去する
 */
void Eye::erasePupil() {
  drawPupilWithColor(TFT_WHITE);
}

/**
 * @brief 瞳孔を描画する
 */
void Eye::drawPupil() {
  drawPupilWithColor(TFT_BLACK);
}

/**
 * @brief 指定した色で瞳孔を描画する
 * @param color 描画色
 */
void Eye::drawPupilWithColor(uint16_t color) {
  Point localPupil = toLocalCoordinates(pupilPosition);
  canvas.fillCircle(localPupil.x, localPupil.y, PUPIL_RADIUS, color);
}

/**
 * @brief 瞳孔を更新する
 * @param newPosition 新しい瞳孔の位置
 */
void Eye::updatePupil(const Point& newPosition) {
  // 位置が変わっていない場合は何もしない
  if (pupilPosition.x == newPosition.x && pupilPosition.y == newPosition.y) {
    return;
  }
  
  erasePupil();
  pupilPosition = newPosition;
  drawPupil();
}

/**
 * @brief グローバル座標をスプライト内のローカル座標に変換する
 * @param globalPoint グローバル座標
 * @return スプライト内のローカル座標
 */
Point Eye::toLocalCoordinates(const Point& globalPoint) const {
  return Point(
    globalPoint.x - displayOffset.x,
    globalPoint.y - displayOffset.y
  );
}

/**
 * @brief 視線追従の瞳孔位置を計算する
 * @param targetPoint 視線の目標点
 * @param saccades 微小な動きの量
 * @return 新しい瞳孔の位置
 */
Point Eye::computeGazingPosition(const Point& targetPoint, const Point& saccades) {
  // 目の中心から目標点への差分ベクトル
  Point diff = targetPoint - basePoint;
  
  // 角度と距離を計算
  double angle = atan2(diff.y, diff.x);
  double dist = std::hypot(diff.x, diff.y);
  
  Point result;
  if (dist > MAX_PUPIL_DISTANCE) {
    // 最大距離を超える場合は、最大距離で制限
    result.x = static_cast<int16_t>(MAX_PUPIL_DISTANCE * cos(angle)) + basePoint.x;
    result.y = static_cast<int16_t>(MAX_PUPIL_DISTANCE * sin(angle)) + basePoint.y;
  } else {
    // 最大距離内の場合は、そのまま使用
    result = basePoint + diff;
  }
  
  // 微小な動きを追加
  result = result + saccades;
  
  return result;
}
