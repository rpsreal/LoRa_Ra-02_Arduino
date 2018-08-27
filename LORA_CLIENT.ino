// This program sends a response whenever it receives the "INF" mens
//
// Copyright 2018 Rui Silva.
// This file is part of rpsreal/LoRa_Ra-02_Arduino
// Based on example Arduino9x_RX RADIOHEAD library
// It is designed to work with LORA_SERVER

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

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:

  
  // Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. 
  // Slow+long range. 
  //rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);

  
  // Defaults after init are 434.0MHz, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // Medium Range
  
  rf95.setTxPower(18);


  
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
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
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
