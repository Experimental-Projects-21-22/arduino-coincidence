#ifndef ARDUINO_COINCIDENCE_DELAY_H
#define ARDUINO_COINCIDENCE_DELAY_H

class DelayLine {
public:
    DelayLine(const uint8_t LE, const uint8_t CLK, const uint8_t D) : LE_pin(LE), CLK_pin(CLK), D_pin(D) {}

    void setup() const;

    void set_delay(uint8_t delay) const;

private:
    const uint8_t LE_pin;
    const uint8_t CLK_pin;
    const uint8_t D_pin;
};

#endif /* ARDUINO_COINCIDENCE_DELAY_H */