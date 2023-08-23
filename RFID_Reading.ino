#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

LiquidCrystal_I2C lcd(0x27, 16, 2); // Initialize LCD with I2C address 0x27

SoftwareSerial ser(2, 3); // RX, TX
int receiverPin = 7; // the pin number of the receiver
int ledPin =8;
int val = 0; // variable to store the read value
unsigned long displayTime = 0; // variable to track the display time

void setup() 
{
  pinMode(receiverPin, INPUT); // sets the receiver pin as input
  pinMode(ledPin, OUTPUT); // sets the LED pin as output
  Serial.begin(9600);   // Initiate a serial communication
  ser.begin(115200);    // Initiate software serial communication
  SPI.begin();          // Initiate SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  lcd.init();           // Initialize the LCD
  lcd.backlight();      // Turn on backlight
  lcd.setCursor(0, 0);
  lcd.print("Wait for Module ...");
  lcd.setCursor(0, 1);
}

void loop() 
{
  val = digitalRead(receiverPin); // read the input pin


  if (val == 1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Put RFID Card to");
    lcd.setCursor(0, 1);
    lcd.print("Scan...");
    Serial.println(val); // print the value to the serial monitor
    delay(1000);

    // Look for new cards
    if (!mfrc522.PICC_IsNewCardPresent()) 
    {
      return;
    }
    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) 
    {
      return;
    }
    // Show UID on serial monitor
  
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println(content);
    Serial.print("User No./Name:    ");
    content.toUpperCase();
    
    lcd.clear();
    lcd.setCursor(0, 0);
    // lcd.print("UID: ");
    // lcd.print(content);

    if (content.substring(1) == "6D BF 63 38")
    {
      lcd.setCursor(0, 1);
      lcd.print("1-Maheel Lakshan");
      ser.write(1);
      digitalWrite(ledPin, HIGH); // Set the LED pin high
      delay(100); // Delay for 20 ms
      digitalWrite(ledPin, LOW); // Set the LED pin low
      displayTime = millis() + 1500; // Set the display time to 1500 ms ahead
      delay(2000);
    }
    else if (content.substring(1) ==  "DC 71 04 49")
    {
      lcd.setCursor(0, 1);
      lcd.print("2-Pradeep Sandaruwan");
      ser.write(2);
      digitalWrite(ledPin, HIGH); // Set the LED pin high
      delay(100); // Delay for 20 ms
      digitalWrite(ledPin, LOW); // Set the LED pin low
      displayTime = millis() + 1500; // Set the display time to 1500 ms ahead
      delay(2000);
    }
    else if (content.substring(1) == "7C 05 3F 49")
    {
      lcd.setCursor(0, 1);
      lcd.print("3-Asela Prasanna");
      ser.write(3);
      digitalWrite(ledPin, HIGH); // Set the LED pin high
      delay(100); // Delay for 20 ms
      digitalWrite(ledPin, LOW); // Set the LED pin low
      displayTime = millis() + 1500; // Set the display time to 1500 ms ahead
      delay(2000);
    }else{
      lcd.setCursor(0, 1);
      lcd.print("Invalid ID");
      digitalWrite(ledPin, HIGH); // Set the LED pin high
      delay(400); // Delay for 20 ms
      digitalWrite(ledPin, LOW); // Set the LED pin low
      displayTime = millis() + 1500; // Set the display time to 1500 ms ahead
      delay(2000);
    }
  }
   // Check if it's time to display "Next"
  if (millis() >= displayTime)
  {
    lcd.setCursor(0, 0);
    //lcd.print("Next");
    lcd.setCursor(0, 1);
    lcd.print("                "); // Clear the previous name
  }
}