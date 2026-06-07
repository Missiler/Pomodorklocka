#ifndef menu_manager_h
#define menu_manager_h

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class menu_manager {
    public:
        menu_manager(Adafruit_SSD1306 &displayRef);
        void init();
        void update(unsigned long currentMillis, int potVal);
        void changeMenu();
        void buttonAction(uint8_t button);
        void nextAction(uint8_t action);

        // Explicit underlying integer type ensures matching indexes
        enum class MenuState : size_t {
            Startup,
            MainMenu,
            Settings,
            Vault,
            Running,
            Finished,
            VolumeChange,
            TimerChange,
            BreakTimeChange,
            Count // Automatically equals total number of states
        };
        
        // Bundles everything unique to a menu view
        struct StateConfig {
            const char** options;      // Pointer to its specific string array
            size_t numOptions;         // How many items are in that array
            void (menu_manager::*renderFunc)(); // Pointer to its unique class layout method
            MenuState button1Targets[3]; // State to transition to if button 1 is pressed on index 0, 1, or 2
            MenuState button2Target;    // Back button behavior state
        };

        void startupFunction();
        void mainMenuFunction();
        void settingsFunction();
        void runningFunction();
        void finishedFunction();
        void vaultFunction();
        void genericSubmenuFunction(const char* label); // Eliminates copy-pasted layout rendering functions

        void selectionManager();
        void menuPositionManager();

    private:
        Adafruit_SSD1306 &display;
        int updateNumber = 0;       
        int potentioValue_raw = 0;
        int menuPosition = 0; // Changed to int to match discrete array options
        MenuState currentState = MenuState::Startup;
        volatile MenuState nextState = MenuState::Startup;

        // Configuration table instance array
        static const StateConfig stateTable[];
};

#endif