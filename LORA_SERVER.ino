//  This program asks a client for data and waits for the response, then sends an ACK. 
//
// Copyright 2018 Rui Silva.
// This file is part of rpsreal/LoRa_Ra-02_Arduino
// Based on example LoRa 9x_TX RADIOHEAD library
// It is designed to work with LORA_CLIENT

#include <SPI.h>
#include <RH_RF95.h>

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

  
  // Slow+long range. Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. 
  //rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
  
  // Defaults medium Range after init are 434.0MHz, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  rf95.setTxPower(18);

  
  Serial.println("START");
}

int8_t send_ack=0; // flag var

void loop()
{

  while (send_ack==0){
    digitalWrite(LED, HIGH);
    Serial.println("Send: INF");
    char radiopacket[4] = "INF";
    rf95.send((uint8_t *)radiopacket, 4);
    delay(10);
    rf95.waitPacketSent();

    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.waitAvailableTimeout(20000)){ 
      if (rf95.recv(buf, &len)){
         Serial.print("Receive: ");
         Serial.println((char*)buf);
         send_ack=1;
         Serial.print("RSSI: ");
         Serial.println(rf95.lastRssi(), DEC);
         delay(2000);    
      }else{
         Serial.println("Receive failed");
      }
    }else{
         Serial.println("Send INF again");
    }
   }
  
  if(send_ack==1){ //Send: ACK
    delay(1000);
    Serial.println("Send: ACK");
    char radiopacket[4] = "ACK";
    rf95.send((uint8_t *)radiopacket,4);
    delay(10);
    rf95.waitPacketSent();
    send_ack=0;
  }
  digitalWrite(LED, LOW);
  delay(10000);
}
