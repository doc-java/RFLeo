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
role_e role = role_ping_out;  
   
// The role of the current running sketch
// A single byte to keep track of the data being sent back and forth
byte counter = 1;
void setup(){
  Serial.begin(115200);
  Serial.print(F("\n\rRF24/examples/pingpair_ack/\n\rROLE: "));
  Serial.println(role_friendly_name[role]);
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));
  
  // Setup and configure rf radio
  radio.begin();
  
  radio.setPALevel(RF24_PA_HIGH);   // RF24_PA_MIN ,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate( RF24_1MBPS ); // RF24_250KBPS, RF24_1MBPS, RF24_2MBPS
  
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.setRetries(15,15);                 // Smallest time between retries, max no. of retries
  //radio.setPayloadSize(100);                // Here we are sending 1-byte payloads to test the call-response speed
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
}

void loop() {
    
    Serial.println("test2y");
    Serial.flush();

    byte c1[9] = {'<', KEY_LEFT_GUI, 'r', '>', 'c', 'm', 'd', 176, '\0'};
    
    if (role == role_ping_out){
      
      radio.stopListening();                                  // First, stop listening so we can talk.
        
      Serial.print("Now sending payload ");
      Serial.println(counter);
      unsigned long timer = micros();                          // Take the time, and send it.  This will block until complete   
                                                              //Called when STANDBY-I mode is engaged (User is finished sending)
      if (!radio.write( &c1, sizeof(c1) )){
        Serial.println(F("Failed."));      
      }else{
        if(!radio.available()){ 
          Serial.println(F("Blank Payload Received.")); 
        }else{
          while(radio.available() ){
            unsigned long tim = micros();
            int gotByte = 0;
            radio.read( &gotByte, 1 );
            Serial.print("Got response, round-trip delay: ");
            Serial.print(tim-timer);
            Serial.println(" microseconds\n\r");
            counter++;
          }
        }
      }

      // Try again later
      delay(1000);
      
    }

}
