#include "irc.h"
#include <IRCClient.h>
#include <WiFi.h> 
#include <WifiClient.h>
#include <WString.h>

WiFiClient* wiFiClient;
IRCClient* irc;

irc_config_callback get_config;
irc_callback message_callback;

unsigned int retries = 0;
const unsigned int retries_treshold = 5;

void irc_setup(irc_callback callback, irc_config_callback config_callback) {
    Serial.println("Setting up IRC");
    get_config = config_callback;
    message_callback = callback;

    wiFiClient = new WiFiClient();
    irc = new IRCClient(IRC_SERVER, IRC_PORT, *wiFiClient);
    irc->setCallback([] (IRCMessage ircMessage) {
        if (ircMessage.command != "PRIVMSG" || ircMessage.text[0] == '\001') {
            Serial.print("[System IRC Message] ");
            Serial.println(ircMessage.original);
            return;
        }

        IRCCallbackMessage msg {
            nick: ircMessage.nick,
            text: ircMessage.text
        };

        Serial.println(msg.text);
        message_callback(msg);
    });
    irc->setSentCallback([] (String data) {
        Serial.println(data);
    });
}

void irc_loop() {
    if (!irc->connected()) {
        Serial.println("Loading IRC config...");

        String config = get_config();
        //String config = EEPROM.readString(EEPROM_CONFIG_ADDR);
        String user = config.substring(0, config.indexOf('#'));
        String token = config.substring(config.indexOf('#') + 1);

        if (user.isEmpty() || token.isEmpty()) {
            Serial.println("Configuration is empty, please enter username and token");
            delay(5000);
            return;
        }

        delay(1000);
        Serial.println("Attempting IRC connection...");
        if (irc->connect(user, user, token)) {
            Serial.println("Connected to IRC");
            delay(200);
            
            String command("JOIN #");
            command.concat(user);
            Serial.print("Sending raw ");
            Serial.println(command);
            irc->sendRaw(command);
        } else {
            retries++;
            Serial.println("Failed... try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
            if (retries > retries_treshold) {
                ESP.restart();
            }
        }
        return;

    }
    irc->loop();
}