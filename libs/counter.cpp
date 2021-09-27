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

    for (uint8_t pin: bus_pins) {
        pinMode(pin, INPUT);
    }
}

void CounterIC::save_counts_to_register() {
    digitalWrite(RCLK_pin, HIGH);
    delayMicroseconds(2);
    digitalWrite(RCLK_pin, LOW);
}

uint16_t CounterIC::read_register(Register reg) {
    /*  Read the value stored on the internal register of the SN74LV8154
     *  Argument: "A" for counter A
     *			  "B" for counter B
     *  If a shift register object has been attached to this counter object
     *  it will automatically be used to retrieve the counter values.  Otherwise,
     *  this function will attempt to read the parallel data pins from the
     *  SN74LV8154 by default.
     */
    return read_bus(reg, Byte::Upper) << 8 | read_bus(reg, Byte::Lower);
}

uint32_t CounterIC::read_counter() {
    /*  Read the 32-bit value stored on the internal register of the SN74LV8154
     *  This function can only be called when the IC is configured as a single 32-bit counter
     *  by either connecting the CLKBEN pin to the RCOA pin or toggling Counter B on using this
     *  library when an overflow occurs on Counter A.
     */
    save_counts_to_register();
    return read_register(Register::B) << 16 | read_register(Register::A);
}

void CounterIC::reset_counter() const {
    /*  This function clears the values stored in the internal register in the SN74LV8154
     *  for both counters A and B.
     */
    digitalWrite(CCLR_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(CCLR_pin, HIGH);
}

uint8_t CounterIC::read_bus(Register reg, Byte byte) {
    uint8_t data_out = 0;

    uint8_t pin = reg == Register::A ?
                  (byte == Byte::Lower ? GAL_pin : GAU_pin) :
                  (byte == Byte::Lower ? GBL_pin : GBU_pin);

    digitalWrite(pin, LOW);
    delayMicroseconds(2);

    for (int i = 0; i < bus_size; ++i) {
        data_out = data_out | (digitalRead(bus_pins[i]) << i);
    }

    digitalWrite(pin, HIGH);

    return data_out;
}
