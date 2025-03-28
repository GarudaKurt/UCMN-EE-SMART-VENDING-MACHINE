#include "display.h"
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '4', '7', '*'},
  {'2', '5', '8', '0'},
  {'3', '6', '9', '#'},
  {'A', 'B', 'C', 'D'}
};
byte rowPins[ROWS] = {32, 33, 25, 26};
byte colPins[COLS] = {27, 14, 12, 13};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String selectedMenu = "";
String quantity = "";
bool selectingMenu = true;

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing system...");

  initFirebase();
  initServo();
  delay(2000);  // Ensure peripherals initialize properly

  Serial.println("Displaying menu...");
  showMenu();  // Ensure menu is shown at startup
}

void loop() {
  char key = keypad.getKey();
  if(paymentSettle()) {
    if(selectedMenu == "1") {
      Serial.println("Small servo");
      servoSM();
    }
    else if(selectedMenu == "2") {
      Serial.println("Medium servo");
      servoMD();
    }
    else if(selectedMenu == "3") {
      Serial.println("Large servo");
      servoLG();
    }
    selectedMenu = "";
    Serial.println("Done settled");
    resetStatus();
    delay(1000);
  }

  if(IRs == 0)
    servoStop();

  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);

    if (selectingMenu) {
      if (key >= '1' && key <= '3') {
        selectedMenu = key;
        selectingMenu = false;
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Smart Water Vendo");
        lcd.setCursor(2, 1);
        lcd.print("Enter QTY: ");
        lcd.setCursor(2, 3);
        lcd.print("Press #: to submit");
      }
    } else {
      if (key >= '0' && key <= '9') {
        quantity += key;
        lcd.setCursor(12, 1);  // Adjust cursor for better display
        lcd.print(quantity);
      } else if (key == '#') { // Confirm
        if (quantity.length() > 0) {
          int qty = quantity.toInt();
          Serial.print("Processing order: ");
          Serial.print(selectedMenu);
          Serial.print(" x ");
          Serial.println(qty);

          processLCD(selectedMenu, qty);
          delay(5000);

          // Reset menu selection
          quantity = "";
          selectingMenu = true;  
          showMenu();
        } else {
          Serial.println("Invalid quantity! Ignoring...");
        }
      }
    }
  }
}
