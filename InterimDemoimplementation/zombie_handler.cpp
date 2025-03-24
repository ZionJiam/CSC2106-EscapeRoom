#include "zombie_handler.h"
#include "mqtt_handler.h"  // if client is declared there
#include <M5StickCPlus.h>

String enteredSequence = "";
const String correctSequence = "1234";

bool powerActivated = false;
bool isLogin = false;

void handleZombieMQTT(String topic, String message) {
  if (message == "SHOW") {
    resetValuesAfterCompletion();
    showEndScreen();  // 🎬 trigger immersive final screen
  }

  if (topic == "m5stick/zombie/shake") {
      if (currentZombieStage != STAGE_POWER) return; // Ignore early input
    activatePowerSystem();
  } else if (topic == "m5stick/zombie/cube") {
      if (currentZombieStage != STAGE_COMMUNICATION) return; // Ignore early input
    loginCommunicationSystem(message);
  }
}

void displayZombieStage() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  switch (currentZombieStage) {
    case STAGE_POWER:
      M5.Lcd.println("Task: Activate Power System");
      break;
    case STAGE_COMMUNICATION:
      M5.Lcd.println("Task: Login to Comm System");
      break;
    case STAGE_COMPLETE:
      M5.Lcd.println("Mission Complete!");
      break;
    default:
      M5.Lcd.println("Initializing...");
      break;
  }
}

void activatePowerSystem() {
  handleVibrationSensor();
  currentZombieStage = STAGE_COMMUNICATION;
  if(powerActivated == true){
    displayZombieStage();
  }
  client.publish("m5stick/zombie/stage/power", "completed");
}

void loginCommunicationSystem(String message){
    handleCubeInput(message);
    if(isLogin == true){
        client.publish("m5stick/zombie/stage/communication", "completed");
        client.publish("m5stick/zombie/stage/all", "completed");
    }
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


void deleteLastPasswordChar() {
    if (enteredSequence.length() > 0) {
        enteredSequence.remove(enteredSequence.length() - 1);
        M5.Beep.tone(600, 100); // Optional: feedback sound
        delay(100);
    }

    // Refresh display
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 10);
    M5.Lcd.printf("Password: %s", enteredSequence.c_str());
}


void loopZombieButtonListener() {

    if (currentZombieStage == STAGE_COMMUNICATION && M5.BtnB.wasPressed()) {
      M5.Lcd.fillScreen(GREEN);
        deleteLastPasswordChar();
    }
}

void checkPuzzle() {
    if (enteredSequence == correctSequence) {
        M5.Lcd.fillScreen(GREEN);
        M5.Lcd.setCursor(0, 10);
        M5.Lcd.println("ACCESS GRANTED!");
        showLoginSuccessDisplay();
        isLogin = true;
    } else {
        M5.Lcd.fillScreen(RED);
        M5.Lcd.setCursor(0, 10);
        M5.Lcd.println("ACCESS DENIED!");
        delay(2000);
        enteredSequence = "";
        displayZombieStage();
    }

    // Update when game logic implements
    delay(3000);
}


void handleVibrationSensor() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 10);
    M5.Lcd.println("System Powered Up!");
    showPowerActivationDisplay();
    powerActivated = true;
}

void resetValuesAfterCompletion(){
  powerActivated = false;
  isLogin = false;
  enteredSequence = "";
  currentZombieStage = STAGE_POWER;
}

void showPowerActivationDisplay() {
  M5.Lcd.fillScreen(BLACK);

  // Draw outer frame
  M5.Lcd.drawRect(5, 5, 150, 118, ORANGE);
  M5.Lcd.setTextColor(ORANGE, BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(15, 15);
  M5.Lcd.println("POWER SYS");

  // Draw status box
  M5.Lcd.drawRect(10, 40, 140, 30, YELLOW);
  M5.Lcd.setTextColor(YELLOW, BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(15, 45);
  M5.Lcd.println("> BOOT SEQUENCE...");
  delay(500);
  M5.Lcd.setCursor(15, 60);
  M5.Lcd.println("> CALIBRATING RELAYS");
  delay(2000);

  // Cool startup tones
  M5.Beep.tone(800, 150);
  delay(150);
  M5.Beep.tone(1000, 200);
  delay(200);
  M5.Beep.tone(1200, 250);
  M5.Beep.mute();

  // Final confirmation
  delay(2000);
  digitalWrite(10, LOW);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(GREENYELLOW, BLACK);
  M5.Lcd.setCursor(15, 85);
  M5.Lcd.println("SYSTEM READY");

  delay(3000);

  digitalWrite(10, HIGH);

}


void showLoginSuccessDisplay() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(GREEN, BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 20);
  M5.Lcd.println("LOGIN");

  M5.Beep.tone(880, 100);
  delay(200);
  M5.Beep.tone(988, 100);
  delay(200);
  M5.Beep.mute();

  M5.Lcd.setCursor(10, 50);
  M5.Lcd.println("SUCCESSFUL");
  delay(800);

  M5.Lcd.fillScreen(BLACK);
  delay(200);

  // 🔳 Draw outer border
  M5.Lcd.drawRect(5, 5, 175, 118, GREEN);  // x, y, width, height, color

  // 🔳 Draw inner status box
  M5.Lcd.drawRect(10, 20, 140, 50, CYAN);

  // Communication System Online
  M5.Lcd.setTextColor(CYAN, BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(15, 25);
  M5.Lcd.println("> INIT SYSTEM...");
  delay(400);
  M5.Lcd.setCursor(15, 40);
  M5.Lcd.println("> CONNECTING TO COMMS");
  delay(500);
  M5.Lcd.setCursor(15, 55);
  M5.Lcd.println("> LINK ESTABLISHED");
  delay(500);

  // Highlighted "ONLINE" section
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(GREENYELLOW, BLACK);
  M5.Lcd.setCursor(15, 80);
  M5.Lcd.println("COMM SYSTEM");
  M5.Lcd.setCursor(15, 105);
  M5.Lcd.println("ONLINE");

  M5.Beep.tone(1175, 150);
  delay(200);
  M5.Beep.tone(1400, 200);
  M5.Beep.mute();
}

void showEndScreen() {
  M5.Lcd.fillScreen(BLACK);

  // Border
  M5.Lcd.drawRect(5, 5, 175, 118, GREEN);

  // Header
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(GREEN, BLACK);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println(">>> TRANSMISSION SENT <<<");

  // Body
  M5.Lcd.setCursor(15, 35);
  M5.Lcd.println("EVAC SIGNAL RECEIVED");
  delay(400);
  M5.Lcd.setCursor(15, 50);
  M5.Lcd.println("STAND BY FOR EXTRACTION");
  delay(600);
  M5.Lcd.setCursor(15, 65);
  M5.Lcd.println("TIME UNTIL ARRIVAL: 04:00");

  // Footer
  M5.Lcd.setCursor(15, 90);
  M5.Lcd.println("STAY QUIET.");
  M5.Lcd.setCursor(15, 105);
  M5.Lcd.println("STAY ALIVE.");

  // Confirmation beep
  M5.Beep.tone(1000, 100);
  delay(200);
  M5.Beep.tone(1400, 100);
  M5.Beep.mute();
}





