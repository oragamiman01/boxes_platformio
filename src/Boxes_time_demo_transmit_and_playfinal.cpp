#include <Arduino.h>
// Include RF libraries
#include <RadioHead.h>
#include <RH_ASK.h>
// Include dependant SPI Library 
#include <SPI.h> 
// Include SD card reading libraries
#include <SdFat.h>
#include <TMRpcm.h>


#define buffSize 128 // for TMRpcm
#define BLOCK_COUNT 10000UL // no references?

// SD card object
SdFat SD;

// Create Amplitude Shift Keying Object
RH_ASK rf_driver;

#define SD_ChipSelectPin 4  //example uses hardware SS pin 53 on Mega2560

TMRpcm audio;   // create an object for use in this sketch 

void setup()
{
  audio.speakerPin = 9; //5,6,11 or 46 on Mega, 9 on Uno, Nano, etc

  Serial.begin(9600);

  if (!SD.begin(SD_ChipSelectPin)) {  // Begin serial connection with SD card
    Serial.println("SD NOT starting");
    return;
  } else {
    Serial.println("SD OK");
  }

  // The audio library needs to know which CS pin to use for recording
  audio.CSPin = SD_ChipSelectPin;
  audio.disable(); // ??

  // Initialize ASK Object
  rf_driver.init();
}

// Initialize with unkown follower and leader status
bool leader = false;
bool follower1 = false;
bool follower2 = false;



void loop()
{
  // Set buffer to size of expected message
  uint16_t data;
  uint8_t datalen = sizeof(data);


  // Send leader packet
  if (leader){
    uint16_t data = millis()/1000-10;
    rf_driver.send((uint8_t*)&data, sizeof(data));
    rf_driver.waitPacketSent();
  }

  // Check if there is a packet to be recieved and if there is play follower
  if(!leader && !follower1){
    for (int i = 0; i <= 1000; i++) {
      delay(10);

      if (rf_driver.recv((uint8_t*)&data, &datalen)&& datalen == sizeof(data)) {
        Serial.println("Message Received: leader taken");
        Serial.println("playing follower 1 at time: ");

        uint16_t xyz = data;
        Serial.print(xyz);

        audio.play("follow1.wav", data); // Play the recording
        follower1 = true; break;
      }
    }
  }

  // If there is no packet and no current audio play leader 
  if(!leader && !follower1){
    if (!rf_driver.recv((uint8_t*)&data, &datalen) && datalen == sizeof(data)) {
      Serial.println("No message");
      audio.play("lead.wav"); 
      Serial.println("playing leader"); //Play the recording
      leader = true;
    }
  }
}
