// This program sends a response whenever it receives the "INF" mens
//
// Arduino9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Arduino9x_TX

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 434.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
#define LED 13

void setup() 
{
  pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  while (!Serial);
  Serial.begin(9600);
  delay(100);
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
  Serial.println("START");
}
    
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];    
uint8_t len = sizeof(buf);

void loop()
{
  if (rf95.available()){    
    
    if (rf95.recv(buf, &len)){
      digitalWrite(LED, HIGH);
      //RH_RF95::printBuffer("Got: ", buf, len);
      Serial.print("Received:  ");
      Serial.println((char*)buf);
      //Serial.print("RSSI: ");
      //Serial.println(rf95.lastRssi(), DEC);
      if (strcmp("INF",((char*)buf)) == 0){
        Serial.println("Received data request INF");
        delay(2000);
        Serial.println("Send mens: DATA ARDUINO");
        uint8_t data[] = "DATA ARDUINO";
        rf95.send(data, 13); //sizeof(data)
        rf95.waitPacketSent();
      }
      digitalWrite(LED, LOW);
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}
