#ifndef menu_manager_h
#define menu_manager_h

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BitmapBank.h>

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
            Count,

            //Library States.
            LibraryOverview,
            ChooseBigSide,
            ChooseSmallSide,
            ChooseGround,
            ChooseFishes,


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
        struct AnimationInstance {
            DisplayAsset id;
            unsigned long lastFrameUpdateTime = 0;
            int currentFrameIndex = 0;
            int x = 0;
            int y = 0;
            bool mirror = false; // <-- LÄGG TILL DENNA RAD!

            // Reset helper for when you switch assets on the fly
            void changeAsset(DisplayAsset newId, int posX, int posY, bool shouldMirror) {
                id = newId;
                x = posX;
                y = posY;
                currentFrameIndex = 0;
                lastFrameUpdateTime = 0;
                mirror = shouldMirror; // <-- LÄGG TILL DENNA RAD!
            }
        };

        void drawDisplay(AnimationInstance& instance);
        static const int MAX_FISHES = 3; // Vi sätter ett fast antal fisk-slots vi kan styra
        struct FishData {
            menu_manager::AnimationInstance instance;
            DisplayAsset assetId;
            int x;
            int y;
            int baseY;
            int speed;
            bool facingRight;
            unsigned long nextSpawnTime;
            float waveDensity;
            float waveHeight;
            int minSpawnDelay;
            int maxSpawnDelay;
        };

        // Ändra egenskaperna på en specifik fisk (0 till MAX_FISHES - 1)
        void setFish(int index, DisplayAsset asset, float density, float height);
        
        // Den nya menyfunktionen för att ändra fiskar
        void fishConfigMenuFunction();
        void fishSwim();

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

        //Animation
        int currentFrameIndex = 0;
        unsigned long lastFrameUpdateTime = 0;
        AnimationInstance coinInstance;
        menu_manager::AnimationInstance vaultAssetInstance; // <-- ADD THIS LINE

        menu_manager::AnimationInstance mainMenuAssetInstance1; // <-- ADD THIS LINE
        menu_manager::AnimationInstance mainMenuAssetInstance2; // <-- ADD THIS LINE
        menu_manager::AnimationInstance mainMenuAssetInstance3; // <-- ADD THIS LINE
    
        menu_manager::AnimationInstance runningBigR;
        menu_manager::AnimationInstance runningBigL;
        menu_manager::AnimationInstance runningLilR;
        menu_manager::AnimationInstance runningLilL;
        menu_manager::AnimationInstance runningFish1;
        menu_manager::AnimationInstance runningFish2;
        menu_manager::AnimationInstance runningFish3;

        DisplayAsset activeRunningBigL = DisplayAsset::Geyser;
        DisplayAsset activeRunningBigR = DisplayAsset::Seaweed;
        DisplayAsset activeRunningLilR = DisplayAsset::Sjogurka;
        DisplayAsset activeRunningLilL = DisplayAsset::SmallKorall;

        //Fiskar som simmar
        DisplayAsset activeRunningFish1 = DisplayAsset::AnglerFish;
        DisplayAsset activeRunningFish2 = DisplayAsset::ClassicFish;
        DisplayAsset activeRunningFish3 = DisplayAsset::FrenchFish;

        FishData aquarium[MAX_FISHES];
        int selectedFishIndex = 0; // Vilken fisk-slot (0-3) vi pillar på i menyn
        bool aquariumInitialized = false;

        int fish1_X = -25;
        int fish2_X = -25;
        int fish3_X = -25;
        
        unsigned long nextFish1Time = 0;
        unsigned long nextFish2Time = 0;
        unsigned long nextFish3Time = 0;
        
        int fish1_speed = 2;
        int fish2_speed = 1;
        int fish3_speed = 1;

        bool fish1_facingRight = true;
        bool fish2_facingRight = true;
        bool fish3_facingRight = true;
        
};

#endif