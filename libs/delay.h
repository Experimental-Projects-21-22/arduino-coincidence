#ifndef ARDUINO_COINCIDENCE_DELAY_H
#define ARDUINO_COINCIDENCE_DELAY_H

class SerialInterface {
public:
    SerialInterface(const uint8_t CLK, const uint8_t D) : CLK_pin(CLK), D_pin(D) {}

    void setup() const;

    void stream(uint8_t data) const;

private:
    const uint8_t CLK_pin;
    const uint8_t D_pin;
};

class DelayLine {
public:
    DelayLine(const uint8_t latch_pin, const SerialInterface serial_interface) : latch_pin(latch_pin),
                                                                                 serial_interface(serial_interface) {}

    void setup() const;

    void set_delay(uint8_t delay);

    void increment_delay(uint8_t value);

    void decrement_delay(uint8_t value);

private:
    const uint8_t latch_pin;
    uint8_t delay = 0;

    const SerialInterface serial_interface;

    void update_delay();
};

#endif /* ARDUINO_COINCIDENCE_DELAY_H */