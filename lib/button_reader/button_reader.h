#ifndef button_reader_h
#define button_reader_h

#include <Arduino.h>

class button_reader {
    public:
        button_reader(int pin);
        void init(void (*ISR_callback)()); // Pass the global ISR function here
        void handleInterrupt();            // The function triggered by the ISR
        bool isButtonPressed();

    private:
        int buttonPin;
        volatile bool interruptFlag;       // Set inside the ISR
        unsigned long lastDebounceTime;
        const unsigned long debounceDelay = 200; // Time frame between allowed presses
};

#endif