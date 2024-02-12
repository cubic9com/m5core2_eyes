#include <M5Unified.h>

const uint8_t eye_radius = 65;
const uint8_t max_dist = 40;
const uint8_t pupil_radius = 13;
const float acc_threshould = 1.4F;
const uint8_t num_of_rotation = 3;

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

float degree = 0.0F; 
bool is_dizzy = false;

void init_eyes() {
  disp.fillCircle(old_pupil[0].x, old_pupil[0].y, pupil_radius, TFT_WHITE);
  disp.fillCircle(old_pupil[1].x, old_pupil[1].y, pupil_radius, TFT_WHITE);

  old_pupil[0].x = eye_base[0].x;
  old_pupil[0].y = eye_base[0].y;
  old_pupil[1].x = eye_base[1].x;
  old_pupil[1].y = eye_base[1].y;
}

void draw_center_pupils() {
  point_t new_pupil[2];
  point_t delta;

  delta.x = random(2);
  delta.y = random(2);

  new_pupil[0].x = eye_base[0].x + delta.x;
  new_pupil[0].y = eye_base[0].y + delta.y;
  new_pupil[1].x = eye_base[1].x + delta.x;
  new_pupil[1].y = eye_base[1].y + delta.y;

  disp.fillCircle(old_pupil[0].x, old_pupil[0].y, pupil_radius, TFT_WHITE);
  disp.fillCircle(new_pupil[0].x, new_pupil[0].y, pupil_radius, TFT_BLACK);
  disp.fillCircle(old_pupil[1].x, old_pupil[1].y, pupil_radius, TFT_WHITE);
  disp.fillCircle(new_pupil[1].x, new_pupil[1].y, pupil_radius, TFT_BLACK);

  old_pupil[0].x = new_pupil[0].x;
  old_pupil[0].y = new_pupil[0].y;
  old_pupil[1].x = new_pupil[1].x;
  old_pupil[1].y = new_pupil[1].y;
}

point_t compute_gazing_pupil(point_t touch_point, point_t base, point_t delta) {
  point_t diff;
  double angle;
  double dist;
  point_t ret;

  diff.x = touch_point.x - base.x;
  diff.y = touch_point.y - base.y;

  angle = atan2(diff.y, diff.x);
  dist = hypot(diff.x, diff.y);

  if (dist > max_dist) {
    ret.x = max_dist * cos(angle) + base.x + delta.x;
    ret.y = max_dist * sin(angle) + base.y + delta.y;
  } else {
    ret.x = diff.x + base.x + delta.x;
    ret.y = diff.y + base.y + delta.y;
  }

  return ret;
}

void draw_gazing_pupils(point_t touch_point) {
  point_t new_pupil[2];
  point_t delta;

  // for saccades
  delta.x = random(2);
  delta.y = random(2);

  new_pupil[0] = compute_gazing_pupil(touch_point, eye_base[0], delta);

  disp.fillCircle(old_pupil[0].x, old_pupil[0].y, pupil_radius, TFT_WHITE);
  disp.fillCircle(new_pupil[0].x, new_pupil[0].y, pupil_radius, TFT_BLACK);

  old_pupil[0].x = new_pupil[0].x;
  old_pupil[0].y = new_pupil[0].y;

  new_pupil[1] = compute_gazing_pupil(touch_point, eye_base[1], delta);

  disp.fillCircle(old_pupil[1].x, old_pupil[1].y, pupil_radius, TFT_WHITE);
  disp.fillCircle(new_pupil[1].x, new_pupil[1].y, pupil_radius, TFT_BLACK);

  old_pupil[1].x = new_pupil[1].x;
  old_pupil[1].y = new_pupil[1].y;
}

void draw_dizzy_pupils() {
  point_t new_pupil[2];

  new_pupil[0].x = (max_dist - max_dist * (degree / 1080.0F)) * cos(degree * DEG_TO_RAD) + eye_base[0].x;
  new_pupil[0].y = (max_dist - max_dist * (degree / 1080.0F)) * sin(degree * DEG_TO_RAD) + eye_base[0].y;

  disp.fillCircle(old_pupil[0].x, old_pupil[0].y, pupil_radius, TFT_WHITE);
  disp.fillCircle(new_pupil[0].x, new_pupil[0].y, pupil_radius, TFT_BLACK);

  old_pupil[0].x = new_pupil[0].x;
  old_pupil[0].y = new_pupil[0].y;

  new_pupil[1].x = (max_dist - max_dist * (degree / 1080.0F)) * cos((degree + 180.0F) * DEG_TO_RAD) + eye_base[1].x;
  new_pupil[1].y = (max_dist - max_dist * (degree / 1080.0F)) * sin((degree + 180.0F) * DEG_TO_RAD) + eye_base[1].y;

  disp.fillCircle(old_pupil[1].x, old_pupil[1].y, pupil_radius, TFT_WHITE);
  disp.fillCircle(new_pupil[1].x, new_pupil[1].y, pupil_radius, TFT_BLACK);

  old_pupil[1].x = new_pupil[1].x;
  old_pupil[1].y = new_pupil[1].y;
}

void setup() {
  auto cfg = M5.config();
  cfg.internal_imu = true;
  M5.begin(cfg);

  M5.Display.init();

  M5.Display.setRotation(1);
  M5.Display.setBrightness(128);

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

void loop() {
  float ax, ay, az;
  M5.Imu.getAccel(&ax, &ay, &az);

  float total_acc = sqrt(ax * ax + ay * ay + az * az);
  if (total_acc > acc_threshould) {
    is_dizzy = true;
    degree = 0.0F;
  }
  if (is_dizzy) {
    draw_dizzy_pupils();
    degree += 15.0F;
    if (degree >= (360.0F * num_of_rotation)) {
      degree = 0.0F;
      is_dizzy = false;
    }
  }

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
      draw_gazing_pupils(touch_point);
      break;
    case 2:
    case 6:
    case 10:
    case 14:
      init_eyes();
      break;
    default:
      if (!is_dizzy) {
        draw_center_pupils();
      }
      break;
  }

  disp.pushSprite(&M5.Display, 0, 0);
}
