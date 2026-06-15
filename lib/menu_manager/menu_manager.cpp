#include "menu_manager.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

int startupUpdateCount = 0; 

// 1. Define raw string arrays for selections
const char* startupOpts[]   = {"This", "Is", "Nice"};
const char* mainOpts[]      = {"Setup", "Start", "Vault"};
const char* settingsOpts[]  = {"Volume", "Time", "Back"};
const char* runningOpts[]   = {"Pause", "Stop"};
const char* finishedOpts[]  = {"Restart", "Exit"};
const char* volumeOpts[]    = {"Up", "Down", "Back"};

#define GET_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

//Kan ändra för att skapa custom menyer. Det visar vilka som går vart.
// --- ARCHITECTURE LAYOUT REWIRE ---
const menu_manager::StateConfig menu_manager::stateTable[] = {
    // [Startup]
    { startupOpts, GET_SIZE(startupOpts), &menu_manager::startupFunction, 
      {MenuState::Startup, MenuState::Startup, MenuState::Startup}, MenuState::Startup },
      
    // [MainMenu] -> Index 1 ("Start") now shifts directly into the [TimerConfirm] step screen
    { mainOpts, GET_SIZE(mainOpts), &menu_manager::mainMenuFunction, 
      {MenuState::TimerConfirm, MenuState::Settings, MenuState::Vault}, MenuState::MainMenu },
      
    // [Settings] -> Options: {"Volume", "Bright", "Back"}. Index 1 ("Bright") links straight to Focus config
    { settingsOpts, GET_SIZE(settingsOpts), &menu_manager::settingsFunction, 
      {MenuState::VolumeChange, MenuState::TimerChange, MenuState::MainMenu}, MenuState::MainMenu },
      
    // [Vault]
    { nullptr, 0, &menu_manager::vaultFunction, 
      {MenuState::MainMenu, MenuState::MainMenu, MenuState::MainMenu}, MenuState::MainMenu },
      
    // [Running]
    { runningOpts, GET_SIZE(runningOpts), &menu_manager::runningFunction, 
      {MenuState::MainMenu, MenuState::Finished, MenuState::MainMenu}, MenuState::MainMenu },

    // [RunningBreak]
    { runningOpts, GET_SIZE(runningOpts), &menu_manager::runningFunction, 
      {MenuState::MainMenu, MenuState::Finished, MenuState::MainMenu}, MenuState::MainMenu },
      
    // [Finished]
    { finishedOpts, GET_SIZE(finishedOpts), &menu_manager::finishedFunction, 
      {MenuState::Running, MenuState::MainMenu, MenuState::MainMenu}, MenuState::MainMenu },

    // [VolumeChange]
    { volumeOpts, GET_SIZE(volumeOpts), nullptr, 
      {MenuState::VolumeChange, MenuState::VolumeChange, MenuState::Settings}, MenuState::Settings },

    // [TimerChange] -> Wizard Step 1: Moves straight to Break setup screen
    { nullptr, 0, &menu_manager::timerChangeFunction, 
      {MenuState::BreakTimeChange, MenuState::BreakTimeChange, MenuState::BreakTimeChange}, MenuState::Settings },

    // [BreakTimeChange] -> Wizard Step 2: Saves values and routes clean directly back to Settings menu
    { nullptr, 0, &menu_manager::timerChangeFunction, 
      {MenuState::Settings, MenuState::Settings, MenuState::Settings}, MenuState::Settings },

    // [TimerConfirm] -> Summary overview shown when 'Start' is hit from Main Menu. Routes to active run
    { nullptr, 0, &menu_manager::timerConfirmFunction, 
      {MenuState::Running, MenuState::Running, MenuState::Running}, MenuState::MainMenu }
};

menu_manager::menu_manager(Adafruit_SSD1306 &displayRef) : display(displayRef) {
    // Defaults (Fallback safeguards if settings are ignored)
    configuredWorkSeconds = 1500; // 25 mins
    configuredBreakSeconds = 300; // 5 mins
}

void menu_manager::timerChangeFunction() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    int displayValue = 0;

    if (currentState == MenuState::TimerChange) {
        display.println(F("Step 1: Set Focus"));
        int rawMinutes = map(potentioValue_raw, 0, 1000, 5, 30);
        configuredWorkSeconds = rawMinutes * 60;
        displayValue = rawMinutes;
    } else {
        display.println(F("Step 2: Set Break"));
        int rawMinutes = map(potentioValue_raw, 0, 1000, 1, 10);
        configuredBreakSeconds = rawMinutes * 60;
        displayValue = rawMinutes;
    }

    display.fillRect(30, 20, 68, 24, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setTextSize(2); 
    display.setCursor(38, 24);
    if (displayValue < 10) display.print(F("0"));
    display.print(displayValue);
    display.print(F("m"));

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(16, 54);
    display.print(F("[Btn1: Continue]"));
    display.display();
}

