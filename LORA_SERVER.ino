//  This program asks a client for data and waits for the response, then sends an ACK. 
//
// LoRa 9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example LoRa9x_RX

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

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

  Serial.println("Arduino LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

int8_t send_ack=0; // flag var

void loop()
{

  while (send_ack==0){
    digitalWrite(LED, HIGH);
    Serial.println("Send: INF");
    char radiopacket[3] = "INF";
    rf95.send((uint8_t *)radiopacket, 3);
    delay(10);
    rf95.waitPacketSent();

    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.waitAvailableTimeout(1000)){ 
      if (rf95.recv(buf, &len)){
         Serial.print("Receive: ");
         Serial.println((char*)buf);
         send_ack=1;
         //Serial.print("RSSI: ");
         //Serial.println(rf95.lastRssi(), DEC);    
      }else{
         Serial.println("Receive failed");
      }
    }else{
         Serial.println("Send INF again");
   }
  
  if(send_ack==1){ //Send: ACK
    Serial.println("Send: ACK");
    char radiopacket[3] = "ACK";
    rf95.send((uint8_t *)radiopacket, 3);
    delay(10);
    rf95.waitPacketSent();
    send_ack=0;
  }
  digitalWrite(LED, LOW);
  delay(10000);
}
