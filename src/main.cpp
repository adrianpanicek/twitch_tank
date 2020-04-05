#include <WiFi.h>
#include <EEPROM.h>
#include "wireless.h"
#include "irc.h"
#include "motor.h"
#include "display.h"

//needed for library
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <IRCClient.h>

void messageParser(IRCCallbackMessage message);

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    EEPROM.end();
    EEPROM.begin(512);
    
    display_setup();

    wifi_setup();
    delay(2000);
    irc_setup(messageParser, [] () {
        return EEPROM.readString(EEPROM_CONFIG_ADDR);
    });
    motor_setup([] (Movement movement) {
        set_movement(movement.front, movement.side);
    });
}

// LOOP 

void loop() {
    irc_loop();
    motor_loop();
    display_loop();
}

Movement parseDirection(String* text) {
    Movement direction;

    if (text->indexOf("front") >= 0) {
        direction.front = -1;
        return direction;
    }

    if (text->indexOf("back") >= 0) {
        direction.front = 1;
        return direction;
    }

    if (text->indexOf("left") >= 0) {
        direction.side = -1;
        return direction;
    }

    if (text->indexOf("right") >= 0) {
        direction.side = 1;
        return direction;
    }

    return direction;
}

void messageParser(IRCCallbackMessage message) { // Todo reimplement
    String* text = &message.text;
    Movement direction = parseDirection(text);
    motor_add_movement(direction);

    set_message(message.nick + ": " + message.text);
}