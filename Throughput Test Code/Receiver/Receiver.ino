#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <printf.h>

RF24 radio(9,10);

const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };              // Radio pipe addresses for the 2 nodes to communicate.

const int packetSize = 32;

int blindData[packetSize];

void setup(){
  Serial.begin(115200);
  while(!Serial);

  printf_begin();

  // Setup and configure rf radio
  radio.begin();

  radio.setChannel(0);
  
  //radio.setChannel(1);
  radio.setPALevel(RF24_PA_HIGH);   // RF24_PA_MIN ,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate( RF24_1MBPS );  // RF24_250KBPS, RF24_1MBPS, RF24_2MBPS

  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  radio.startListening();                 // Start listening
  radio.printDetails();                   // Dump the configuration of the rf unit for debugging
}

int total = 0;

void loop() {

  while(!radio.available());
 
  radio.read( &blindData, packetSize );
  total++;

  Serial.print("Finished reading data. Total Packets = ");
  Serial.print(total);
  Serial.print("\n");
}
