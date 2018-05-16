# LoRa_Ra-02_Arduino
Use Ra-02 LoRa module with Arduino 

This example code is based on RadioHead library so you will need to download the RadioHead library. 
You can do that by visiting the [AirSpayce's Radiohead site](http://www.airspayce.com/mikem/arduino/RadioHead/)

These examples can be used to communicate with Raspberry Pi through python using examples from this library [rpsreal/pySX127x](https://github.com/rpsreal/pySX127x)

AI-Thinker module Ra-02:
 - Semtech SX1278 low power long range transceiver
 - 433MHz frequency (420 to 450 MHz range)
 - SPI communication
 - Supply Voltage default 3.3V

Arduino boad or compatible
 - This boad is 3,3V level and not 5V so to use Arduino Uno or Arduino Pro Mini 5V you have to use a Bi-Directional Logic Level Converter.

 Wiring example with Arduino Pro Mini 3,3V 8MHz
	
 ![wiring_img](./wiring.png)
 
 Developed by Rui Silva, Porto, Portugal
