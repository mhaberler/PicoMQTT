#ifdef M5UNIFIED
    #include <M5Unified.h>
#endif

#include <PicoMQTT.h>

#if __has_include("config.h")
    #include "config.h"
#endif

#ifndef WIFI_SSID
    #define WIFI_SSID "WiFi SSID"
#endif

#ifndef WIFI_PASSWORD
    #define WIFI_PASSWORD "password"
#endif


class CustomMQTTServer: public PicoMQTT::Server {
  protected:
    void on_connected(const char * client_id) override {
        Serial.printf("client %s connected\n", client_id);
    }
    virtual void on_disconnected(const char * client_id) override {
        Serial.printf("client %s disconnected\n", client_id);
    }
    virtual void on_subscribe(const char * client_id, const char * topic) override {
        Serial.printf("client %s subscribed %s\n", client_id, topic);
    }
    virtual void on_unsubscribe(const char * client_id, const char * topic) override {
        Serial.printf("client %s unsubscribed %s\n", client_id, topic);
    }
};

CustomMQTTServer mqtt;

void setup() {
#ifdef M5UNIFIED
    auto cfg = M5.config();
    M5.begin(cfg);
#endif

    // Setup serial
    Serial.begin(115200);

    // Connect to WiFi
    Serial.printf("Connecting to WiFi %s\n", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }
    Serial.printf("WiFi connected, IP: %s\n", WiFi.localIP().toString().c_str());

    mqtt.begin();
}

void loop() {
    mqtt.loop();
}