void menu_manager::timerConfirmFunction() {
    display.clearDisplay();
    display.setTextSize(1); 
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 0);
    display.println(F("--- START SESSION ---"));

    int focusMins = configuredWorkSeconds / 60;
    int breakMins = configuredBreakSeconds / 60;

    display.setCursor(4, 20);
    display.print(F("Focus Target: "));
    display.print(focusMins);
    display.print(F(" Min"));

    display.setCursor(4, 36);
    display.print(F("Break Target: "));
    display.print(breakMins);
    display.print(F(" Min"));

    display.setCursor(10, 54);
    display.print(F("[Btn1: Begin Run]"));
    display.display();
}

void menu_manager::init() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Hello, World!"));
    display.display();
}

void menu_manager::nextAction(uint8_t action){
    currentState = MenuState::MainMenu;
}

void menu_manager::startupFunction() {
    const int maxStartupUpdates = 100;
    display.clearDisplay();
        if (coinInstance.id != DisplayAsset::CoinAnim) {
        coinInstance.changeAsset(DisplayAsset::CoinAnim, 42, 24, false);
        }
    startupUpdateCount++;
    drawDisplay(coinInstance);
    display.display();
    if(startupUpdateCount == maxStartupUpdates){
        nextState = MenuState::MainMenu;
    }

}

void menu_manager::mainMenuFunction() {
    menuPositionManager();
    display.clearDisplay();
    
    DisplayAsset activeMainMenuAsset1 = DisplayAsset::PlayIcon;
    DisplayAsset activeMainMenuAsset2 = DisplayAsset::SettingsIcon;
    DisplayAsset activeMainMenuAsset3 = DisplayAsset::VaultIcon;

    uint8_t posX = 48;
    uint8_t posY = 8;

    // 1. Play icon
    if (mainMenuAssetInstance1.id != activeMainMenuAsset1) {
        mainMenuAssetInstance1.changeAsset(activeMainMenuAsset1, posX, posY, false);
    }
    drawDisplay(mainMenuAssetInstance1);

    // 2. Settings Icon
    if (mainMenuAssetInstance2.id != activeMainMenuAsset2) {
        mainMenuAssetInstance2.changeAsset(activeMainMenuAsset2, posX, posY * 3, false);
    }
    drawDisplay(mainMenuAssetInstance2);

    // 3. Vault Icon
    if (mainMenuAssetInstance3.id != activeMainMenuAsset3) {
        mainMenuAssetInstance3.changeAsset(activeMainMenuAsset3, posX, posY * 5, false);
    }
    drawDisplay(mainMenuAssetInstance3);

    // --- MARKERINGSLOGIK ---
    // Räkna ut exakt var den valda ikonen ligger i Y-led
    // Index 0 -> Y: 8, Index 1 -> Y: 24, Index 2 -> Y: 40
    uint8_t selectedY = posY + (menuPosition * posY * 2);

    // Rita en snygg "väljar-pil" (>) till vänster om den valda ikonen
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(posX - 16, selectedY + 4); // Sätter den 16 pixlar till vänster om ikonen
    display.print(F(">"));

    // ELLER: Rita en snygg rektangulär ram runt hela ikonen (om de är t.ex. 16x16 pixlar)
    // display.drawRoundRect(posX - 2, selectedY - 2, 20, 20, 3, SSD1306_WHITE);

    display.display();

}

