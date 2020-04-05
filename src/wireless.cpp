#include <WiFi.h>
#include <EEPROM.h>

#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <IRCClient.h>

#include "wireless.h"

AsyncWebServer* server;
DNSServer dns;

const char* configHTML = " \
    <form method=\"POST\" action=\"/\"> \
        User: <input type=\"text\" name=\"user\"> \
        <br> \
        Token: <input type=\"text\" name=\"token\"> \
        <br> \
        <input type=\"submit\"> \
    </form> \
";

void startConfigServer() {
    delete server;
    Serial.println("Starting config server");
    server = new AsyncWebServer(80);

    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", configHTML);
    });

    server->on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (!request->hasParam("user", true) || !request->hasParam("token", true)) {
            request->send(400, "text/plain", "Missing user or token param");
            return;
        }

        String config(request->getParam("user", true)->value());
        config.concat("#"); // Delimiter
        config.concat("oauth:");
        config.concat(request->getParam("token", true)->value());

        EEPROM.writeString(EEPROM_CONFIG_ADDR, config);
        EEPROM.commit();

        Serial.println("Configuration saved to EEPROM " + config);
    });

    server->onNotFound([] (AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Page not found");
    });

    server->begin();
    Serial.println("Started config server");
}

void wifi_setup() {
    server = new AsyncWebServer(80);
    AsyncWiFiManager wifiManager(server, &dns);

    wifiManager.setAPCallback([] (AsyncWiFiManager* manager) {
        Serial.println("Entered config mode");
        Serial.println(WiFi.softAPIP());
        Serial.println(manager->getConfigPortalSSID());
    });

    Serial.println("Connecting to WiFi");
    if(!wifiManager.autoConnect()) {
        Serial.println("failed to connect and hit timeout");
        //reset and try again, or maybe put it to deep sleep
        ESP.restart();
        delay(1000);
    }
    Serial.println("Connected to WiFi");
    
    startConfigServer();
}