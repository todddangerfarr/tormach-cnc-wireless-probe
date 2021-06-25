
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
const int outputPin = 6; 


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
  // put your main code here, to run repeatedly:
  if (rf69.available()) {
    // Should be a message for us now   
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (!len) return;
      buf[len] = 0;
      Serial.print("Received [");
      Serial.print(len);
      Serial.print("]: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf69.lastRssi(), DEC);

      if (strstr((char *)buf, "Hello World")) {
        // Send a reply!
        uint8_t data[] = "And hello back to you";
        rf69.send(data, sizeof(data));
        rf69.waitPacketSent();
        Serial.println("Sent a reply");
      }
    } else {
      Serial.println("Receive failed");
      display.println("Nothing");
      display.display();
    }
  }
}
