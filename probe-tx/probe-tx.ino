// -*- mode: C++ -*-
// Tormach Wireless Probe Project Sender Code

#include <SPI.h>
#include <RH_RF69.h>

/************ Radio Setup ***************/

#define RF69_FREQ 915.0
#define RFM69_CS      8
#define RFM69_INT     7
#define RFM69_RST     4
#define LED           13
#define VBATPIN       A9

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetnum = 0;  // packet counter, we increment per xmission
int timeBetweenTX = 10;
int probeState = 0; 

void setup() {
  Serial.begin(115200);
  //while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer

  pinMode(LED, OUTPUT);     
  pinMode(RFM69_RST, OUTPUT);
  pinMode(A0, INPUT_PULLUP);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("Feather RFM69 TX Test!");
  Serial.println();

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
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);
  
  pinMode(LED, OUTPUT);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");

}

void loop() {
  delay(timeBetweenTX);

  // Battery Management
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage

  // Probe State
  probeState = digitalRead(A0);
  Serial.print("Probe State "); Serial.println(probeState);

  // Construct Packet
  char radiopacket[3];
  radiopacket[0] = ProbeState(probeState);
  radiopacket[1] = BatteryState(measuredvbat); 
  itoa(packetnum++, radiopacket+2, 10);
  Serial.print("Sending "); Serial.println(radiopacket);
  
  // SEND THE MESSAGE!
  rf69.send((uint8_t *)radiopacket, strlen(radiopacket));
  rf69.waitPacketSent(); 
}

// Function to return Probe State
char ProbeState(int probeState) {
  if (probeState == 0) {
    return '1'; 
  } else {
    return '0';
  }
}

// Function to return Battery State
char BatteryState(float batteryVoltage) {
  if (batteryVoltage > 3.3) {
    return '1'; // Battery OK
  } else {
    return '0'; // Battery Low
  }
}
