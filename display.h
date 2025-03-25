#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <LiquidCrystal_PCF8574.h>
#include <ESP32Servo.h>

extern LiquidCrystal_PCF8574 lcd;  // Declare lcd as external

void initLCD();
void showMenu();
void processLCD(String menu, int qty);
void servoStop();
void initFirebase();
bool paymentSettle();
void resetStatus();
void servoSM();
void servoMD();
void servoLG();
void initServo();

#endif
