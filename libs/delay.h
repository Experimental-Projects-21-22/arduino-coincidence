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

    // Increments the delay with a specific value.
    void increment_delay(uint8_t value);

    // Decrements the delay with a specific value.
    void decrement_delay(uint8_t value);

    // Returns the current delay value.
    uint8_t get_delay();

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