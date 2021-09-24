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
#include <math.h>

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
    //Single 32-bit counter mode requires specific pin configuration
    if (mode == Mode::SINGLE && CLKBEN_pin != unspecified_pin) {
        Serial.println(
                "fatal error: From CounterIC::init() -- cannot define CLKBEN or RCOA pins for single counter mode.");
        while (1);
    }

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

    //Initialize overflow status pin
    if (RCOA_pin != unspecified_pin) {
        _overflow = true;
        pinMode(RCOA_pin, INPUT_PULLUP);
    } else {
        _overflow = false;
    }

    //Initialize Clock B enable pin
    if (CLKBEN_pin != unspecified_pin) {
        _clkBenable = true;
        _toggle = true;
        pinMode(CLKBEN_pin, OUTPUT);
        digitalWrite(CLKBEN_pin, LOW);
    } else {
        _clkBenable = true;
        _toggle = false;
    }

    for (int i = 0; i < bus_size; ++i) {
        if (data_pins[i] == unspecified_pin) {
            Serial.println("fatal error: From CounterIC::init() -- data pins (Y0-Y7) must be defined!");
            while (1);
        }
    }

    //Initialize CCLR pin
    if (CCLR_pin != unspecified_pin) {
        _clear = true;
        pinMode(CCLR_pin, OUTPUT);
        digitalWrite(CCLR_pin, HIGH);
    } else {
        _clear = false;
    }

    //Initialize test pins
    if (CLKA_pin != unspecified_pin) {
        _testA = true;
        pinMode(CLKA_pin, OUTPUT);
        digitalWrite(CLKA_pin, LOW);
        testA_delay = (uint32_t) round((1.0 / a_freq) * 1E6);
        _resetTimers = true;
    } else {
        _testA = false;
    }
    if (CLKB_pin != unspecified_pin) {
        _testB = true;
        pinMode(CLKB_pin, OUTPUT);
        digitalWrite(CLKB_pin, LOW);
        testB_delay = (uint32_t) round((1.0 / b_freq) * 1E6);
        _resetTimers = true;
    } else {
        _testB = false;
    }
}

void CounterIC::update() {
    /*  This function must be called in the main loop if testing of Counters
     *  A/B is being performed.  It can also be used to monitor for overflow condition
     *  on Counter A.  Alternatively, overFlow() function can be called for this purpose
     */
    _updateRunning = true;
    tf_2 = micros();
    tf_3 = micros();


    //Handle timers for TestA and TestB signals
    if (_testA || _testB) {
        if (_resetTimers) {
            t0_2 = micros();
            t0_3 = micros();
            tf_2 = micros();
            tf_3 = micros();
            _resetTimers = false;
        }
    }

    if (_testA) {
        if (_resetTimer2) {
            t0_2 = micros();
            tf_2 = micros();
            _resetTimer2 = false;
        }

        if (tf_2 - t0_2 >= testA_delay / 2) {
            digitalWrite(CLKA_pin, !digitalRead(CLKA_pin));
            _resetTimer2 = true;
        }
    }

    if (_testB) {
        if (_resetTimer3) {
            t0_3 = micros();
            tf_3 = micros();
            _resetTimer3 = false;
        }

        if (tf_3 - t0_3 >= testB_delay / 2) {
            digitalWrite(CLKB_pin, !digitalRead(CLKB_pin));
            _resetTimer3 = true;
        }
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

    if (_testA || _testB) {
        if (!_updateRunning) {
            Serial.println("fatal error: From CounterIC::readCounter() -- CounterIC::Update() must be called in loop");
            while (1);
        }
    }

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
        if (!_clkBenable) {
            Serial.println("warning: From CounterIC::readCounter() -- counter B is not enabled, cannot read.");
            return data_out;
        }
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
    uint32_t ret = 0xFFFF;
    if (mode == Mode::SINGLE) {
        uint32_t high_byte = readCounter(Register::A);
        uint32_t low_byte = readCounter(Register::B);
        ret = (high_byte << 16) | low_byte;
    } else {
        Serial.println(
                "error: From CounterIC::readCounter_32bit() -- this function cannot be called for dual 16-bit counters configuration");
    }

    return ret;
}

void CounterIC::toggleCounterB() {
    /*  Toggles the state of the CLKBEN pin on the SN74LV8154
     *
     */
    if (_toggle) {
        digitalWrite(CLKBEN_pin, !digitalRead(CLKBEN_pin));
        if (digitalRead(CLKBEN_pin) == HIGH) { _clkBenable = false; }
        else { _clkBenable = true; }
    } else {
        Serial.println(
                "error: From CounterIC::toggleCounterB() -- cannot toggle counter B since CLKBEN pin was not defined.");
    }
}

bool CounterIC::enabledCounterB() {
    /*  Returns ON/OFF status of Counter B
     *
     */
    bool ret = false;

    if (_toggle) {
        if (digitalRead(CLKBEN_pin) == HIGH) {
            ret = false;
        } else {
            ret = true;
        }
    } else {
        Serial.println(
                "error: From CounterIC::enabledCounterB() -- this function cannot be called because CLKBEN pin was not undefined");
    }
    return ret;
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
    uint32_t val = 0x00;
    uint32_t data_out = 0x00;

    for (int i = 0; i < bus_size; ++i) {
        val = digitalRead(data_pins[i]);
        data_out = data_out | (val << i);
    }

    return data_out;
}
