
// -*- mode: C++ -*-
// Tormach Wireless Probe Project Reciever Code

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>

/************ OLED Setup ****************/

Adafruit_SH110X display = Adafruit_SH110X(64, 128, &Wire);

// OLED FeatherWing buttons map to different pins depending on board:
#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5


/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 915.0

// who am i? (server address)
#define MY_ADDRESS    1
#define RFM69_CS      8
#define RFM69_INT     7
#define RFM69_RST     4
#define LED           13

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

int16_t packetnum = 0;
int probeConnected = 1; 
bool itsBeenAWhile = false;
const int outputPin = 6; 
unsigned long refTime;
unsigned long curTime; 
String batteryInfo;
int numLoops = 0;


/************ SETUP *********************/
void setup() {
  Serial.begin(115200);

  display.clearDisplay();
  delay(100);
  display.begin(0x3C, true); // Address 0x3C default
  display.display();
  delay(3000); // Show Splash Screen
  display.clearDisplay();
  display.display();

  display.setRotation(1);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE, SH110X_BLACK);
  display.setCursor(0,0);
  display.print("Looking for Probe...");
  display.display(); // actually display all of the above

  pinMode(LED, OUTPUT);     
  pinMode(RFM69_RST, OUTPUT);
  pinMode(outputPin, OUTPUT); 
  digitalWrite(RFM69_RST, LOW);

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  
  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");

  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }
  
  rf69.setTxPower(20, true);
  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);

  pinMode(LED, OUTPUT);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");

}


/************ LOOP *********************/
void loop() {
  delay(5);
  refTime = millis();
  if (rf69.available()) {
    // Should be a message for us now   
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (!len) return;
      buf[len] = 0;

      // IF PROBE IS ON, BRING OUTPUT LOW
      if (buf[0] == 48) {
        digitalWrite(outputPin, HIGH);
      } else if (buf[0] == 49) {
        digitalWrite(outputPin, LOW);
      }

      // BATTERY INFORMATION 
      if (buf[1] == 48) {
        batteryInfo = "Battery: Low";
      } else if (buf[0] == 49) {
        batteryInfo = "Battery: OK!";
      }

      // Update display every 1000 ms (each loop is 5ms)
      if ((numLoops > 100) || (numLoops == 0)) {
        display.setCursor(0, 0);
        display.println("FARR FRAMEWORKS      ");
        display.println("                     ");
        display.println("                     ");
        display.println("Probe Connected!     ");
        display.println(batteryInfo);
        display.print("RSSI:"); display.println(rf69.lastRssi());
        display.display();
        numLoops = 1;
      }
      numLoops ++; 
      delay(5);

    } else {
      Serial.println("Receive failed");
    }
  } 

  // Time out probe comms haven't happened in a while
  curTime = millis();
  if ((curTime - refTime) > 2000) {
    itsBeenAWhile = true;
  } else {
    itsBeenAWhile = false; 
  }
  if (itsBeenAWhile) {
    digitalWrite(outputPin, HIGH);
    display.clearDisplay(); 
    display.setCursor(0, 0);
    display.println("FARR FRAMEWORKS      ");
    display.println("                     ");
    display.println("ET Probe Home?");
    display.print("Check Connection."); 
    display.display();
    delay(300); 
  }
}
