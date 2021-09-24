/*counter.cpp - Source file for SN74LV8154 IC driver
  Copyright (c) 2020 Justin Holland.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  Modified by Julian van Doorn.
 */

#include "Arduino.h"
#include "counter.h"

void CounterIC::set_data_pins(uint8_t pins[bus_size]) {
    /*  Data pins on SN74LV8154 for parallel output.
     *      Y0 (LSB) -> pins[0]
     *      ...
     *      Y7 (MSB) -> pins[7]
     */
    for (int i = 0; i < bus_size; ++i) {
        data_pins[i] = pins[i];
    }
}

void CounterIC::init() {
    //Initialize Gate pins
    if (GAL_pin == unspecified_pin || GAU_pin == unspecified_pin || GBL_pin == unspecified_pin ||
        GBU_pin == unspecified_pin) {
        Serial.println("fatal error: From CounterIC::init() -- all gate pins must be defined!");
        while (1);
    } else {
        pinMode(GAL_pin, OUTPUT);
        pinMode(GAU_pin, OUTPUT);
        pinMode(GBL_pin, OUTPUT);
        pinMode(GBU_pin, OUTPUT);
        digitalWrite(GAL_pin, HIGH);
        digitalWrite(GAU_pin, HIGH);
        digitalWrite(GBL_pin, HIGH);
        digitalWrite(GBU_pin, HIGH);
    }

    //Initialize Register Clock
    if (RCLK_pin != unspecified_pin) {
        pinMode(RCLK_pin, OUTPUT);
        digitalWrite(RCLK_pin, LOW);
    } else {
        Serial.println("fatal error: From CounterIC::init() -- register clock pin must be defined!");
    }

    for (int i = 0; i < bus_size; ++i) {
        if (data_pins[i] == unspecified_pin) {
            Serial.println("fatal error: From CounterIC::init() -- data pins (Y0-Y7) must be defined!");
            while (1);
        }
    }

    if (CCLR_pin != unspecified_pin) {
        pinMode(CCLR_pin, OUTPUT);
        digitalWrite(CCLR_pin, HIGH);
    } else {
        Serial.println("fatal error: From CounterIC::init() -- CCLR pin must be defined!");
        while (1);
    }
}

uint32_t CounterIC::readCounter(Register reg) {
    /*  Read the value stored on the internal register of the SN74LV8154
     *  Argument: "A" for counter A
     *			  "B" for counter B
     *  If a shift register object has been attached to this counter object
     *  it will automatically be used to retrieve the counter values.  Otherwise,
     *  this function will attempt to read the parallel data pins from the
     *  SN74LV8154 by default.
     */
    uint32_t data_out = 0x00;

    digitalWrite(RCLK_pin, HIGH);
    delayMicroseconds(2);
    digitalWrite(RCLK_pin, LOW);

    //Read A counter
    if (reg == Register::A) {
        digitalWrite(GAU_pin, LOW);
        delayMicroseconds(2);
        data_out = readDataPins();
        digitalWrite(GAU_pin, HIGH);

        digitalWrite(GAL_pin, LOW);
        delayMicroseconds(2);
        data_out = (data_out << 8) | readDataPins();
        digitalWrite(GAL_pin, HIGH);
    } else if (reg == Register::B) {
        digitalWrite(GBU_pin, LOW);
        delayMicroseconds(2);
        data_out = readDataPins();
        digitalWrite(GBU_pin, HIGH);

        digitalWrite(GBL_pin, LOW);
        delayMicroseconds(2);
        data_out = (data_out << 8) | readDataPins();
        digitalWrite(GBL_pin, HIGH);
    }

    return data_out;
}

uint32_t CounterIC::readCounter_32bit() {
    /*  Read the 32-bit value stored on the internal register of the SN74LV8154
     *  This function can only be called when the IC is configured as a single 32-bit counter
     *  by either connecting the CLKBEN pin to the RCOA pin or toggling Counter B on using this
     *  library when an overflow occurs on Counter A.
     */
    uint32_t low_byte = readCounter(Register::A);
    uint32_t high_byte = readCounter(Register::B);

    return (high_byte << 16) | low_byte;
}

void CounterIC::clearCounters() {
    /*  This function clears the values stored in the internal register in the SN74LV8154
     *  for both counters A and B.
     */
    digitalWrite(CCLR_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(CCLR_pin, HIGH);
}

uint32_t CounterIC::readDataPins() {
    uint32_t data_out = 0x00;

    for (int i = 0; i < bus_size; ++i) {
        data_out = data_out | (digitalRead(data_pins[i]) << i);
    }

    return data_out;
}
