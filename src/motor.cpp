#include "motor.h"
#include <esp32-hal-ledc.h>
#include <esp32-hal.h>
#include <esp32-hal-log.h>

bool _ready = false;

motor_callback _callback;

Movement _m;
Movement _m_previous;

unsigned long lastLoop = 0;
const unsigned int smoothFactor = 2200;
const unsigned int pwm_cutoff = 80;
const float sideMovementMultiplier = 0.5f;
double power = 1;
const float balance = 0.1f;

double clamp(double d, double min, double max) {
  const double t = d < min ? min : d;
  return t > max ? max : t;
}

float sig(float number) {
    return number >= 0 ? 1 : -1;
}

void motor_setup(motor_callback callback) {
    _callback = callback;

    ledcSetup(MOTOR_CHAN_LEFT_FRONT, MOTOR_PWM_FREQ, MOTOR_PWM_RESOLUTION);
    ledcAttachPin(MOTOR_PIN_LEFT_FRONT, MOTOR_CHAN_LEFT_FRONT);
    ledcSetup(MOTOR_CHAN_LEFT_BACK, MOTOR_PWM_FREQ, MOTOR_PWM_RESOLUTION);
    ledcAttachPin(MOTOR_PIN_LEFT_BACK, MOTOR_CHAN_LEFT_BACK);
    ledcSetup(MOTOR_CHAN_RIGHT_FRONT, MOTOR_PWM_FREQ, MOTOR_PWM_RESOLUTION);
    ledcAttachPin(MOTOR_PIN_RIGHT_FRONT, MOTOR_CHAN_RIGHT_FRONT);
    ledcSetup(MOTOR_CHAN_RIGHT_BACK, MOTOR_PWM_FREQ, MOTOR_PWM_RESOLUTION);
    ledcAttachPin(MOTOR_PIN_RIGHT_BACK, MOTOR_CHAN_RIGHT_BACK);
    _ready = true;
}

void motor_loop() {
    if (!_ready) {
        return;
    }
    if (lastLoop == 0) {
        lastLoop = millis();
    }

    int left = clamp(_m.front + _m.side, -1, 1) * 255;
    int right = clamp(_m.front - _m.side, -1, 1) * 255;

    left = abs(left) < pwm_cutoff? 0 : left;
    right = abs(right) < pwm_cutoff? 0 : right;

    ledcWrite(MOTOR_CHAN_LEFT_FRONT, left >= 0? left : 0);
    ledcWrite(MOTOR_CHAN_LEFT_BACK, left < 0? -left+1 : 0);

    ledcWrite(MOTOR_CHAN_RIGHT_FRONT, right >= 0? right : 0);
    ledcWrite(MOTOR_CHAN_RIGHT_BACK, right < 0? -right+1 : 0);

    long delta = millis() - lastLoop; // This can be adjusted
    _m.front = (_m.front * smoothFactor - delta * sig(_m.front)) / smoothFactor;
    _m.side = (_m.side * smoothFactor - delta * sig(_m.side)) / smoothFactor;

    _callback(_m);

    power += (1 - power) * delta / 1000;
    lastLoop = millis();
}

void motor_add_movement(Movement direction) {
    _m.front += direction.front * power;
    _m.side += direction.side * power * sideMovementMultiplier;

    power *= power/2 + (1 - power) * balance;

    _m.front = clamp(_m.front, -1, 1);
    _m.side = clamp(_m.side, -1, 1);

    log_d("Movement %.4f, %.4f", _m.front, _m.side);
}