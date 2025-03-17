#include "cube_handler.h"
#include <M5StickCPlus.h>

String enteredSequence = "";
const String correctSequence = "1234";

void resetSequence() {
    enteredSequence = "";
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 10);
    M5.Lcd.println("Sequence Reset");
}

void checkPuzzle() {
    if (enteredSequence == correctSequence) {
        M5.Lcd.fillScreen(GREEN);
        M5.Lcd.setCursor(0, 10);
        M5.Lcd.println("ACCESS GRANTED!");
    } else {
        M5.Lcd.fillScreen(RED);
        M5.Lcd.setCursor(0, 10);
        M5.Lcd.println("ACCESS DENIED!");
    }

    // Update when game logic implements
    delay(3000);
    resetSequence();
}

void handleCubeInput(String message) {
    int faceUp = message.toInt();
    enteredSequence += String(faceUp);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 10);
    M5.Lcd.printf("Entered: %s", enteredSequence.c_str());

    if (enteredSequence.length() == 4) {
        checkPuzzle();
    }
}
