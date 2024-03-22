#include <ProxyWebSocketsServer.h>

#define USE_SERIAL Serial

void hexdump(const void *mem, uint32_t len, uint8_t cols ) {
    const uint8_t* src = (const uint8_t*) mem;
    USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
    for(uint32_t i = 0; i < len; i++) {
        if(i % cols == 0) {
            USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
        }
        USE_SERIAL.printf("%02X ", *src);
        src++;
    }
    USE_SERIAL.printf("\n");
}

bool validateHttpHeader(String headerName, String headerValue) {

    //assume a true response for any headers not handled by this validator
    // bool valid = true;

    // if(headerName.equalsIgnoreCase("Cookie")) {
    // 	//if the header passed is the Cookie header, validate it according to the rules in 'isCookieValid' function
    // 	valid = isCookieValid(headerValue);
    // }
    USE_SERIAL.printf("---- header validate OK\n");
    return true;
}

void ProxyWebSocketsServer::_loop(void) {
    for (int i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++) {
        WiFiClient *d = _dest[i];

        if (d && d->connected()) {
            while (d->available()) {
                uint8_t buffer[TOWS_SIZE];
                int res = d->read(buffer, sizeof(buffer));
                // if ((res > 0) && (_clients[i].status == WSC_CONNECTED)) {
                if ((res > 0)) {
                    Serial.printf("--->toclient %d '%s'\n", i, buffer);

                    this->sendBIN(i, buffer, res);
                }
            }
        }
        // while (d && d->available()) {
        //     uint8_t buffer[TOWS_SIZE];
        //     int res = d->read(buffer, sizeof(buffer));
        //     // if ((res > 0) && (_clients[i].status == WSC_CONNECTED)) {
        //     if ((res > 0)) {
        //         Serial.printf("--->toclient %d '%s'\n", i, buffer);

        //         this->sendTXT(i, buffer, res);
        //     }
        // }
    }
}


void _proxyWebSocketEvent(WebSocketsServerCore *server, uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    ProxyWebSocketsServer *pws = (ProxyWebSocketsServer*)server;

    WSclient_t *wsc = &pws->_clients[num];
    WiFiClient *wc = pws->_dest[num];

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected\n", num);
            if (wc && wc->connected()) {
                wc->stop();
                delete wc;
                pws->_dest[num] = nullptr;
            }
            break;
        case WStype_CONNECTED: {
                // if (wc && wc->connected()) {
                //     wc->stop();
                //     delete wc;
                //     pws->_dest[num] = nullptr;
                // }
                IPAddress ip = wsc->tcp->remoteIP() ;
                uint16_t port = wsc->tcp->remotePort() ;
                Serial.printf("[%u] Connected from %d.%d.%d.%d port %u url: %s len=%u\n", num, ip[0], ip[1], ip[2], ip[3], port, payload, length);

                pws->_dest[num] = new WiFiClient;
                WiFiClient *cp =  pws->_dest[num];

                if (!cp->connect(pws->_destHost.c_str(), pws->_destPort, pws->_timeout_ms)) {
                    delete pws->_dest[num] ;
                    pws->_dest[num] = nullptr;
                    Serial.printf("[%u] proxy connect failed\n", num);
                    break;
                }
                // uint8_t reply[2] = {0};
                // pws->sendBIN(num, reply, 0);
                // cp->write(payload, length);
                // pws->_dest[num] = cp;
                Serial.printf("[%u] proxy connect success connected=%d  wc=%p\n", num, cp->connected(), cp);
    pws->loop();
                // pws->sendTXT(num, "");
                // pws->enableHeartbeat(3000,10000, 30000);
                // while (cp->available()) {
                //     uint8_t buffer[TOWS_SIZE];
                //     int res = cp->read(buffer, sizeof(buffer));
                //     // if ((res > 0) && (_clients[i].status == WSC_CONNECTED)) {
                //     if ((res > 0)) {
                //         Serial.printf("--->toclient %d '%s'\n", num, buffer);

                //         pws->sendTXT(num, buffer, res);
                //     }
                // }
            }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);
            hexdump(payload, length);

            if (wc && wc->connected()) {
                wc->write(payload, length);
            }
            break;
        case WStype_BIN:
            Serial.printf("[%u] get binary length: %u wc=%p wc->connected=%d payload=%s\n", num, length, wc, wc && wc->connected(), payload);
            hexdump(payload, length);
            // if (wc && wc->connected()) {
            if (wc) {
                Serial.printf("[%u] ----->write binary length: %u\n", num, length);

                wc->write(payload, length);
            }
            WEBSOCKETS_YIELD();

            break;
        case WStype_ERROR:
        case WStype_FRAGMENT_TEXT_START:
        case WStype_FRAGMENT_BIN_START:
        case WStype_FRAGMENT:
        case WStype_FRAGMENT_FIN:
            Serial.printf("wsmessage %u\n", type);
            break;
    }
}