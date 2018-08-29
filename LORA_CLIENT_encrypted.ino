// This program sends a response whenever it receives the "INF" mens
//
// Copyright 2018 Rui Silva.
// This file is part of rpsreal/LoRa_Ra-02_Arduino
// Based on example LoRa 9x_RX RADIOHEAD library
// It is designed to work with LORA_SERVER_encrypted in Arduino or Raspberry Pi

#include <SPI.h>      // SPI
#include <RH_RF95.h>  //HadioHead http://www.airspayce.com/mikem/arduino/RadioHead/
#include <Base64.h> // https://github.com/adamvr/arduino-base64
#include <AESLib.h> // https://github.com/DavyLandman/AESLib

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
  rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);

  
  // Defaults after init are 434.0MHz, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // Medium Range
  
  rf95.setTxPower(18);
  Serial.println("START");
}
    
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];    
uint8_t len = sizeof(buf);
uint8_t key[] = "1234567890123456";

void loop()
{
  if (rf95.available()){    
    
    if (rf95.recv(buf, &len)){
      digitalWrite(LED, HIGH);
      //RH_RF95::printBuffer("Got: ", buf, len);
      //Serial.print("RSSI: ");
      //Serial.println(rf95.lastRssi(), DEC);
      Serial.print("== RECEIVED:  ");
      Serial.print((char*)buf);
      
      uint8_t bufLen = sizeof(buf);
      uint8_t decodedLen = base64_dec_len((char*)buf, bufLen);
      uint8_t data_de[decodedLen];
      base64_decode((char*)data_de, (char*)buf, bufLen);

      aes128_dec_single(key, data_de);
      Serial.print("  |  Decoded: ");
      Serial.println((char*)data_de);

      
      if (strcmp("INF             ",((char*)data_de)) == 0){
        Serial.println("Received data request INF - going to send mens:DATA ARDUINO   ");
        delay(2000);

        uint8_t input[] = "DATA ARDUINO    "; // 16 char 
        aes128_enc_single(key, input);
        uint8_t  inputLen = sizeof(input)-1;
        uint8_t  encodedLen = base64_enc_len(inputLen);
        uint8_t  encoded[encodedLen+1];
        base64_encode((char*)encoded, (char*)input, inputLen);
        
        rf95.send(encoded, sizeof(encoded));
        rf95.waitPacketSent();
        Serial.print("== SEND:  ");
        Serial.print("DATA ARDUINO   ");
        Serial.print("  |  Encoded: ");
        Serial.println((char*)encoded);
      }else if (strcmp("ACK             ",((char*)data_de)) == 0){
        Serial.println("\n");
      }
        digitalWrite(LED, LOW);
    }else{
      Serial.println("Receive failed");
    }
  }
}
