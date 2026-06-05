#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "mbed.h"
//Skip timers for this setup.
//My Libraries
#include "button_reader.h"
#include "menu_manager.h"


//Komponenterna

//Knappar:
int confirmButtonPin = 12;
int backButtonPin = 11;
int potVal = 0;

//OLED:
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C //Screen Address!!!!!

//Annat:
unsigned long previousMillis = 0;
const unsigned long interval = 1000; // Interval time in milliseconds (1 second)


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
button_reader confirmButton(confirmButtonPin);
button_reader backButton(backButtonPin);
menu_manager menu(display);

//Interrupts for buttons:
void confirmISR() {
    confirmButton.handleInterrupt(); // Redirects the hardware alert to the object
    menu.buttonAction(1);

}

void backISR() {
    backButton.handleInterrupt();    // Redirects the hardware alert to the object
    menu.buttonAction(2);

}

//Piezo:
int piezoPin = 2;

//Potentiometer:
int potentiometerPin = A0;

void setup() {
  Serial.begin(9600);
  confirmButton.init(confirmISR);
  backButton.init(backISR);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); 
    }
      menu.init();
}


void loop() {
    unsigned long currentMillis = millis();
    menu.update(currentMillis);
}