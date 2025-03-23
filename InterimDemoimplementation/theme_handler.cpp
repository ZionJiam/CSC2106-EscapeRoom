#include <M5StickCPlus.h>
#include "theme_handler.h"
#include "mqtt_handler.h"
#include "theme_type.h"
#include "zombie_stage_state.h"
#include "zombie_handler.h"


EscapeRoomTheme currentTheme = THEME_NONE;
bool themeLocked = false;

static int selectedIndex = 0;
static unsigned long buttonAStartTime = 0;
static bool buttonAHeld = false;

String themeNames[] = { "Zombie", "Haunted", "Prison" };

void displayThemeSelection() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.println("Select Theme:");
  for (int i = 0; i < 3; i++) {
    if (i == selectedIndex) {
      M5.Lcd.print("> ");
    } else {
      M5.Lcd.print("  ");
    }
    M5.Lcd.println(themeNames[i]);
  }
}

void handleThemeSelection() {
  if (!themeLocked) {
    if (M5.BtnB.wasPressed()) {
      selectedIndex = (selectedIndex + 1) % 3;
      displayThemeSelection();
      delay(150); // debounce
    }

    if (M5.BtnA.wasPressed()) {
      currentTheme = (EscapeRoomTheme)selectedIndex;
      themeLocked = true;
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(0, 10);
      M5.Lcd.printf("Selected: %s\n", themeNames[selectedIndex].c_str());
      setupMQTT(currentTheme);
      delay(1000);

      switch (currentTheme) {
        case THEME_ZOMBIE:
          displayZombieStage();
          break;

        case THEME_HAUNTED:
          break;

        case THEME_PRISON:
          break;
      }
    }
  }
}

void handleThemeReset() {
  if (M5.BtnA.isPressed()) {
    if (!buttonAHeld) {
      buttonAStartTime = millis();
      buttonAHeld = true;
    } else if (millis() - buttonAStartTime > 3000 && themeLocked) {
      themeLocked = false;
      currentTheme = THEME_NONE;
      selectedIndex = 0;
      M5.Lcd.fillScreen(ORANGE);
      M5.Lcd.setCursor(0, 10);
      M5.Lcd.println("Theme Reset!");
      delay(1500);
      displayThemeSelection();
    }
  } else {
    buttonAHeld = false;
  }
}