void menu_manager::settingsFunction() {

    display.clearDisplay();
    display.setTextSize(1); 
    selectionManager();
    display.setCursor(0,0);
    display.setTextColor(SSD1306_WHITE);
    display.println(F("Settings"));
    display.display();
}
//Denna räknar ner. Nuvarande placeholder för att hålla tid.
void menu_manager::runningFunction() {
    //Basic information that will keep the timer going
    display.clearDisplay();

    fishSwim();

    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);



    //Actual running timer
    unsigned long currentMillis = millis();
    if (currentMillis - timerLastMillis >= 1000) {
        timerLastMillis = currentMillis;
        
        if (remainingSeconds > 0) {
            remainingSeconds--;
        } else {
            if (currentState == MenuState::Running) {
                // Work timer finished -> pull custom break time parameters into action
                remainingSeconds = configuredBreakSeconds; 
                timerLastMillis = millis();
                nextState = MenuState::RunningBreak;
            } else {
                nextState = MenuState::Finished;
            }
            return;
        }
    }

    int displayMinutes = remainingSeconds / 60;
    int displaySeconds = remainingSeconds % 60;

    display.setTextSize(1);
    display.setCursor(45, 0);
    if (displayMinutes < 10) display.print(F("0"));
    display.print(displayMinutes);
    display.print(F(":"));
    if (displaySeconds < 10) display.print(F("0"));
    display.print(displaySeconds);
    //----------------------------

    //Personalized: FIXA DENNA
    if (runningBigL.id != activeRunningBigL) {
    runningBigL.changeAsset(activeRunningBigL, 0, 0, false); //[cite: 5, 6]
    }
    drawDisplay(runningBigL);

    //Personalized: FIXA DENNA
    if (runningBigR.id != activeRunningBigR) {
    runningBigR.changeAsset(activeRunningBigR, 112, 0, false); //[cite: 5, 6]
    }
    drawDisplay(runningBigR);

        if (runningLilR.id != activeRunningLilR) {
    runningLilR.changeAsset(activeRunningLilR, 96, 0, false); //[cite: 5, 6]
    }
    drawDisplay(runningLilR);

        if (runningLilL.id != activeRunningLilL) {
    runningLilL.changeAsset(activeRunningLilL, 16, 0, false); //[cite: 5, 6]
    }
    drawDisplay(runningLilL);

    display.display();
}

void menu_manager::finishedFunction() {
    display.clearDisplay();
    display.setTextSize(1); 
    selectionManager();
    display.setCursor(0,0);
    display.setTextColor(SSD1306_WHITE);
    display.println(F("Finished"));
    display.display();
}
//Placeholder

void menu_manager::vaultFunction() {
    display.clearDisplay(); //[cite: 5]

    // 1. Map the potentiometer to choose between your 5 underwater assets[cite: 5, 7]
    int choice = map(potentioValue_raw, 0, 1024, 0, 3); //[cite: 5]
    DisplayAsset activeVaultAsset;

    switch(choice) {
        case 0:  activeVaultAsset = DisplayAsset::Geyser;   break;
        case 1:  activeVaultAsset = DisplayAsset::Kelp;    break;
        case 2:  activeVaultAsset = DisplayAsset::Seaweed;  break;
    }

    // 2. Manage the dedicated vault asset instance instead of coinInstance
    if (vaultAssetInstance.id != activeVaultAsset) {
        vaultAssetInstance.changeAsset(activeVaultAsset, 0, 0, false); //[cite: 5, 6]
    }

    // 3. Draw the vault-specific asset to the display buffer[cite: 5]
    drawDisplay(vaultAssetInstance); //[cite: 5]

    // 4. Print your coin text on the right side of the screen[cite: 5]
    display.setTextSize(1); //[cite: 5]
    display.setTextColor(SSD1306_WHITE); //[cite: 5]
    display.setCursor(32, 28); // Shipped right to avoid overlapping the 16px wide asset[cite: 5]
    display.print(F("x 150 Coins")); //[cite: 5]
    
    display.display(); //[cite: 5]
}

