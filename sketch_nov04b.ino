#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Ultrasonic.h>
#include <Adafruit_GFX.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
#define RST_PIN 9
#define SS_PIN 10
MFRC522 mfrc522(SS_PIN, RST_PIN);
#define ULTRASONIC_TRIGGER 7
#define ULTRASONIC_ECHO 6
Ultrasonic ultrasonic(ULTRASONIC_TRIGGER, ULTRASONIC_ECHO);
#define PIR_PIN 2
#define LDR_PIN A0
#define RELAY_PIN 5
#define BUZZER_PIN 8

// Define a list of random RFID tags (you can replace these with actual RFID tag values)
char* allowedRFIDTags[] = {
  "6E 6E E0 87",  // Tag 1
  "D0 DD 5D 10",  // Tag 2
  "F3 DB OE 96",  // Tag 3
  // Add more RFID tags here
};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Security System");
  delay(2000);
  lcd.clear();
  pinMode(PIR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String scannedTag = getRFIDTag();
    if (isRFIDTagAllowed(scannedTag)) {
      lcd.clear();
      lcd.print("Access Granted");
      digitalWrite(RELAY_PIN, HIGH);
      delay(2000);
      lcd.clear();
    } else {
      lcd.clear();
      lcd.print("Unauthorized Tag");
      delay(2000);
      lcd.clear();
    }
  }

  float distance = ultrasonic.distanceRead(); // Corrected function name
  if (distance < 100) {
    lcd.clear();
    lcd.print("Intruder Detected");
    digitalWrite(BUZZER_PIN, HIGH);
    delay(2000);
    lcd.clear();
  }

  int motionDetected = digitalRead(PIR_PIN);
  if (motionDetected == HIGH) {
    lcd.clear();
    lcd.print("Motion Detected");
    digitalWrite(BUZZER_PIN, HIGH);
    delay(2000);
    lcd.clear();
  }

  int lightIntensity = analogRead(LDR_PIN);
  if (lightIntensity < 100) {
    lcd.clear();
    lcd.print("Laser Breach");
    digitalWrite(BUZZER_PIN, HIGH);
    delay(2000);
    lcd.clear();
  }

  lcd.clear();
  lcd.print("System Ready");
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);
  delay(1000);
}

String getRFIDTag() {
  String tag = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    tag.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    tag.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  tag.toUpperCase();
  return tag;
}

bool isRFIDTagAllowed(String scannedTag) {
  for (int i = 0; i < sizeof(allowedRFIDTags) / sizeof(allowedRFIDTags[0]); i++) {
    if (scannedTag == allowedRFIDTags[i]) {
      return true;
    }
  }
  return false;
}
