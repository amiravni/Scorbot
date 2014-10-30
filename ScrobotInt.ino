#define DELAY 25
#define LEDPIN 14

#include "Encoder_Polling_V2.h"

unsigned long startTime;
// —————————————————————————  MicroSwitches
const int MS1 = 2;


// —————————————————————————  Encoders

const int pin_P0[6] = {22,24,26,28,30,32};  // Encoder input pins
const int pin_P1[6] = {23,25,27,29,31,33};
int FW[6] = {0,0,0,0,0,0};
volatile long  counter[6]={0,0,0,0,0,0};

// —————————————————————————  Motors
const int M1[6] = {14,4,6,8,10,12};
const int M2[6] = {15,5,7,9,11,13};
// ————————————————————————— Setup
void setup() {
  Serial.begin(9600);
  encoder_begin();  // Start the library
  for (int i=0;i<6;i++) {
    attach_encoder(i, pin_P0[i], pin_P1[i]);  // Attach an encoder to pins A and B
    pinMode(pin_P0[i],INPUT_PULLUP);
    pinMode(pin_P1[i],INPUT_PULLUP);
    pinMode(M1[i],OUTPUT);
    pinMode(M2[i],OUTPUT);
    motorST(M1[i],M2[i]);
  }
  pinMode(MS1,INPUT);

  pinMode(LEDPIN,OUTPUT);
  digitalWrite(LEDPIN,LOW);

attachInterrupt(0, resetCounter, FALLING);

}


void loop() {
  char cmd=0;
  int dir0[6]={0,0,0,0,0,0};
  unsigned long testTime = 500;
  if ( millis() - startTime  < testTime  ) { 
    
  for (int i=0;i<6;i++) {
  dir0[i] = encoder_data(i);  // First 
    if(dir0[i] != 0)  // Check for rotation
  {
    counter[i] += FW[i]*abs(dir0[i]);
  }
  }
  
  }
  else
  {
      for (int i=0;i<6;i++) {
    motorST(M1[i],M2[i]);
    FW[i]=0;
      }
  }

  if ( (millis() - startTime  > testTime + 50 ) && ( millis() - startTime< testTime + 55 ) ) { 
     for (int i=0;i<6-1;i++) {
     Serial.print(counter[i]);
     Serial.print("  ,  ");
  }
    Serial.println(counter[5]);
  }

  if (Serial.available() > 0) 
  {

    cmd = Serial.read();
    if (cmd=='q') {
      for (int i=0;i<6;i++) {
    motorFW(M1[i],M2[i]);
    FW[i]=1;
      }
    }
  else if (cmd == 'Q')  {
      for (int i=0;i<6;i++) {
    motorBW(M1[i],M2[i]);
    FW[i]=-1;
      }
  }
 startTime = millis();
  }

}



void motorFW(const int mtr1, const int mtr2  ){
  digitalWrite(mtr1, HIGH);
  digitalWrite(mtr2, LOW);
}
void motorBW(const int mtr1, const int mtr2  ){
  digitalWrite(mtr2, HIGH);
  digitalWrite(mtr1, LOW);
}
void motorST(const int mtr1, const int mtr2  ){
  digitalWrite(mtr1, LOW);
  digitalWrite(mtr2, LOW);
}

void resetCounter()
{
counter[0]=0;
  
}

