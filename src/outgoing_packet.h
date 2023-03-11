#pragma once

// #define MQTT_OUTGOING_PACKET_DEBUG

#include <Arduino.h>
#include "Print.h"

#include "packet.h"
#include "buffer.h"

namespace NanoMQTT {

class OutgoingPacket: public Packet, public Print {
    public:
        OutgoingPacket(Print & print, Buffer & buffer, Type type, uint8_t flags, size_t payload_size);
        virtual ~OutgoingPacket();

        const OutgoingPacket & operator=(const OutgoingPacket &) = delete;
        OutgoingPacket(OutgoingPacket && other);

        // this is for Print class compatibility
        size_t write(const uint8_t * data, size_t length) override { return write((void *) data, length); }
        size_t write(uint8_t value) override { return write_u8(value); }

        size_t write(const void * data, size_t length);
        size_t write_P(const void * data, size_t length);
        size_t write_u8(uint8_t value);
        size_t write_u16(uint16_t value);
        size_t write_string(const char * string, uint16_t size);

        void flush() override;
        virtual bool send();

    protected:
        OutgoingPacket(const OutgoingPacket &) = default;

        size_t write(const void * data, size_t length, void * (*memcpy_fn)(void *, const void *, size_t n));
        size_t write_packet_length(size_t length);
        size_t write_header(uint8_t type, uint8_t flags = 0, size_t size = 0);

        Print & print;
        Buffer & buffer;

        enum class State {
            ok,
            sent,
            error,
            dead,
        } state;
};

}