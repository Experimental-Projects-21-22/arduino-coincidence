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
 */

#ifndef COUNTER_H_
#define COUNTER_H_

#include "Arduino.h"

class CounterIC {
public:
    static const uint8_t buffer_size = 8;
    
    const char *mode;

    uint8_t GAL_pin = 255;
    uint8_t GAU_pin = 255;

    uint8_t GBL_pin = 255;
    uint8_t GBU_pin = 255;

    uint8_t CCLR_pin = 255;
    uint8_t RCLK_pin = 255;
    uint8_t RCOA_pin = 255;
    uint8_t CLKBEN_pin = 255;

    uint8_t data_pins[buffer_size] = {255};

    uint8_t a_trig_pin = 255;
    uint8_t b_trig_pin = 255;

    uint32_t a_freq = 0;
    uint32_t b_freq = 0;

    //Initialization argument:
    //mode = "single" for single 32-bit counter configuration
    //mode = "dual" for dual 16-bit counters configuration
    explicit CounterIC(const char *m) : mode(m) {}

    //Pin setting functions
    void set_data_pins(uint8_t pins[buffer_size]);

    void set_gate_pins(uint8_t gau, uint8_t gal);

    void set_gate_pins(uint8_t gau, uint8_t gal, uint8_t gbu, uint8_t gbl);

    void set_clear_pin(uint8_t cclr);

    void set_regclock_pin(uint8_t rclk);

    void set_clkben_pin(uint8_t clkben);

    void set_test_pins(uint8_t a);

    void set_test_pins(uint8_t a, uint8_t b);

    //Configuration functions
    void set_testA_freq(uint32_t fa);

    void set_testB_freq(uint32_t fb);

    //Initialization function
    void init();

    //Update function
    void update();

    //Counter functions
    uint32_t readCounter(const char *ab);

    uint32_t readCounter_32bit();

    void clearCounters();

    void toggleCounterB();

    bool enabledCounterB();

    bool overFlow();

private:
    bool _clkBenable = false;
    bool _updateRunning = false;
    bool _single = false;
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
