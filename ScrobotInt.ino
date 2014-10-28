#define DELAY 25
#define LEDPIN 14

#include "Encoder_Polling_V2.h"

unsigned long startTime;
// —————————————————————————  MicroSwitches
const int MS1 = 2;


// —————————————————————————  Encoders

const int pin_1_P0 = 22;  // Encoder input pins
const int pin_1_P1 = 23;
int FW1 = 0;
volatile long  counter1=0;

// —————————————————————————  Motors
const int motor1[] = {   14, 15};
const int motor2[] = {   4, 5};
const int motor3[] = {   6, 7};
const int motor4[] = {   8, 9};
const int motor5[] = {   10, 11};
const int motor6[] = {   12, 13};

// ————————————————————————— Setup
void setup() {
  Serial.begin(9600);
  encoder_begin();  // Start the library
  attach_encoder(0, pin_1_P0, pin_1_P1);  // Attach an encoder to pins A and B
  pinMode(pin_1_P0,INPUT_PULLUP);
  pinMode(pin_1_P1,INPUT_PULLUP);
  pinMode(MS1,INPUT);
  // Setup motors
  int i;
  for(i = 0; i < 2; i++){
    pinMode(motor1[i], OUTPUT);
    pinMode(motor2[i], OUTPUT);   
    pinMode(motor3[i], OUTPUT);
    pinMode(motor4[i], OUTPUT);
    pinMode(motor5[i], OUTPUT);
    pinMode(motor6[i], OUTPUT);    
  }
  pinMode(LEDPIN,OUTPUT);
  motorST(motor1);
  motorST(motor2);
  motorST(motor3);
  motorST(motor4);
  motorST(motor5);
  motorST(motor6);
  digitalWrite(LEDPIN,LOW);

attachInterrupt(0, resetCounter, FALLING);

}


void loop() {
  char cmd=0;
  
  unsigned long testTime = 500;
  if ( millis() - startTime  < testTime  ) { 

  int dir_0 = encoder_data(0);  // First encoder
  if(dir_0 != 0)  // Check for rotation
  {
    counter1 += FW1*abs(dir_0);
 //   Serial.print("Encoder 0: ");
 //   Serial.println(dir_0);
  }
  
  }
  else
  {
  motorST(motor1);
    FW1=0;
  }

  if ( (millis() - startTime  > testTime + 50 ) && ( millis() - startTime< testTime + 55 ) ) { 
     Serial.println(counter1);
  }

  if (Serial.available() > 0) 
  {

    cmd = Serial.read();
    if (cmd=='q') {
          motorFW(motor1);
          FW1 = 1;
    }
  else if (cmd == 'Q')  {
          motorBW(motor1);
        FW1 = -1;
  }
 startTime = millis();
  }

   /* switch (cmd) {

    case 'q':
      motorFW(motor1);
      break;
    case 'a':
      motorBW(motor1);
      break;   
    case 'w':
      motorFW(motor2);
      motorFW(motor3);      
      break;
    case 's':
      motorBW(motor2);
      motorBW(motor3);     
      break;     
    case 'e':
      motorFW(motor2);
      motorBW(motor3);      
      break;
    case 'd':
      motorBW(motor2);
      motorFW(motor3);     
      break;   
    case 'r':
      motorFW(motor4);
      motorFW(motor5);      
      break;
    case 'f':
      motorBW(motor4);
      motorBW(motor5);     
      break;     
    case 't':
      motorFW(motor4);
      motorBW(motor5);      
      break;
    case 'g':
      motorBW(motor4);
      motorFW(motor5);     
      break;      
    case 'y':
      motorFW(motor6);
      break;
    case 'h':
      motorBW(motor6);
      break;  
      case 'u':
      digitalWrite(LEDPIN,HIGH);
      break;
      case 'j':
      digitalWrite(LEDPIN,LOW);
      break;      
    }
    delay(DELAY);
    motorST(motor1);
    motorST(motor2);
    motorST(motor3);
    motorST(motor4);
    motorST(motor5);
    motorST(motor6);

  }
*/

 
}



void motorFW(const int motor[2]){
  digitalWrite(motor[0], HIGH);
  digitalWrite(motor[1], LOW);
}
void motorBW(const int motor[2]){
  digitalWrite(motor[0], LOW);
  digitalWrite(motor[1], HIGH);
}
void motorST(const int motor[2]){
  digitalWrite(motor[0], LOW);
  digitalWrite(motor[1], LOW);
}

void resetCounter()
{
 // if (!digitalRead(MS1))
//{
counter1=0;
//}
  
}

