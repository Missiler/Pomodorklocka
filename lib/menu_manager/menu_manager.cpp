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
    selectionManager();
    display.setCursor(0,0);
    display.setTextColor(SSD1306_WHITE);
    display.println(F("Initial State"));
    display.display();
}

void menu_manager::mainMenuFunction() {
    // Code for main menu here
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("Main Menu"));
    selectionManager();
    display.display();
}

void menu_manager::settingsFunction() {
    // Code for settings here
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("Settings"));
    selectionManager();
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

void menu_manager::menuPositionManager() {
    // 1. Layout configurations (Adjusted radius slightly to prevent bleeding!)
    int radius = 24; 
    int centerX = 64;
    int centerY = 55; 
    const int charWidth = 6;
    const int charHeight = 7;

    // 2. Generic menu pointers (Declared outside the switch so the loop can see them)
    const char** menuOptions = nullptr;
    size_t numOptions = 0;

    // 3. Define the actual arrays for each state
    const char* initialOptions[] = {"This", "Is", "Nice"};
    const char* mainOptions[]    = {"Setup", "Start", "Vault"};
    const char* settingsOptions[]= {"Volume", "Bright", "Back"};
    const char* runningOptions[] = {"Pause", "Stop"};
    const char* finishedOptions[]= {"Restart", "Exit"};

    // 4. Assign the right data depending on the state
    switch(currentState) {
        case MenuState::initialState:
            menuOptions = initialOptions;
            numOptions = sizeof(initialOptions) / sizeof(initialOptions[0]); // Auto-calculates size (3)
            break;
            
        case MenuState::mainMenu:
            menuOptions = mainOptions;
            numOptions = sizeof(mainOptions) / sizeof(mainOptions[0]);       // Auto-calculates size (3)
            break;
            
        case MenuState::settings:
            menuOptions = settingsOptions;
            numOptions = sizeof(settingsOptions) / sizeof(settingsOptions[0]);
            break;
            
        case MenuState::running:
            menuOptions = runningOptions;
            numOptions = sizeof(runningOptions) / sizeof(runningOptions[0]); // This math safely changes to 2 options!
            break;
            
        case MenuState::finished:
            menuOptions = finishedOptions;
            numOptions = sizeof(finishedOptions) / sizeof(finishedOptions[0]);
            break;

        default:
            return; // Safety exit if state is invalid
    }
    
    // Safety check: If a state doesn't have options defined, don't run the rest of the code
    if (menuOptions == nullptr || numOptions == 0) return;

    // 5. Potentiometer Mapping (Now adapts perfectly whether you have 2, 3, or 5 options!)
    int currentSelection = map(potentioValue_raw, 0, 1024, 0, numOptions);
    if (currentSelection >= (int)numOptions) currentSelection = numOptions - 1; 

    // 6. Draw Loop
    for(size_t i = 0; i < numOptions; i++) {
        
        if (i == (size_t)currentSelection) {
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); 
        } else {
            display.setTextColor(SSD1306_WHITE, SSD1306_BLACK); 
        }

        // The math handles dynamic numbers of items. 
        // If numOptions is 2, the math prevents a divide-by-zero error using a quick ternary check:
        float denominator = (numOptions > 1) ? (numOptions - 1) : 1;
        float currentAngle = 180.0 + (i * (180.0 / denominator)); 
        float radians = currentAngle * (PI / 180.0);

        int startX = radius * cos(radians);
        int startY = radius * sin(radians);

        int stringWidth = strlen(menuOptions[i]) * charWidth;
        int finalX = centerX + startX - (stringWidth / 2);
        int finalY = centerY + startY - (charHeight / 2);

        display.setCursor(finalX, finalY);
        display.print(menuOptions[i]);
    }
}

void menu_manager::selectionManager() {
    // Code to manage selection based on potentiometer position to choose.
    menuPosition = map(potentioValue_raw, 0, 1023, 0, 3);
    menuPositionManager();

    switch(currentState){
        case MenuState::initialState:

            break;
        case MenuState::mainMenu:

            break;
        case MenuState::settings:

            break;
        case MenuState::running:

            break;
        case MenuState::finished:

            break;
    }

}

void menu_manager::update(unsigned long currentMillis, int potVal) {
    potentioValue_raw = potVal;
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
