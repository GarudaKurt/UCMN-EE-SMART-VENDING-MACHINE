#include "display.h"
#include "ESP32Servo.h"
#include <Arduino.h>
LiquidCrystal_PCF8574 lcd(0x27);  // Define lcd here

Servo SM;
Servo MD;
Servo LG;

#define smPIN 18
#define mdPIN 19
#define lgPIN 22
#define IR 17

#ifdef ESP32
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <WiFiUdp.h>
#endif

#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "il2wrk"
#define WIFI_PASSWORD "il2wrk2025"
#define API_KEY "AIzaSyCf_yEMzkxTZTZvhxnIUfZl3rlqdWRE9Yo"
#define DATABASE_URL "https://iot-healthcare-72d91-default-rtdb.asia-southeast1.firebasedatabase.app"
#define USER_EMAIL "user1@gmail.com" //create OAuth user email
#define USER_PASSWORD "qwerty" //create OAuth user password

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long prevTime = 0;

void initFirebase() {

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Not connected to WiFi, please check!");
    delay(300);
  }
  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  Firebase.reconnectWiFi(true);
  fbdo.setBSSLBufferSize(4096, 1024);
  Firebase.begin(&config, &auth);
  Firebase.setDoubleDigits(5);
}

static void sendPayment(int total) {
  if (Firebase.ready()) {
    bool success = Firebase.setInt(fbdo, "/data/productTotal", total);
    bool success1 = Firebase.setBool(fbdo, "/data/paidSettled", false);
    Serial.print("Total ");
    Serial.println(total);
    if(!success || !success1)
      Serial.println("Failed to send to firebase");
  }
}

void resetStatus() {
  if (Firebase.ready()) {
    bool success1 = Firebase.setBool(fbdo, "/data/paidSettled", false);
    if(!success1)
      Serial.println("Failed to send to firebase");
  }
}

bool paymentSettle() {
  if (Firebase.ready() && (millis() - prevTime > 15000 || prevTime == 0)) {
    prevTime = millis();
    if (Firebase.getBool(fbdo, "/data/paidSettled")) {
      return fbdo.boolData();
    }
    Serial.println("Failed to get payment status from Firebase.");
    return false;
  }
  return false;
}



void initServo() {
  SM.attach(smPIN);
  MD.attach(mdPIN);
  LG.attach(lgPIN);
  pinMode(IR, INPUT);
}

void initLCD() {
  lcd.begin(20, 4);
  lcd.setBacklight(255);
  lcd.setCursor(2, 2);
  lcd.print("Initialized..");
  delay(1000);
}

int IRs() {
  return digitalRead(IR);
}

void servoSM() {
  Serial.println("Pass here!");
  SM.attach(smPIN);
  delay(300);
  SM.write(180);
  Serial.println("Done on SM");
}
void servoMD() {
  MD.attach(mdPIN);
  delay(300);
  MD.write(180);
  Serial.println("Done on MD");
}
void servoLG() {
  LG.attach(lgPIN);
  delay(300);
  LG.write(180);
  Serial.println("Done on LG");
}
void servoStop() {
  Serial.println("Stop Servo");
  SM.detach();
  MD.detach();
  LG.detach();
}

void showMenu() {
  Serial.println("Showing menu...");  // Debugging line
  Serial.println("Clearing LCD...");
  lcd.begin(20, 4);
  lcd.setBacklight(255);
  //lcd.clear();
  delay(1000);  // Allow time for clearing
  
  lcd.setCursor(1, 0);
  lcd.print("Smart Water Vendo");
  lcd.setCursor(1, 1);
  lcd.print("[1] SM $15.00");
  lcd.setCursor(1, 2);
  lcd.print("[2] MD $25.00");
  lcd.setCursor(1, 3);
  lcd.print("[3] LG $30.00");

  lcd.display();  // Ensure LCD updates
  Serial.println("Menu displayed!");
}


void processLCD(String menu, int qty) {
  int pricePerUnit = 0;
  String size = "";

  if (menu == "1") {
    pricePerUnit = 15;
    size = "SM";
  } else if (menu == "2") {
    pricePerUnit = 25;
    size = "MD";
  } else if (menu == "3") {
    pricePerUnit = 30;
    size = "LG";
  }

  int total = qty * pricePerUnit;
  sendPayment(total);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Selected: ");
  lcd.print(size);
  lcd.setCursor(2, 1);
  lcd.print("QTY: ");
  lcd.print(qty);
  lcd.setCursor(2, 2);
  lcd.print("Total: $");
  lcd.print(total);
  lcd.setCursor(0, 3);
  lcd.print("Scan QR for payment");
}
