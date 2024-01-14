#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <printf.h>

RF24 radio(9,10);

const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };              // Radio pipe addresses for the 2 nodes to communicate.

const int totalData = 50;

const int packetSize = 32;

byte blindData[totalData][packetSize];

long gendatatot = 0;

void generateData(){
  for(int i = 0; i < totalData; i++){
    gendatatot++;
    //Serial.print(i);
    //Serial.print(": ");
    for(int j = 0; j < packetSize; j++){
      blindData[i][j] = (byte)random(25);
      //Serial.print( blindData[i][j]);
      //Serial.print(" ");
    }
    //Serial.print("\n");
  }
}

void setup(){
  Serial.begin(115200);
  while(!Serial);
 
  printf_begin();
  Serial.println("Creating blind data");

  generateData();

  // Setup and configure rf radio
  radio.begin();
  
  radio.setChannel(0);
  radio.setPALevel(RF24_PA_HIGH);   // RF24_PA_MIN ,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate( RF24_2MBPS );  // RF24_250KBPS, RF24_1MBPS, RF24_2MBPS
  
  radio.setPayloadSize(packetSize); //payload size of byte array size

  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.printDetails();                   // Dump the configuration of the rf unit for debugging
  
  Serial.println("Ready to go. Press 1 to begin data transfer.");
}

int maxIterations = 10;

void loop() {
    
    if(Serial.available() > 0 && Serial.read() == '1'){ 
      //start the blind data transfer
      Serial.println("Sending");
      Serial.flush();

      radio.stopListening(); 

      for(int gen = 0; gen < maxIterations; gen++){
        
        for(int i = 0; i < totalData; i++){
          radio.write( &blindData[i], packetSize );
        }
        generateData();
      }

      Serial.println("Finished sending all blind data");
      Serial.println(gendatatot);
    }

}
