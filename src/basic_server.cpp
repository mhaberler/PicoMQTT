#include <PicoMQTT.h>

#if __has_include("config.h")
    #include "config.h"
#endif

#include <ProxyWebSocketsServer.h>

#ifndef WIFI_SSID
    #define WIFI_SSID "WiFi SSID"
#endif

#ifndef WIFI_PASSWORD
    #define WIFI_PASSWORD "password"
#endif
#define INTERVAL 5000
PicoMQTT::Server mqtt(1883);
// PicoMQTT::Client client("127.0.0.1", 1883, "loccl");

#define PROXY_DEST "127.0.0.1"
ProxyWebSocketsServer *webSocket;

void setup() {
    delay(3000);
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


    mqtt.subscribe("#", [](const char * topic, const char * payload) {
        // payload might be binary, but PicoMQTT guarantees that it's zero-terminated
        Serial.printf("Received message in topic '%s': %s\n", topic, payload);
    });
    mqtt.begin();

    webSocket = new ProxyWebSocketsServer(8883, PROXY_DEST, 1883, 500);
    // webSocket->begin();

}
uint32_t last;

void loop() {
    mqtt.loop();
    // client.loop();
    if (webSocket)
        webSocket->loop();

    // if (millis() - last > INTERVAL) {
    //     last = millis();
    //     mqtt.publish("blah", "fasel");
    // }
}
