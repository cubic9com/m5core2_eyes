#include <M5Core2.h>

#define EYE_RADIUS 65
#define EYE0_BASE_X 80
#define EYE0_BASE_Y 120
#define EYE1_BASE_X 240
#define EYE1_BASE_Y 120
#define MAX_DIST 40
#define PUPIL_RADIUS 13

TFT_eSprite disp(&M5.Lcd);
Point old_pupil[2];

void setup() {
  M5.begin();

  disp.createSprite(320, 240);

  disp.fillScreen(TFT_BLACK);
  disp.fillCircle(EYE0_BASE_X, EYE0_BASE_Y, EYE_RADIUS, TFT_WHITE);
  disp.fillCircle(EYE1_BASE_X, EYE1_BASE_Y, EYE_RADIUS, TFT_WHITE);

  old_pupil[0].x = EYE0_BASE_X;
  old_pupil[0].y = EYE0_BASE_Y;
  old_pupil[1].x = EYE1_BASE_X;
  old_pupil[1].y = EYE1_BASE_Y;
  init_eyes();
}

void init_eyes() {
  disp.fillCircle(old_pupil[0].x, old_pupil[0].y, PUPIL_RADIUS, TFT_WHITE);
  disp.fillCircle(old_pupil[1].x, old_pupil[1].y, PUPIL_RADIUS, TFT_WHITE);
  disp.fillCircle(EYE0_BASE_X, EYE0_BASE_Y, PUPIL_RADIUS, TFT_BLACK);
  disp.fillCircle(EYE1_BASE_X, EYE1_BASE_Y, PUPIL_RADIUS, TFT_BLACK);

  old_pupil[0].x = EYE0_BASE_X;
  old_pupil[0].y = EYE0_BASE_Y;
  old_pupil[1].x = EYE1_BASE_X;
  old_pupil[1].y = EYE1_BASE_Y;
}

Point computePupil(Point tap, Point base) {
  Point diff;
  double angle;
  double dist;
  Point ret;

  diff.x = tap.x - base.x;
  diff.y = tap.y - base.y;

  angle = atan2(diff.y, diff.x);
  dist = hypot(diff.x, diff.y);

  if (dist > MAX_DIST) {
    ret.x = MAX_DIST * cos(angle) + base.x;
    ret.y = MAX_DIST * sin(angle) + base.y;
  } else {
    ret.x = diff.x + base.x;
    ret.y = diff.y + base.y;
  }

  return ret;
}

void drawPupils(Point tap) {
  Point new_pupil[2];
  Point base[2];

  base[0].x = EYE0_BASE_X;
  base[0].y = EYE0_BASE_Y;
  new_pupil[0] = computePupil(tap, base[0]);

  disp.fillCircle(old_pupil[0].x, old_pupil[0].y, PUPIL_RADIUS, TFT_WHITE);
  disp.fillCircle(new_pupil[0].x, new_pupil[0].y, PUPIL_RADIUS, TFT_BLACK);

  old_pupil[0].x = new_pupil[0].x;
  old_pupil[0].y = new_pupil[0].y;

  base[1].x = EYE1_BASE_X;
  base[1].y = EYE1_BASE_Y;
  new_pupil[1] = computePupil(tap, base[1]);

  disp.fillCircle(old_pupil[1].x, old_pupil[1].y, PUPIL_RADIUS, TFT_WHITE);
  disp.fillCircle(new_pupil[1].x, new_pupil[1].y, PUPIL_RADIUS, TFT_BLACK);

  old_pupil[1].x = new_pupil[1].x;
  old_pupil[1].y = new_pupil[1].y;
}

void loop() {
  M5.update();

  Event& e = M5.Buttons.event;
  if (e & (E_TOUCH | E_MOVE)) {
    drawPupils(e.to);
  } else if (e & E_RELEASE) {
    init_eyes();
  }

  disp.pushSprite(0, 0);
}
