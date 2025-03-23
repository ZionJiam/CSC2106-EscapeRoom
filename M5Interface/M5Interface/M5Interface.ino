#include <M5StickCPlus.h>

#define LED_PIN 10 // Define LED pin

int screenState = 0; // 0: Main, 1: Menu, 2: Theme Selection, 3: LED Control
int menuIndex = 0;
int themeIndex = 0;
int ledState = LOW;
const char* themes[] = {"Zombie Apocalypse", "Spaceship", "Mysterious"};
const char* currentTheme = "Zombie Apocalypse";

void drawMainScreen() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 20);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("\nTheme: " + String(currentTheme));
    M5.Lcd.println("Area : B site");
    M5.Lcd.println("Stage : 3/5");
    M5.Lcd.println("\nMenu (press big btn)");
}

void drawMenuScreen() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 20);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println(menuIndex == 0 ? "> Select Theme" : "  Select Theme");
    M5.Lcd.println(menuIndex == 1 ? "> On/Off LED" : "  On/Off LED");
    M5.Lcd.println(menuIndex == 2 ? "> Back" : "  Back");
}

void drawThemeScreen() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 20);
    M5.Lcd.setTextSize(2);
    for (int i = 0; i < 3; i++) {
        if (i == themeIndex) {
            M5.Lcd.print("> ");
        } else {
            M5.Lcd.print("  ");
        }
        M5.Lcd.println(themes[i]);
    }
    M5.Lcd.println(themeIndex == 3 ? "> Back" : "  Back");
}

void drawLEDControlScreen() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 20);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println(menuIndex == 0 ? "> On" : "  On");
    M5.Lcd.println(menuIndex == 1 ? "> Off" : "  Off");
    M5.Lcd.println(menuIndex == 2 ? "> Back" : "  Back");
}

void setup() {
    M5.begin();
    M5.Lcd.setRotation(3);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, ledState);
    drawMainScreen();
}

void loop() {
    M5.update();

    if (screenState == 0) { // Main Screen
        if (M5.BtnA.wasPressed()) {
            screenState = 1;
            menuIndex = 0;
            drawMenuScreen();
        }
    }
    else if (screenState == 1) { // Menu Screen
        if (M5.BtnB.wasPressed()) { // Toggle between options
            menuIndex = (menuIndex + 1) % 3;
            drawMenuScreen();
        }
        if (M5.BtnA.wasPressed()) { // Select
            if (menuIndex == 0) {
                screenState = 2;
                themeIndex = 0;
                drawThemeScreen();
            } else if (menuIndex == 1) {
                screenState = 3;
                menuIndex = 0;
                drawLEDControlScreen();
            } else {
                screenState = 0;
                drawMainScreen();
            }
        }
    }
    else if (screenState == 2) { // Theme Selection Screen
        if (M5.BtnB.wasPressed()) { // Toggle between themes
            themeIndex = (themeIndex + 1) % 4;
            drawThemeScreen();
        }
        if (M5.BtnA.wasPressed()) { // Select
            if (themeIndex == 3) {
                screenState = 1;
                drawMenuScreen();
            } else {
                currentTheme = themes[themeIndex];
                screenState = 0;
                drawMainScreen();
            }
        }
    }
    else if (screenState == 3) { // LED Control Screen
        if (M5.BtnB.wasPressed()) { // Toggle between options
            menuIndex = (menuIndex + 1) % 3;
            drawLEDControlScreen();
        }
        if (M5.BtnA.wasPressed()) { // Select
            if (menuIndex == 0) {
                ledState = LOW;
                digitalWrite(LED_PIN, ledState);
                screenState = 0;
                drawMainScreen();
            } else if (menuIndex == 1) {
                ledState = HIGH;
                digitalWrite(LED_PIN, ledState);
                screenState = 0;
                drawMainScreen();
            } else {
                screenState = 1;
                drawMenuScreen();
            }
        }
    }
}