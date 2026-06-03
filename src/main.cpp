#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Komponenterna

//Knappar:
int confirmButton = 12;
int backButton = 11;

//OLED:
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Piezo:
int piezoPin = 2;

//Potentiometer:
int potentiometerPin = A0;

void setup() {
  Serial.begin(9600);

  

}

void loop() {

  //tone(piezoPin,4000,20);
  //delay(10);
}
