/*
 * This file, delay.cpp, is able to interact with the DS1023-XX series using serial communication. It has been developed
 * for the purpose of controlling the delay value, however it is probably reusable for other purposes (or requires only
 * minor modifications). It contains a class SerialInterface that can be shared with multiple DS1023-XX devices.
 *
 * The general pin mapping for the DS1023-XX series is as follows:
 * latch_pin -> LE (pin 2)
 * clock_pin -> CLK (pin 4)
 * D_pin -> D (pin 5)
 * 5V -> S (pin 14) and VCC (pin 16)
 *
 * Other pins should be connected to GND (0V) or they are input/output pins.
 *
 * This software is written by Julian van Doorn <j.c.b.van.doorn@umail.leidenuniv.nl>.
 */
#ifndef ARDUINO_COINCIDENCE_DELAY_H
#define ARDUINO_COINCIDENCE_DELAY_H

class SerialInterface {
public:
    SerialInterface() = default;

    // Properly sets the pin modes.
    void setup() const;

    // Streams data from LSB to MSB over the serial pins.
    void stream(uint8_t data) const;

private:
    // The CLK (clock) pin of the serial interface with the delay lines.
    const uint8_t CLK_pin = 3;
    // The D (data) pin of the serial interface with the delay lines.
    const uint8_t D_pin = 2;
};

class DelayLine {
public:
    // Constructor
    DelayLine(const uint8_t latch_pin, const SerialInterface serial_interface) : latch_pin(latch_pin),
                                                                                 serial_interface(serial_interface) {}

    // Sets the output mode of the latch pin.
    void setup() const;

    // Sets the delay to a specific value.
    void set_delay(uint8_t delay);

private:
    // Latch pin to enable writing to the delay line.
    const uint8_t latch_pin;
    // Internal value to keep track of the current set delay.
    uint8_t delay = 0;

    // The common serial interface between all the counter chips.
    const SerialInterface serial_interface;

    // Puts an updated delay on the chip by sending it over the serial interface.
    void update_delay();
};

#endif /* ARDUINO_COINCIDENCE_DELAY_H */