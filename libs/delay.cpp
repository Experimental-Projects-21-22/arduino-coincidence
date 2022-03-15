/*
 * This file, delay.cpp, is able to interact with the DS1023-XX series using serial communication. It has been developed
 * for the purpose of controlling the delay value, however it is probably reusable for other purposes (or requires only
 * minor modifications). It contains a class SerialInterface that can be shared with multiple DS1023-XX devices.
 *
 * The general pin mapping for the DS1023-XX series is as follows:
 * latch_pin -> LE (pin 2)
 * clock_pin -> CLK (pin 4)
 * D_pin -> D (pin 5)
 * 5V -> S (pin 14) and VCC (pin 16)
 *
 * Other pins should be connected to GND (0V) or they are input/output pins.
 *
 * This software is written by Julian van Doorn <j.c.b.van.doorn@umail.leidenuniv.nl>.
 */

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