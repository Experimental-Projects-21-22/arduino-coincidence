/*
 * This file (counter.cpp) is based on the work of Justin Holland (c) 2020. It was released under the GNU Lesser General
 * Public License as published by the Free Software Foundation (version 2.1).
 *
 * This file has been modified to suit our needs in the following way:
 *  - Remove support for dual counter mode;
 *  - Use of 3 SN74LV8154 chips in parallel;
 *
 * This modification has been done by Julian van Doorn <j.c.b.van.doorn@umail.leidenuniv.nl>. The software is subject to
 * the original license.
 */

#include "Arduino.h"
#include "counter.h"

void CounterIC::setup() const {
    pinMode(GAL_pin, OUTPUT);
    pinMode(GAU_pin, OUTPUT);
    pinMode(GBL_pin, OUTPUT);
    pinMode(GBU_pin, OUTPUT);
    digitalWrite(GAL_pin, HIGH);
    digitalWrite(GAU_pin, HIGH);
    digitalWrite(GBL_pin, HIGH);
    digitalWrite(GBU_pin, HIGH);

    pinMode(RCLK_pin, OUTPUT);
    digitalWrite(RCLK_pin, LOW);

    pinMode(CCLR_pin, OUTPUT);
    digitalWrite(CCLR_pin, HIGH);

    for (const auto &bus: bus_pins) {
        for (const auto &pin: bus) {
            pinMode(pin, INPUT);
        }
    }
}

void CounterIC::save_counts_to_register() const {
    digitalWrite(RCLK_pin, HIGH);
    delayMicroseconds(2);
    digitalWrite(RCLK_pin, LOW);
}

void CounterIC::read_counters(uint32_t *out) {
    for (int i = 0; i < counters; ++i) {
        out[i] = read_counter(i);
    }
}

uint32_t CounterIC::read_counter(uint8_t counter) {
    return ((uint32_t) read_register(counter, Register::B)) << 16 | read_register(counter, Register::A);
}

uint16_t CounterIC::read_register(uint8_t counter, Register reg) {
    return ((uint16_t) read_bus(counter, reg, Byte::Upper)) << 8 | read_bus(counter, reg, Byte::Lower);
}

uint8_t CounterIC::read_bus(uint8_t counter, Register reg, Byte byte) {
    uint8_t data_out = 0;

    uint8_t pin = reg == Register::A ?
                  (byte == Byte::Lower ? GAL_pin : GAU_pin) :
                  (byte == Byte::Lower ? GBL_pin : GBU_pin);
    digitalWrite(pin, LOW);
    delayMicroseconds(2);

    for (int i = 0; i < bus_size; ++i) {
        data_out = data_out | (digitalRead(bus_pins[counter][i]) << i);
    }

    digitalWrite(pin, HIGH);
    return data_out;
}

void CounterIC::reset_counters() const {
    digitalWrite(CCLR_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(CCLR_pin, HIGH);
}