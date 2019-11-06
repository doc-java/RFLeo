#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Keyboard.h>

RF24 radio(9,10);

// Topology
const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };              // Radio pipe addresses for the 2 nodes to communicate.
// Role management: Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  
typedef enum { role_ping_out = 1, role_pong_back } role_e;                 // The various roles supported by this sketch


const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};  // The debug-friendly names of those roles

// The role of the current running sketch
role_e role = role_pong_back;      


byte command[100]; //allow 100 chars

// A single byte to keep track of the data being sent back and forth
byte counter = 1;
void setup(){
  Keyboard.begin();
  
  Serial.begin(115200);

  // Setup and configure rf radio
  radio.begin();

  radio.setPALevel(RF24_PA_HIGH);   // RF24_PA_MIN ,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate( RF24_1MBPS ); // RF24_250KBPS, RF24_1MBPS, RF24_2MBPS

  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.setRetries(0,15);                 // Smallest time between retries, max no. of retries
  //radio.setPayloadSize(100);                // Here we are sending 1-byte payloads to test the call-response speed
  radio.openWritingPipe(pipes[1]);        // Both radios listen on the same pipes by default, and switch when writing
  radio.openReadingPipe(1,pipes[0]);
  radio.startListening();                 // Start listening
  //radio.printDetails();                   // Dump the configuration of the rf unit for debugging
}
void loop() {
      delay(5000); //give 5s to not explode
      
      // Pong back role.  Receive each packet, dump it out, and send it back
      if ( role == role_pong_back ) {
        byte pipeNo;
        byte gotByte;                                       // Dump the payloads until we've gotten everything
        while( radio.available(&pipeNo)){

              Serial.println("matter");
              Serial.flush();
          
              radio.read( command, 100 );
    
              String com = String((char*)command);

              
              Serial.println(com);
              Serial.flush();

              int i = 0;
              char c = com[i];
              while(c != '\0'){
                if(c == '<'){
                  while(c != '>'){
                    c = com[++i];
                    Keyboard.press(c); //press the command key
                    c = com[++i];
                    Keyboard.press(c); //press the command letter
                    c = com[++i];
                  }
                  c = com[++i];
                  Keyboard.releaseAll();
                  delay(250);
                }
                else{ //otherwise just write the command
                  Keyboard.press(c);
                  c = com[++i];
                  Keyboard.releaseAll();
                }
                while(c != '<'){
                  Keyboard.press(c);
                  c = com[++i];
                }
                delay(10);
       
              }

          int j = 1;
          radio.writeAckPayload(pipeNo, &j, 1);         
       }
     }

}
