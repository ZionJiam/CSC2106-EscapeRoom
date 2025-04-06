#ifndef ZOMBIE_HANDLER_H
#define ZOMBIE_HANDLER_H

#include <Arduino.h>
#include "zombie_stage_state.h"


void handleZombieMQTT(String topic, String message);

void displayZombieStage();
void initZombieStage();
void activatePowerSystem();
void loginCommunicationSystem(String message);
void loopZombieButtonListener();

// For the Cube
void handleCubeInput(String message);
void resetSequence();
void deleteLastPasswordChar();
void checkPuzzle();

// For the Vibration Sensor
void handleVibrationSensor();

void resetValuesAfterCompletion();

void extractUserAreaFromJSON(String message);
void testUserArea(String message);

void showHackingSuccessDisplay();
void showHackingFailureDisplay();
void showHackingInProgressDisplay();

void showPowerActivationDisplay();
void showLoginSuccessDisplay();
void showEndScreen();

#endif

