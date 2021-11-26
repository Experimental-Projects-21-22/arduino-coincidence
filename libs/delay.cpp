#include "Arduino.h"
#include "delay.h"


void DelayLine::setup() const {
    pinMode(latch_pin, OUTPUT);
    digitalWrite(latch_pin, LOW);
}

void DelayLine::update_delay() {
    // Enable writing to the delay line.
    digitalWrite(latch_pin, HIGH);
    // Stream the delay over the common serial interface
    serial_interface.stream(delay);
    // Disable writing to the delay line.
    digitalWrite(latch_pin, LOW);
}

void DelayLine::set_delay(uint8_t value) {
    delay = value;
    update_delay();
}

void DelayLine::increment_delay(uint8_t value) {
    // Update the internal value of delay
    delay += value;
    update_delay();
}

void DelayLine::decrement_delay(uint8_t value) {
    // Update the internal value of delay
    delay -= value;
    update_delay();
}

void SerialInterface::setup() const {
    pinMode(CLK_pin, OUTPUT);
    digitalWrite(CLK_pin, LOW);

    pinMode(D_pin, OUTPUT);
    digitalWrite(D_pin, LOW);
}

void SerialInterface::stream(uint8_t data) const {
    // Write the value MSB to LSB over serial.
    for (int i = 8; i > 0; --i) {
        // Set the correct bit on the D pin
        digitalWrite(D_pin, (data >> (i - 1)) & 0x01);
        // Trigger the clock
        digitalWrite(CLK_pin, HIGH);
        digitalWrite(CLK_pin, LOW);
    }

    // Reset the D pin
    digitalWrite(D_pin, LOW);
}

uint8_t DelayLine::get_delay() {
    return delay;
}