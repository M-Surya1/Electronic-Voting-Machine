#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>  // Include the Wire library for I2C communication
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#define BUZZER_PIN 8  // Define the pin for the buzzer
#define FINGERPRINT_RX 5  // Pin 5 on Arduino for receiving data from the fingerprint sensor
#define FINGERPRINT_TX 4  // Pin 4 on Arduino for transmitting data to the fingerprint sensor

SoftwareSerial mySerial(FINGERPRINT_RX, FINGERPRINT_TX);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

#define RST_PIN     9    // Reset pin
#define SS_PIN      10   // Slave Select pin
#define BUTTON_A_PIN  2  // Push button A pin
#define BUTTON_B_PIN  6  // Push button B pin
#define BUTTON_C_PIN  7  // Push button A pin
#define BUTTON_D_PIN  3  // Push button B pin
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
bool votestat[3] = {false, false, false};
int novotes[4] = {0,0,0,0};

void setup() {
  Wire.begin();

  lcd.init();
  lcd.backlight(); // Turn on backlight
  lcd.setCursor(0, 0);
  lcd.print("TAP YOUR VOTER ID");
  Serial.println("TAP YOUR VOTER ID");
  lcd.setCursor(0, 1);
  lcd.print("to vote");

  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);
  pinMode(BUTTON_C_PIN, INPUT_PULLUP);
  pinMode(BUTTON_D_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);  // Set buzzer pin as an output
  while (!Serial); // Wait for serial port to connect

  // Initialize the sensor
  finger.begin(57600);
}

void loop() {
 
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Show UID on serial monitor
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    content.toUpperCase();

    // Check if the UID matches the authorized tag
    if (content.substring(1) == "E3 CF 13 34" && votestat[0] == false) {
      lcd.clear();
      lcd.print("SURYA, PLACE YOUR FINGER");
      Serial.println("SURYA, PLACE YOUR FINGER");
      if (verifyFingerprint(1)) {
        lcd.clear();
        lcd.print("PRESS BUTTON TO VOTE");
        Serial.println("PRESS BUTTON TO VOTE");
        waitForButtonPress(0);
      }
    } else if (content.substring(1) == "C2 C9 4E D9" && votestat[1] == false) {
      lcd.clear();
      lcd.print("YOKESH, PLACE YOUR FINGER");
      Serial.println("YOKESH, PLACE YOUR FINGER");
      if (verifyFingerprint(2)) {
        lcd.clear();
        lcd.print("PRESS BUTTON TO VOTE");
        Serial.println("PRESS BUTTON TO VOTE");
        waitForButtonPress(1);
      }
    }
    else if (content.substring(1) == "42 0F E2 ED" && votestat[2] == false) {
      lcd.clear();
      lcd.print("VIGNESH, PLACE YOUR FINGER");
      Serial.println("VIGNESH, PLACE YOUR FINGER");
      if (verifyFingerprint(3)) {
        lcd.clear();
        lcd.print("PRESS BUTTON TO VOTE");
        Serial.println("PRESS BUTTON TO VOTE");
        waitForButtonPress(1);
      }
    }
    else if (content.substring(1) == "02 A8 52 D9") {
      lcd.clear();
      lcd.print("ELECTION OFFICER, PLACE YOUR FINGER");
      Serial.println("ELECTION OFFICER, PLACE YOUR FINGER");
      if (verifyFingerprint(4)) {
        displayVotes();
        delay(10000);
      }
    }
  }
}

void waitForButtonPress(int voterIndex) {
  bool buttonAPressed = false;
  bool buttonBPressed = false;
  while (!buttonAPressed && !buttonBPressed) {
    if (digitalRead(BUTTON_A_PIN) == LOW) {
      delay(50); // Debounce delay
      if (digitalRead(BUTTON_A_PIN) == HIGH) {
        buttonAPressed = true;
        novotes[0]++;
        lcd.clear();
        lcd.print("You have voted for A");
        Serial.println("You have voted for A");
        soundBuzzer();
        delay(1000);
        votestat[voterIndex] = true;
      }
    } else if (digitalRead(BUTTON_B_PIN) == LOW) {
      delay(50); // Debounce delay
      if (digitalRead(BUTTON_B_PIN) == HIGH) {
        buttonBPressed = true;
        novotes[1]++;
        lcd.clear();
        lcd.print("You have voted for B");
        Serial.println("You have voted for B");
        soundBuzzer();
        delay(1000);
        votestat[voterIndex] = true;
      }
    }
    else if (digitalRead(BUTTON_C_PIN) == LOW) {
      delay(50); // Debounce delay
      if (digitalRead(BUTTON_C_PIN) == HIGH) {
        buttonBPressed = true;
        novotes[2]++;
        lcd.clear();
        lcd.print("You have voted for C");
        Serial.println("You have voted for C");
        soundBuzzer();
        delay(1000);
        votestat[voterIndex] = true;
      }
    }
    else if (digitalRead(BUTTON_D_PIN) == LOW) {
      delay(50); // Debounce delay
      if (digitalRead(BUTTON_D_PIN) == HIGH) {
        buttonBPressed = true;
        novotes[3]++;
        lcd.clear();
        lcd.print("You have voted for D");
        Serial.println("You have voted for D");
        soundBuzzer();
        delay(1000);
        votestat[voterIndex] = true;
      }
    }
  }
}

void displayVotes() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" A: ");
  lcd.print(novotes[0]);
  lcd.print(" B: ");
  lcd.print(novotes[1]);
  lcd.print(" C: ");
  lcd.print(novotes[2]);
  lcd.setCursor(0, 1);
  lcd.print(" D: ");
  lcd.print(novotes[3]);

}
void soundBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);  // Turn on the buzzer
  delay(1000);  // Wait for 1 second
  digitalWrite(BUZZER_PIN, LOW);  // Turn off the buzzer
}
bool verifyFingerprint(int expectedID) {
  lcd.clear();
  lcd.print("Place finger to verify");

  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    Serial.println("No finger detected");
    return false;
  }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error converting image to template");
    return false;
  }

  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    uint16_t id = finger.fingerID;
    Serial.println(id);
    if (id == expectedID) {
      lcd.clear();
      lcd.print("Fingerprint matches!");
      Serial.println("Fingerprint matches!");
      soundBuzzer();
      return true;
    } else {
      lcd.clear();
      lcd.print("Fingerprint doesn't match");
      Serial.println("Fingerprint doesn't match");
      return false;
    }
  } else {
    lcd.clear();
    lcd.print("No match found");
    Serial.println("No match found");
    return false;
  }
}