//Håller koll på positionen. Antagligen kommer denna tags bort för custom istället.
void menu_manager::menuPositionManager() {
    display.setTextSize(1); 
    size_t stateIdx = static_cast<size_t>(currentState);
    const char** menuOptions = stateTable[stateIdx].options;
    size_t numOptions = stateTable[stateIdx].numOptions;
    if (menuOptions == nullptr || numOptions == 0) return;

    int radius = 24; 
    int centerX = 64;
    int centerY = 55; 
    const int charWidth = 6;
    const int charHeight = 7;

    int currentSelection = map(potentioValue_raw, 0, 1024, 0, numOptions);
    if (currentSelection >= (int)numOptions) currentSelection = numOptions - 1; 
    
    menuPosition = currentSelection;

    for(size_t i = 0; i < numOptions; i++) {
        if (i == (size_t)currentSelection) {
            display.fillRect(centerX + radius * cos((180.0 + (i * (180.0 / ((numOptions > 1) ? (numOptions - 1) : 1)))) * (PI / 180.0)) - ((strlen(menuOptions[i]) * charWidth) / 2) - 2, centerY + radius * sin((180.0 + (i * (180.0 / ((numOptions > 1) ? (numOptions - 1) : 1)))) * (PI / 180.0)) - (charHeight / 2) - 2, (strlen(menuOptions[i]) * charWidth) + 4, charHeight + 4, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); 
        } else {
            display.setTextColor(SSD1306_WHITE, SSD1306_BLACK); 
        }

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
    menuPositionManager();
}


//Vanlig submeny för olika delar av systemet.
void menu_manager::genericSubmenuFunction(const char* label) {
    display.clearDisplay();
    display.setTextSize(1); 
    selectionManager();
    display.setCursor(0,0);
    display.setTextColor(SSD1306_WHITE);
    display.println(label);
    display.display();
}


//Hur ofta det uppdateras
void menu_manager::update(unsigned long currentMillis, int potVal) {
    potentioValue_raw = potVal;
    currentState = nextState;

    size_t stateIdx = static_cast<size_t>(currentState);
    if (stateIdx >= static_cast<size_t>(MenuState::Count)) return;

    if (stateTable[stateIdx].renderFunc != nullptr) {
        (this->*stateTable[stateIdx].renderFunc)();
    } else {
        if (currentState == MenuState::VolumeChange) genericSubmenuFunction("Volume");
    }
}


//Det som händer när man trycker på en knapp
void menu_manager::buttonAction(uint8_t button) {
    size_t stateIdx = static_cast<size_t>(currentState);
    
    if (currentState == MenuState::Startup) return;

    // --- INTERCEPT INPUT ACTIONS FOR DYNAMIC CLOCK WIZARDS ---
    if (currentState == MenuState::TimerChange || currentState == MenuState::BreakTimeChange) {
        if (button == 1) {
            nextState = stateTable[stateIdx].button1Targets[0];
        } 
        else if (button == 2) {
            nextState = MenuState::Settings;
        }
        return; 
    }

    // --- INTERCEPT CONFIRMATION ACTION TO CAPTURE RUNTIME PARAMS ---
    if (currentState == MenuState::TimerConfirm) {
        if (button == 1) {
            // Button 1 confirms! Lock custom user values directly into the active tracking register fields
            remainingSeconds = configuredWorkSeconds; 
            timerLastMillis = millis();
            nextState = MenuState::Running; // Go to focus countdown loop
        }
        else if (button == 2) {
            nextState = MenuState::MainMenu; // Cancel out to home screen
        }
        return;
    }

    // --- GENERIC RADIAL MENU CONTROLS ENGINE ---
    //Väljer index för menyn. Detta är rätt snyggt ngl
    if (button == 1) {
        if (menuPosition >= 0 && menuPosition < 3) {
            nextState = stateTable[stateIdx].button1Targets[menuPosition];
        }
    } 
    else if (button == 2) {
        nextState = stateTable[stateIdx].button2Target;
    }
}

// --- FUNKTION FÖR ATT ÄNDRA EN EXISTERANDE FISK ---
void menu_manager::setFish(int index, DisplayAsset asset, float density, float height) {
    if (index < 0 || index >= MAX_FISHES) return;

    aquarium[index].assetId = asset;
    aquarium[index].waveDensity = density;
    aquarium[index].waveHeight = height;
    
    // Standard-delays som funkar bra för alla
    aquarium[index].minSpawnDelay = 3000;
    aquarium[index].maxSpawnDelay = 10000;
}
void menu_manager::fishSwim() {
    unsigned long currentMillis = millis();
    randomSeed(analogRead(A1));

    // Sätt standard-fiskar allra första gången om arrayen är helt tom
    if (!aquariumInitialized) {
        setFish(0, DisplayAsset::AnglerFish,  0.12, 4.0);
        setFish(1, DisplayAsset::ClassicFish, 0.08, 5.0);
        setFish(2, DisplayAsset::FrenchFish,  0.20, 3.0);
        
        // --- RESPEKTERADE DELAYS ---
        // Justera dessa fritt. 5-10 sekunder ger ett fint, naturligt flöde.
        aquarium[0].minSpawnDelay = 4000; aquarium[0].maxSpawnDelay = 10000;  
        aquarium[1].minSpawnDelay = 5000; aquarium[1].maxSpawnDelay = 8000;  
        aquarium[2].minSpawnDelay = 3000; aquarium[2].maxSpawnDelay = 9000;  

        for(int i = 0; i < MAX_FISHES; i++) {
            // Sätt till ett djupt viloläge så de inte tjuvstartar
            aquarium[i].x = -100; 
            aquarium[i].speed = 1;
            
            // Sprid ut första startskottet i tid
            aquarium[i].nextSpawnTime = currentMillis + (i * 2000);
            aquarium[i].baseY = 10 + (i * 10); 
        }
        aquariumInitialized = true;
    }

    // Loopa igenom alla fasta slots
    for (int i = 0; i < MAX_FISHES; i++) {
        
        // FIX: Ändrat villkor. Om fisken är i viloläge (-100), vänta ut timern
        if (aquarium[i].x == -100) {
            if (currentMillis >= aquarium[i].nextSpawnTime) {
                aquarium[i].facingRight = (random(0, 2) == 1);
                aquarium[i].speed = 1;

                // SEPARATION PÅ OLIKA NIVÅER
                if (i == 0)      aquarium[i].baseY = random(6, 16);   
                else if (i == 1) aquarium[i].baseY = random(20, 26);  
                else if (i == 2) aquarium[i].baseY = random(34, 40);  


                if (aquarium[i].facingRight) aquarium[i].x = -24;
                else aquarium[i].x = 128;
            }
        } else {
            // RÖRELSE
            if (aquarium[i].facingRight) aquarium[i].x += aquarium[i].speed;
            else aquarium[i].x -= aquarium[i].speed;

            // FIX: När fisken simmar ut, kasta in den i viloläget (-100) 
            // och sätt den FRAMTIDA spawntiden härifrån!
            if (aquarium[i].x < -24 || aquarium[i].x > 128) {
                aquarium[i].x = -100; 
                aquarium[i].nextSpawnTime = currentMillis + random(aquarium[i].minSpawnDelay, aquarium[i].maxSpawnDelay);
            }
        }

        // RENDERING (Rita bara om den faktiskt är på skärmen)
        if (aquarium[i].x >= -24 && aquarium[i].x <= 128) {
            int calcY = aquarium[i].baseY + (sin(aquarium[i].x * aquarium[i].waveDensity) * aquarium[i].waveHeight);
            
            if (calcY < 0) calcY = 0;
            if (calcY > 44) calcY = 44;

            if (aquarium[i].instance.id != aquarium[i].assetId || 
                aquarium[i].instance.x != aquarium[i].x || 
                aquarium[i].instance.y != calcY || 
                aquarium[i].instance.mirror != !aquarium[i].facingRight) 
            {
                aquarium[i].instance.changeAsset(aquarium[i].assetId, aquarium[i].x, calcY, !aquarium[i].facingRight);
            }
            drawDisplay(aquarium[i].instance);
        }
    }
}

//Animation or Images
void menu_manager::drawDisplay(AnimationInstance& instance) { //
    // 1. Hämta asset-detaljer
    AssetData asset = BitmapBank::getAsset(instance.id); //
    if (asset.frames == nullptr || asset.totalFrames == 0) return; //

    // 2. Hantera animeringstimer (om det finns flera frames)
    if (asset.fps > 0) { //
        unsigned long frameInterval = 1000 / asset.fps; //
        unsigned long currentMillis = millis(); //

        if (currentMillis - instance.lastFrameUpdateTime >= frameInterval) { //
            instance.lastFrameUpdateTime = currentMillis; //
            instance.currentFrameIndex = (instance.currentFrameIndex + 1) % asset.totalFrames; //
        }
    } else {
        instance.currentFrameIndex = 0; //
    }

    // Hämta pekaren till nuvarande frame
    const unsigned char* frameToDraw = (const unsigned char*)pgm_read_ptr(&(asset.frames[instance.currentFrameIndex])); //
    
    // 3. RENDERINGS-LOGIK MED ÄKTA SPEGELVÄNDNING
    if (instance.mirror) {
        // Om mirror är true, ritar vi bitmappen pixel för pixel, men speglar X-axeln
        int byteWidth = (asset.width + 7) / 8; // Hur många bytes per rad i bitmappen
        
        for (int y = 0; y < asset.height; y++) { //
            for (int x = 0; x < asset.width; x++) { //
                // Räkna ut exakt vilken bit i PROGMEM vi läser just nu
                int byteIdx = y * byteWidth + (x / 8);
                uint8_t byteVal = pgm_read_byte(frameToDraw + byteIdx); //
                
                // Kolla om pixeln är tänd (1) eller släckt (0)
                if (byteVal & (128 >> (x % 8))) {
                    // SPEGELVÄNDNINGEN: Rita pixeln på (bredd - 1 - x) istället för x!
                    int mirroredX = instance.x + (asset.width - 1 - x);
                    display.drawPixel(mirroredX, instance.y + y, SSD1306_WHITE); //
                }
            }
        }
    } else {
        // Om mirror är false, använd Adafruits snabba standardfunktion
        // Genom att lägga till SSD1306_BLACK blir tillgången solid och döljer allt bakom sig!
        display.drawBitmap(instance.x, instance.y, frameToDraw, asset.width, asset.height, SSD1306_WHITE); //
    }
}