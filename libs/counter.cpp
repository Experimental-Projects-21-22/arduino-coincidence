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
}

uint32_t CounterIC::read_register(Register reg) {
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
        data_out = read_bus();
        digitalWrite(GAU_pin, HIGH);

        digitalWrite(GAL_pin, LOW);
        delayMicroseconds(2);
        data_out = (data_out << 8) | read_bus();
        digitalWrite(GAL_pin, HIGH);
    } else if (reg == Register::B) {
        digitalWrite(GBU_pin, LOW);
        delayMicroseconds(2);
        data_out = read_bus();
        digitalWrite(GBU_pin, HIGH);

        digitalWrite(GBL_pin, LOW);
        delayMicroseconds(2);
        data_out = (data_out << 8) | read_bus();
        digitalWrite(GBL_pin, HIGH);
    }

    return data_out;
}

uint32_t CounterIC::read_counter() {
    /*  Read the 32-bit value stored on the internal register of the SN74LV8154
     *  This function can only be called when the IC is configured as a single 32-bit counter
     *  by either connecting the CLKBEN pin to the RCOA pin or toggling Counter B on using this
     *  library when an overflow occurs on Counter A.
     */
    uint32_t low_byte = read_register(Register::A);
    uint32_t high_byte = read_register(Register::B);

    return (high_byte << 16) | low_byte;
}

void CounterIC::clear_counter() const {
    /*  This function clears the values stored in the internal register in the SN74LV8154
     *  for both counters A and B.
     */
    digitalWrite(CCLR_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(CCLR_pin, HIGH);
}

uint32_t CounterIC::read_bus() {
    uint32_t data_out = 0x00;

    for (int i = 0; i < bus_size; ++i) {
        data_out = data_out | (digitalRead(bus_pins[i]) << i);
    }

    return data_out;
}
