#include "zombie_handler.h"
#include "mqtt_handler.h"  // if client is declared there
#include <M5StickCPlus.h>
#include "BLEAdvertiser.h"
#include <ArduinoJson.h>


String enteredSequence = "";
const String correctSequence = "1234";

// For BLE Adverising for Position
BLEAdvertiser bleAdvertiser;

bool powerActivated = false;
bool isHacked = false;
bool isLogin = false;

// String playerNumber = "player1";
// String playerHackArea = "Corner D";
// String playerArea = "";

String playerNumber = "player2";
String playerHackArea = "Corner A";
String playerArea = "";

unsigned long lastPressTime = 0;  // Store timestamp for Player 2





// Handle any zombie topic messages received 
void handleZombieMQTT(String topic, String message) {

  if (message == "SHOW") {
        resetValuesAfterCompletion();
        showEndScreen();  // 🎬 trigger immersive final screen
    }

    if (topic == "m5stick/zombie/hack_result") {
        if (message == "success") {
            currentZombieStage = STAGE_COMMUNICATION;
            client.publish("m5stick/zombie/stage/security", "completed");
            showHackingSuccessDisplay();
        } else {
            showHackingFailureDisplay();
        }
        displayZombieStage();
    } 
    else if (topic == "m5stick/" + String(playerNumber) + "position") {
        if (currentZombieStage == STAGE_SECURITY) {
            extractUserAreaFromJSON(message);
            //testUserArea(message.c_str());
            displayZombieStage();
        }
    } 
    else if (topic == "m5stick/zombie/shake") {
        if (currentZombieStage != STAGE_POWER) return; // Ignore early input
        activatePowerSystem();
    } 
    else if (topic == "m5stick/zombie/cube") {
        if (currentZombieStage != STAGE_COMMUNICATION) return; // Ignore early input
        loginCommunicationSystem(message);
    }
}

// Display the current Zombie Stage
void displayZombieStage() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);

  switch (currentZombieStage) {
    case STAGE_INIT:
      initZombieStage();
      break;
    case STAGE_POWER:
      M5.Lcd.println("Task: Activate Power System");
      break;
    case STAGE_SECURITY:
      M5.Lcd.println("Task: Hack the Security System");
      M5.Lcd.setCursor(0, 30);       // Move cursor down for spacing
      M5.Lcd.println(playerNumber + ": " + playerArea);  // Display the extracted area on screen
      // Change this for each Player
      break;

    case STAGE_COMMUNICATION:
      M5.Lcd.println("Task: Login to Communications System");

      break;
    case STAGE_COMPLETE:
      M5.Lcd.println("Mission Complete!");
      break;
    default:
      M5.Lcd.println("Error in Stage...");
      break;
  }
}

void initZombieStage(){
    bleAdvertiser.begin("Player2");
    M5.Lcd.println("Initializing Task");
    delay(2000);
    currentZombieStage = STAGE_POWER;
    displayZombieStage();
}

void activatePowerSystem() {
  handleVibrationSensor();
  currentZombieStage = STAGE_SECURITY;
  if(powerActivated == true){
    displayZombieStage();
  }
  client.publish("m5stick/zombie/stage/power", "completed");
}

void extractUserAreaFromJSON(String message){
    // Create a DynamicJsonDocument to store the parsed data
  DynamicJsonDocument doc(1024);  // You can adjust the size as needed

  // Parse the incoming JSON message
  DeserializationError error = deserializeJson(doc, message);

    // Check for JSON parsing errors
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.f_str());
    return;
  }

  // Get the "area" from the parsed JSON
  const char* area = doc["area"];  // Extract the area code

  playerArea = String(area);
}

void testUserArea(String message){
   
  playerArea = message;
}

void hackSecuritySystem(){
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


    if (currentZombieStage == STAGE_SECURITY && M5.BtnA.wasPressed()) {
      //Player is corner A
      if(playerArea == playerHackArea){        
            String payload = "{ \"player\": \"" + String(playerNumber) + "\" }";
            client.publish("m5stick/zombie/hack_attempt", payload.c_str());


          //client.publish("m5stick/zombie/${playerNumber2}/hacked", "true");
      }
      showHackingInProgressDisplay();
      // ADD automation to check when both paler
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
  isHacked = false;
  isLogin = false;
  enteredSequence = "";
  playerArea = "";
  currentZombieStage = STAGE_INIT;
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

void showHackingInProgressDisplay() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(RED, BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 20);
  M5.Lcd.println("HACKING");

  // Flashing "IN PROGRESS" with color change
  M5.Lcd.setTextSize(3);
  for (int i = 0; i < 6; i++) {
    M5.Lcd.setTextColor(CYAN, BLACK);
    M5.Lcd.setCursor(10, 60);
    M5.Lcd.println("IN PROGRESS");
    delay(400);
    M5.Lcd.fillScreen(BLACK);  // Clear the screen for flashing effect
    delay(400);
  }

  // Cool animation of scrolling text
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(MAGENTA, BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(0, 40);
  String animationText = ">>> HACKING INTO MAINFRAME... <<<";
  for (int i = 0; i < animationText.length(); i++) {
    M5.Lcd.print(animationText.charAt(i));
    delay(100);  // Slow scrolling effect
  }
}

void showHackingSuccessDisplay(){
      // Final cool flashing message with beeps
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setTextColor(RED, BLACK);
    M5.Lcd.setCursor(10, 50);
    M5.Lcd.println("SYSTEM BREACHED");
    delay(500);
    M5.Lcd.fillScreen(BLACK);
    delay(500);
    M5.Lcd.setTextColor(GREEN, BLACK);
    M5.Lcd.println("ACCESS GRANTED!");
    
    // Beep effects to enhance excitement
    M5.Beep.tone(1000, 150);
    delay(100);
    M5.Beep.tone(1500, 150);
    delay(100);
    M5.Beep.tone(2000, 200);
    M5.Beep.mute();
}

void showHackingFailureDisplay(){
      // Pause for a moment before showing failure message
    delay(1000);
    M5.Lcd.fillScreen(BLACK);

    // Flashing "HACKING FAILED" message to simulate failure
    M5.Lcd.setTextColor(RED, BLACK);
    M5.Lcd.setTextSize(3);
    for (int i = 0; i < 6; i++) {
      M5.Lcd.setCursor(10, 50);
      M5.Lcd.println("HACKING FAILED");
      delay(500);
      M5.Lcd.fillScreen(BLACK);  // Clear screen to create a flashing effect
      delay(500);
    }

    // Display failure sound for dramatic effect
    M5.Beep.tone(500, 300);  // Low beep for failure
    delay(300);
    M5.Beep.tone(600, 300);  // Higher pitch beep
    delay(300);
    M5.Beep.mute();
    
    // Final "ERROR" message to indicate complete failure
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(RED, BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10, 80);
    M5.Lcd.println("ERROR!");
    delay(1500);  // Hold the final error message for a bit

    // Optionally, reset the screen or go back to a previous state
    M5.Lcd.fillScreen(BLACK);
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





