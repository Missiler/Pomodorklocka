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
            RunningBreak,   
            Finished,
            VolumeChange,
            TimerChange,     // Row 1: Focus Time adjustment
            BreakTimeChange, // Row 2: Break Time adjustment
            TimerConfirm,    // Summary/Confirmation View Screen
            Count
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

        void timerChangeFunction();
        void timerConfirmFunction();

    private:
        Adafruit_SSD1306 &display;
        int updateNumber = 0;       
        int potentioValue_raw = 0;
        int menuPosition = 0; // Changed to int to match discrete array options
        MenuState currentState = MenuState::Startup;
        volatile MenuState nextState = MenuState::Startup;

        // Configuration table instance array
        static const StateConfig stateTable[];

        //Timer 
        // Configuration Settings Storage
        int configuredWorkSeconds  = 300;  // Default: 5 minutes
        int configuredBreakSeconds = 60;   // Default: 1 minute

        // Active Loop Tracker Variables
        int totalTimerSeconds = 0;         // Tracks the currently active view setting
        int remainingSeconds = 0;          // Shared structural tracking register
        unsigned long timerLastMillis = 0;
        bool editingBreakTime = false;
};

#endif