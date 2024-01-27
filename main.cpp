#include <M5Unified.h>

const uint8_t eye_radius = 65;
const uint8_t max_dist = 40;
const uint8_t pupil_radius = 13;

struct point_t {
  int16_t x;
  int16_t y;
};

const point_t eye_base[2] = {
  {80, 120},
  {240, 120}
};

M5Canvas disp;
point_t old_pupil[2];

void init_eyes() {
  disp.fillCircle(old_pupil[0].x, old_pupil[0].y, pupil_radius, TFT_WHITE);
  disp.fillCircle(old_pupil[1].x, old_pupil[1].y, pupil_radius, TFT_WHITE);
  disp.fillCircle(eye_base[0].x, eye_base[0].y, pupil_radius, TFT_BLACK);
  disp.fillCircle(eye_base[1].x, eye_base[1].y, pupil_radius, TFT_BLACK);

  old_pupil[0].x = eye_base[0].x;
  old_pupil[0].y = eye_base[0].y;
  old_pupil[1].x = eye_base[1].x;
  old_pupil[1].y = eye_base[1].y;
}

void setup() {
  M5.begin();

  M5.Display.init();

  M5.Display.setRotation(1);
  M5.Display.setBrightness(128);

  Serial.begin(115200);

  disp.setPsram(true);
  disp.createSprite(320, 240);

  disp.fillScreen(TFT_BLACK);
  disp.fillCircle(eye_base[0].x, eye_base[0].y, eye_radius, TFT_WHITE);
  disp.fillCircle(eye_base[1].x, eye_base[1].y, eye_radius, TFT_WHITE);

  old_pupil[0].x = eye_base[0].x;
  old_pupil[0].y = eye_base[0].y;
  old_pupil[1].x = eye_base[1].x;
  old_pupil[1].y = eye_base[1].y;
  init_eyes();
}

point_t computePupil(point_t touch_point, point_t base) {
  point_t diff;
  double angle;
  double dist;
  point_t ret;

  diff.x = touch_point.x - base.x;
  diff.y = touch_point.y - base.y;

  angle = atan2(diff.y, diff.x);
  dist = hypot(diff.x, diff.y);

  if (dist > max_dist) {
    ret.x = max_dist * cos(angle) + base.x;
    ret.y = max_dist * sin(angle) + base.y;
  } else {
    ret.x = diff.x + base.x;
    ret.y = diff.y + base.y;
  }

  return ret;
}

void drawPupils(point_t touch_point) {
  point_t new_pupil[2];
  point_t base[2];

  base[0].x = eye_base[0].x;
  base[0].y = eye_base[0].y;
  new_pupil[0] = computePupil(touch_point, base[0]);

  disp.fillCircle(old_pupil[0].x, old_pupil[0].y, pupil_radius, TFT_WHITE);
  disp.fillCircle(new_pupil[0].x, new_pupil[0].y, pupil_radius, TFT_BLACK);

  old_pupil[0].x = new_pupil[0].x;
  old_pupil[0].y = new_pupil[0].y;

  base[1].x = eye_base[1].x;
  base[1].y = eye_base[1].y;
  new_pupil[1] = computePupil(touch_point, base[1]);

  disp.fillCircle(old_pupil[1].x, old_pupil[1].y, pupil_radius, TFT_WHITE);
  disp.fillCircle(new_pupil[1].x, new_pupil[1].y, pupil_radius, TFT_BLACK);

  old_pupil[1].x = new_pupil[1].x;
  old_pupil[1].y = new_pupil[1].y;
}

void loop() {
  M5.update();

  auto t = M5.Touch.getDetail();

  switch (t.state) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 9:
    case 11:
    case 13:
    case 15:
      point_t touch_point;
      touch_point.x = t.x;
      touch_point.y = t.y;
      drawPupils(touch_point);
      break;
    case 2:
    case 6:
    case 10:
    case 14:
      init_eyes();
      break;
    default:
      break;
  }

  disp.pushSprite(&M5.Display, 0, 0);
}
