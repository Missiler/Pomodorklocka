#ifndef menu_manager_h
#define menu_manager_h

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class menu_manager{
    public:

        //Actions that is used in the main code.
        menu_manager(Adafruit_SSD1306 &displayRef);
        void init();
        void update(unsigned long currentMillis);
        void changeMenu();
        void buttonAction(uint8_t button);
        void nextAction(uint8_t action);

        //Menu states and their functions:
        enum class MenuState {
            initialState,
            mainMenu,
            settings,
            running,
            finished
        };
        
        void initialStateFunction();
        void mainMenuFunction();
        void settingsFunction();
        void runningFunction();
        void finishedFunction();

        //Functions that are internal for the menu manager.
        



    private:
        Adafruit_SSD1306 &display; // Your private variable is named 'display'
        int updateNumber = 0;       // Make sure this is declared here so updateNumber++ works!
        MenuState currentState = MenuState::initialState;
        MenuState nextState = MenuState::initialState;
};



#endif