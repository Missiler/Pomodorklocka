#ifndef menu_manager_h
#define menu_manager_h

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class menu_manager{
    public:

        menu_manager(Adafruit_SSD1306 &displayRef);
        void init();
        void update();
        void changeMenu();
        void buttonAction(uint8_t button);
        void nextAction(uint8_t action);

        enum class MenuState {
            initialState,
            mainMenu,
            settings,
            running,
            finished
        };



    private:
        Adafruit_SSD1306 &display; // Your private variable is named 'display'
        int updateNumber = 0;       // Make sure this is declared here so updateNumber++ works!
        MenuState cuurentState = MenuState::initialState;
};



#endif