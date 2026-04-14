#include <Arduino.h>
#include "orn_proto.h"

namespace
{
constexpr uint32_t kBlinkIntervalMs = 500;
constexpr int kLedPin = LED_BUILTIN;

uint32_t g_last_toggle_ms = 0;
bool g_led_state = false;
} // namespace

void setup()
{
    pinMode(kLedPin, OUTPUT);
    digitalWrite(kLedPin, LOW);

    Serial.begin(115200);
    delay(250);

    Serial.println();
    Serial.println("OpenRailNet RP2040 Gateway Node");
    Serial.println("Target: RP2040");
    Serial.println("Role: Gateway");

    orn_frame_t frame{};
    orn_frame_init(
        &frame,
        ORN_PKT_HELLO,
        ORN_PRIO_NORMAL,
        0x0001,
        ORN_NODE_ID_BROADCAST,
        0x00,
        0,
        0);

    Serial.print("ORN protocol frame header size: ");
    Serial.println(sizeof(frame.header));
}

void loop()
{
    const uint32_t now = millis();

    if ((now - g_last_toggle_ms) >= kBlinkIntervalMs)
    {
        g_last_toggle_ms = now;
        g_led_state = !g_led_state;
        digitalWrite(kLedPin, g_led_state ? HIGH : LOW);

        Serial.println("ORN gateway heartbeat");
    }
}