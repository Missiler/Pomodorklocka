#ifndef BITMAP_BANK_H
#define BITMAP_BANK_H

#include <Arduino.h>

enum class DisplayAsset {
    TitlePage,
    Settings,
    LoadingAnim,
    HeartIcon,    // New small static asset
    CoinAnim,      // New small animated asset
    Geyser,
    Kelp,              
    Seaweed,
    PlayIcon,
    SettingsIcon,
    VaultIcon,
    AnglerFish,
    FrenchFish,
    ClassicFish,      
};

struct AssetData {
    const unsigned char* const* frames; 
    int totalFrames;
    int fps; 
    int width;   // Added: Width of this specific asset
    int height;  // Added: Height of this specific asset
};

class BitmapBank {
public:
    static AssetData getAsset(DisplayAsset id);
    // Global max bounds can stay if needed for clearing screens, but are no longer forced
    static const int MAX_WIDTH = 128;
    static const int MAX_HEIGHT = 64;
};

#endif