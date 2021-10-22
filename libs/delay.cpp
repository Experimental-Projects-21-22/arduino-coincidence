#include "Arduino.h"
#include "delay.h"


void DelayLine::setup() const {
    pinMode(LE_pin, OUTPUT);
    digitalWrite(LE_pin, LOW);

    pinMode(CLK_pin, OUTPUT);
    digitalWrite(CLK_pin, LOW);

    pinMode(D_pin, OUTPUT);
    digitalWrite(D_pin, LOW);
}

void DelayLine::set_delay(uint8_t value) const {
    // Enable writing to the delay line.
    digitalWrite(LE_pin, HIGH);

    // Write the value LSB to MSB over serial.
    for (int i = 8; i > 0; --i) {
        // Set the correct bit on the D pin
        digitalWrite(D_pin, ((value >> i) & 0x01));
        // Trigger the clock
        digitalWrite(CLK_pin, HIGH);
        delayMicroseconds(1);
        digitalWrite(CLK_pin, LOW);
        delayMicroseconds(1);
    }

    // Disable writing to the delay line.
    digitalWrite(LE_pin, LOW);
    // Reset the D pin
    digitalWrite(D_pin, LOW);
}
