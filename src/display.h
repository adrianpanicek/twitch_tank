#include <Arduino.h>
#include <U8g2lib.h>
#include <WString.h>

void display_setup();

void display_loop();

void set_message(String message);
void set_movement(float front, float side);