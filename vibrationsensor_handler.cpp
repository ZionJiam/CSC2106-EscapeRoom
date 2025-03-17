#include "vibrationsensor_handler.h"
#include <M5StickCPlus.h>

void handleVibrationSensor(String message) {
    if (message == "BUZZING") {
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(0, 10);
        M5.Lcd.println("Buzz!");
        blinkLEDAndSound();
    }
}

void blinkLEDAndSound() {
  for (int i = 0; i < 10; i++) {  // Blink LED for 5 seconds (10 blinks at 500ms each)
    M5.Beep.tone(2000, 3000);
    digitalWrite(10, LOW);
    delay(250);
    digitalWrite(10, HIGH);
    delay(250);
  }
  M5.Beep.mute();  // Properly stop the beep
}