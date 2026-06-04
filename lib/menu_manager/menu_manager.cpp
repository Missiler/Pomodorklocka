#include "menu_manager.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// FIX 1: Change 'displayRef(displayRef)' to 'display(displayRef)' 
// This correctly links the incoming screen reference to your private header variable 'display'
menu_manager::menu_manager(Adafruit_SSD1306 &displayRef) : display(displayRef) {
    // Constructor code here (if needed)
}

void menu_manager::init() {
    // Note: We use 'display' now instead of 'displayRef'
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Hello, World!"));
    display.display();
}

void menu_manager::nextAction(uint8_t action){
    
}

void menu_manager::update() {
    display.clearDisplay();
    
    // FIX 2: Ensure text properties are explicitly defined for the refresh frame
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE); 
    
    display.setCursor(0, 0); // Start at top-left corner
    display.println(F("Update!"));
    
    display.display();

}

void menu_manager::changeMenu() {
    // Code to change the menu here
}

void menu_manager::buttonAction(uint8_t button){

}
