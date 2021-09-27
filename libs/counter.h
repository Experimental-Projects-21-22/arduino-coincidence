/*
 * This file (counter.h) is based on the work of Justin Holland (c) 2020. It was released under the GNU Lesser General
 * Public License as published by the Free Software Foundation (version 2.1).
 *
 * This file has been modified to suit our needs in the following way:
 *  - Remove support for dual counter mode;
 *  - Use of 3 SN74LV8154 chips in parallel;
 *
 * This modification has been done by Julian van Doorn <j.c.b.van.doorn@umail.leidenuniv.nl>. The software is subject to
 * the original license.
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
    enum class Byte {
        Lower,
        Upper
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
    uint8_t read_bus(Register reg, Byte byte);

    // Reads a whole register (16 bit).
    uint16_t read_register(Register reg);

    // Puts the counts onto the registers such that they can be read.
    void save_counts_to_register() const;
};

#endif /* COUNTER_H_ */
