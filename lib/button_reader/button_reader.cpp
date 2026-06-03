#include "button_reader.h"

button_reader::button_reader(int pin) {
    buttonPin = pin;
    interruptFlag = false;
    lastDebounceTime = 0;
}

void button_reader::init(void (*ISR_callback)()) {
    pinMode(buttonPin, INPUT_PULLUP);
    
    // Map the pin directly to the hardware interrupt
    // FALLING mode triggers right when the button goes from HIGH to LOW
    attachInterrupt(digitalPinToInterrupt(buttonPin), ISR_callback, FALLING);
}

// This runs immediately when the hardware pin drops low
void button_reader::handleInterrupt() {
    interruptFlag = true;
}

bool button_reader::isButtonPressed() {
    bool pressed = false;
    
    // Check if the interrupt hardware fired a flag
    if (interruptFlag) {
        unsigned long currentTime = millis();
        
        // Debounce: Only process if enough time has passed since the last valid click
        if ((currentTime - lastDebounceTime) > debounceDelay) {
            pressed = true;
            lastDebounceTime = currentTime; // Update the stable stamp
        }
        
        interruptFlag = false; // Reset the flag
    }
    
    return pressed;
}