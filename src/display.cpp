#include <Arduino.h>
#include <U8g2lib.h>
#include <WString.h>
#include "display.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

const unsigned int height = 64;
const unsigned int width = 128;
const size_t messages_length = 5;

String _message = "";
bool message_updated = false;

unsigned int frames = 0;
unsigned long lastDraw = 0;

float sign(float number) {
    return number >= 0 ? 1 : -1;
}

void draw_compass(float front, float side, const unsigned int compass_y) {
    const unsigned int compass_center_x = width/2 - 1;
    const unsigned int line_width = 7;
    const unsigned int line_offset = 3;
    unsigned int line = 0;

    // Horizontal and vertical clear
    u8g2.setDrawColor(0);
    u8g2.drawBox(0, 0, width - 1, height - 8);
    u8g2.setDrawColor(1);
    // Horizontal Draw
    for (
        unsigned int y = compass_y;
        y < compass_y + line_width;
        y++
    ) {
        const int diff = line++ - line_offset;
        const int offset = sign(side) * (-abs(diff) + line_offset * 2);
        u8g2.drawLine(
            compass_center_x + offset,
            y,
            compass_center_x + offset + side * (compass_center_x - line_width),
            y
        );
    }
    line = 0;

    // Vertical Draw
    for (
        unsigned int x = compass_center_x - line_width / 2;
        x < compass_center_x + line_width / 2 + 1;
        x++
    ) {
        const int diff = line++ - line_offset;
        const int offset = sign(front) * (-abs(diff) + line_offset * 2) + line_offset;
        u8g2.drawLine(
            x,
            compass_y + offset,
            x,
            compass_y + offset + front * (compass_y - line_width)
        );
    }

    u8g2.updateDisplayArea( // Update Horizontal
        0, 
        u8g2.getBufferTileHeight()/2-1,
        u8g2.getBufferTileWidth(),
        1
    );

    u8g2.updateDisplayArea( // Update Vertical
        u8g2.getBufferTileWidth()/2-2,
        0,
        3,
        u8g2.getBufferTileHeight() - 1
    );
}

void display_setup() {
    u8g2.begin();
    u8g2.setBusClock(400000);
    u8g2.clearBuffer();
    u8g2.sendBuffer();
    u8g2.clearDisplay();

    lastDraw = millis();
}


void display_loop() {
    const int draw_ms = millis() - lastDraw;
    lastDraw = millis();

    if (frames > 1000) {
        u8g2.clearDisplay();
        frames = 0;
    }
    
    u8g2.setDrawColor(0);
    u8g2.drawBox(0, height - 8, width -1, 8);
    u8g2.setDrawColor(1);
    u8g2.drawLine(0, height - 1, draw_ms, height - 1);
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.drawUTF8(0, height - 2, _message.c_str());

    u8g2.updateDisplayArea( // Update draw_ms line
        0, 
        u8g2.getBufferTileHeight() - 1,
        u8g2.getBufferTileWidth(),
        1
    );
};

void set_message(String message) {
    _message = message;
}

void set_movement(float front, float side) {
    draw_compass(front, side, height/2 - 8);
}