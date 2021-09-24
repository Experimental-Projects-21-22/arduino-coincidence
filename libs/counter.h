/*counter.h - Include file for SN74LV8154 IC driver
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

#ifndef COUNTER_H_
#define COUNTER_H_

#include "Arduino.h"

class CounterIC {
public:
    CounterIC() = default;

    // Sets the mode of each pin and prepares the counters.
    void setup() const;

    // Reads the value of the counter.
    uint32_t read_counter();

    // Resets the counter to 0.
    void reset_counter() const;

private:
    static const uint8_t bus_size = 8;

    enum class Register {
        A,
        B
    };

    // Register A lower byte, active-low puts lower byte of counter A on the Y bus.
    const uint8_t GAL_pin = 24;
    // Register A upper byte, active-low puts upper byte of counter A on the Y bus.
    const uint8_t GAU_pin = 25;

    // Register B lower byte, active-low puts lower byte of counter B on the Y bus.
    const uint8_t GBL_pin = 26;
    // Register B upper byte, active-low puts upper byte of counter B on the Y bus.
    const uint8_t GBU_pin = 27;

    // Register Clock, rising edge stores counters into an internal storage register.
    const uint8_t RCLK_pin = 28;
    // Clock clear, asynchronous active-low clear for both counters
    const uint8_t CCLR_pin = 29;

    // Data output bits 0 - 7, bus_pins[0] -> Y0 (LSB) - bus_pins[7] -> Y7 (MSB).
    const uint8_t bus_pins[bus_size] = {30, 31, 32, 33, 34, 35, 36, 37};

    // Reads the byte that is currently on the Y bus.
    uint32_t read_bus();

    // Reads a whole register (16 bit).
    uint32_t read_register(Register reg);
};

#endif /* COUNTER_H_ */
