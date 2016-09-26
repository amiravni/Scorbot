
#include "Encoder_Polling_V2.h"

const int pin_A = 4;  // Encoder input pins
const int pin_B = 5;
const int pin_C = 6;
const int pin_D = 7;

void setup()
{
  Serial.begin(9600);
  encoder_begin();  // Start the library
  attach_encoder(0, pin_A, pin_B);  // Attach an encoder to pins A and B
  attach_encoder(1, pin_C, pin_D);  // Attach another encoder to pins C and D
}

void loop()
{
  int dir_0 = encoder_data(0);  // First encoder
  int dir_1 = encoder_data(1);  // Second
  
  if(millis() > 10000)
  {
    dettach_encoder(1);  // After 10 seconds, dettach encoder 1 
  }
  
  if(dir_0 != 0)  // Check for rotation
  {
    Serial.print("Encoder 0: ");
    Serial.println(dir_0);
  }
  
  if(dir_1 != 0)  // Check for rotation
  {
    Serial.print("Encoder 1: ");
    Serial.println(dir_1);
  }
}
