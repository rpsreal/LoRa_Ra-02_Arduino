//  This program asks a client for data and waits for the response, then sends an ACK. 
//
// Copyright 2018 Rui Silva.
// This file is part of rpsreal/LoRa_Ra-02_Arduino
// Based on example LoRa 9x_TX RADIOHEAD library
// It is designed to work with LORA_CLIENT_encrypted in Arduino or Raspberry Pi

#include <SPI.h>      // SPI
#include <RH_RF95.h>  // HadioHead http://www.airspayce.com/mikem/arduino/RadioHead/
#include <Base64.h> // https://github.com/adamvr/arduino-base64
#include <AESLib.h> // https://github.com/DavyLandman/AESLib

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

// Blinky on receipt
#define LED 13
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 434.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() 
{
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

int8_t send_ack=0; // flag var
uint8_t key[] = "1234567890123456";

void loop()
{

  while (send_ack==0){
    digitalWrite(LED, HIGH);
    
    uint8_t input[] = "INF             "; // 16 char

    aes128_enc_single(key, input);

    uint8_t  inputLen = sizeof(input)-1;
    uint8_t  encodedLen = base64_enc_len(inputLen);
    uint8_t  encoded[encodedLen+1];
    base64_encode((char*)encoded, (char*)input, inputLen);

    rf95.send(encoded, sizeof(encoded));
    rf95.waitPacketSent();
    Serial.print("== SEND:  ");
    Serial.print("INF             ");
    Serial.print("  |  Encoded: ");
    Serial.println((char*)encoded);

    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.waitAvailableTimeout(10000)){
      if (rf95.recv(buf, &len)){
        
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

          send_ack=1;
        
      }else{
         Serial.println("Receive failed");
      }
    }else{
         Serial.println("Send INF again");
    }
   }
  
  if(send_ack==1){ //Send: ACK
    delay(2000);
    
    uint8_t input[] = "ACK             "; // 16 char
       
    aes128_enc_single(key, input);

    uint8_t  inputLen = sizeof(input)-1;
    uint8_t  encodedLen = base64_enc_len(inputLen);
    uint8_t  encoded[encodedLen+1];
    base64_encode((char*)encoded, (char*)input, inputLen);

    rf95.send(encoded, sizeof(encoded));
    rf95.waitPacketSent();
    Serial.print("== SEND:  ");
    Serial.print("ACK             ");
    Serial.print("  |  Encoded: ");
    Serial.println((char*)encoded);
    Serial.println("\n");
    
    send_ack=0;
  }
  digitalWrite(LED, LOW);
  delay(10000); // send new packet every 10 seconds
}
