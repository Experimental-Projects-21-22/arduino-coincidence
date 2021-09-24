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
    
    enum class Mode {
        SINGLE,
        DUAL
    };
    enum class Register {
        A,
        B
    };
    
    const Mode mode;

    // Gate A lower byte, active-low puts lower byte of counter A on the Y bus.
    uint8_t GAL_pin = unspecified_pin;
    // Gate A upper byte, active-low puts upper byte of counter A on the Y bus.
    uint8_t GAU_pin = unspecified_pin;

    // Gate B lower byte, active-low puts lower byte of counter A on the Y bus.
    uint8_t GBL_pin = unspecified_pin;
    // Gate B upper byte, active-low puts upper byte of counter A on the Y bus.
    uint8_t GBU_pin = unspecified_pin;

    // Clock clear, asynchronous active-low clear for both counters
    uint8_t CCLR_pin = unspecified_pin;
    // Register Clock, rising edge stores counters into an internal storage register.
    uint8_t RCLK_pin = unspecified_pin;
    // Clock B enable, active-low allows clocking for counter B; connect to RCOA for 32-bit counter.
    uint8_t CLKBEN_pin = unspecified_pin;

    // Data output bits 0 - 7, data_pins[0] -> Y0 (LSB) - data_pins[7] -> Y7 (MSB).
    uint8_t data_pins[bus_size] = {unspecified_pin};

    // Clock A, rising edge count clock - used for testing.
    uint8_t CLKA_pin = unspecified_pin;
    // Clock B, rising edge count clock - used for testing.
    uint8_t CLKB_pin = unspecified_pin;

    // Count frequency for A - used for testing.
    uint32_t a_freq = 0;
    // Count frequency for B - used for testing.
    uint32_t b_freq = 0;

    // Constructor
    explicit CounterIC(Mode mode) : mode(mode) {}

    // Pin setting functions
    void set_data_pins(uint8_t pins[bus_size]);

    // Initialization function
    void init();

    // Update function
    void update();

    //Counter functions
    uint32_t readCounter(Register reg);

    uint32_t readCounter_32bit();

    void clearCounters();

    void toggleCounterB();

    bool enabledCounterB();

private:
    bool _clkBenable = false;
    bool _updateRunning = false;
    bool _overflow = false;
    bool _toggle = false;
    bool _clear = false;
    bool _testA = false;
    bool _testB = false;
    bool _resetTimers = false;
    bool _resetTimer1 = false;
    bool _resetTimer2 = false;
    bool _resetTimer3 = false;
    uint32_t t0_1 = 0;
    uint32_t t0_2 = 0;
    uint32_t t0_3 = 0;
    uint32_t tf_1 = 0;
    uint32_t tf_2 = 0;
    uint32_t tf_3 = 0;
    uint32_t testA_delay = 0;
    uint32_t testB_delay = 0;

    uint32_t readDataPins();

};

#endif /* COUNTER_H_ */
