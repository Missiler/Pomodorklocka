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
    currentState = MenuState::mainMenu;
}

void menu_manager::initialStateFunction() {
    // Code for initial state here
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("Initial State"));
    display.display();
}

void menu_manager::mainMenuFunction() {
    // Code for main menu here
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("Main Menu"));
    display.display();
}

void menu_manager::settingsFunction() {
    // Code for settings here
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("Settings"));
    display.display();
}

void menu_manager::runningFunction() {
    // Code for running state here
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("Running"));
    display.display();
}

void menu_manager::finishedFunction() {
    // Code for finished state here
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("Finished"));
    display.display();
}

void menu_manager::update(unsigned long currentMillis) {
    currentState = nextState;
    switch(currentState) {
        case MenuState::initialState:
            initialStateFunction();
            break;
        case MenuState::mainMenu:
            mainMenuFunction();
            break;
        case MenuState::settings:
            settingsFunction();
            break;
        case MenuState::running:
            runningFunction();
            break;
        case MenuState::finished:
            finishedFunction();
            break;
    }
}

void menu_manager::changeMenu() {
    // Code to change the menu here
}

void menu_manager::buttonAction(uint8_t button){

    if (button == 1){
        nextState = MenuState::mainMenu;
    }

    if(button ==2){
        nextState = MenuState::settings;
    }
}
