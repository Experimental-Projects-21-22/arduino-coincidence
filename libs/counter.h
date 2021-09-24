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
    static const uint8_t bus_size = 8;
    static const uint8_t unspecified_pin = 255;

    enum class Register {
        A,
        B
    };

    // Register A lower byte, active-low puts lower byte of counter A on the Y bus.
    uint8_t GAL_pin = unspecified_pin;
    // Register A upper byte, active-low puts upper byte of counter A on the Y bus.
    uint8_t GAU_pin = unspecified_pin;

    // Register B lower byte, active-low puts lower byte of counter A on the Y bus.
    uint8_t GBL_pin = unspecified_pin;
    // Register B upper byte, active-low puts upper byte of counter A on the Y bus.
    uint8_t GBU_pin = unspecified_pin;

    // Clock clear, asynchronous active-low clear for both counters
    uint8_t CCLR_pin = unspecified_pin;
    // Register Clock, rising edge stores counters into an internal storage register.
    uint8_t RCLK_pin = unspecified_pin;

    // Data output bits 0 - 7, data_pins[0] -> Y0 (LSB) - data_pins[7] -> Y7 (MSB).
    uint8_t data_pins[bus_size] = {unspecified_pin};

    // Constructor
    CounterIC() {};

    // Pin setting functions
    void set_data_pins(uint8_t pins[bus_size]);

    // Initialization function
    void init();

    uint32_t readCounter_32bit();

    void clearCounters();

    uint32_t readDataPins();

    uint32_t readCounter(Register reg);
};

#endif /* COUNTER_H_ */
