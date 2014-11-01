#define LEDPIN 49
#define TESTLED 1

#include "Encoder_Scorbot.h"

unsigned long startTime;
// —————————————————————————  MicroSwitches
const int MS[6] = {
  2,3,21,20,19,18};
volatile boolean  MSpressed[6]={
  0,0,0,0,0,0};    
boolean initMTRCCW[6] = {
  false, false, true,false,false,false};

// —————————————————————————  Encoders
const int pin_P0[6] = {
  22,24,26,28,30,32};  // Encoder input pins
const int pin_P1[6] = {
  23,25,27,29,31,33};
volatile long  counter[6]={
  0,0,0,0,0,0};
// —————————————————————————  Motors
const int M1[6] = {
  14,4,6,8,10,12};
const int M2[6] = {
  15,5,7,9,11,13};



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
    pinMode(MS[i],INPUT);
  }
  attachInterrupt(0, resetCounter0, FALLING);  
  attachInterrupt(1, resetCounter1, FALLING);  
  attachInterrupt(2, resetCounter2, FALLING);  
  attachInterrupt(3, resetCounter3, FALLING);  
  attachInterrupt(4, resetCounter4, FALLING);  
  //attachInterrupt(5, resetCounter5, FALLING);    
  pinMode(LEDPIN,OUTPUT);

}

// ————————————————————————— MAIN
void loop() {
  char cmd=0;
  //  unsigned long testTime = 500;

  Serial.println("Hi I'm Scorbot! \t What would you like to do?");
  Serial.println("0 - Go Home ");
  Serial.println("1 - Control specific arm ");
  Serial.println("2 - Control all arms ");
  Serial.println("3 - Select Coordinates ");

  while  (Serial.available() == 0);
  cmd = Serial.read();
  switch (cmd) {
  case '0':
    initWrapper();
    break;
  case '1':
    break;
  case '2':
    break;
  case '3':
    break ;  
  }

}
/*

 
 
 if ( millis() - startTime  < testTime  ) {
 for (int i=0;i<6;i++) {
 counter[i] = encoder_data(i);  // First 
 }
 
 }
 else
 { 
 for (int i=0;i<6;i++) {
 motorST(M1[i],M2[i]);
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
 for (int i=5;i<6;i++) {
 motorFW(M1[i],M2[i]);
 }
 }
 else if (cmd == 'Q')  {
 for (int i=5;i<6;i++) {
 motorBW(M1[i],M2[i]);
 }
 }
 else if (cmd == 'a')  {
 int i=3;
 motorBW(M1[i],M2[i]);
 motorFW(M1[i+1],M2[i+1]);    
 }    
 else if (cmd == 'A')  {
 int i=3;
 motorFW(M1[i],M2[i]);
 motorBW(M1[i+1],M2[i+1]);         
 }
 startTime = millis();
 }
 */





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

void resetCounter0(){ 
  counter[0]=0;
  MSpressed[0]=1;
  if (TESTLED) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN,LOW);
    else digitalWrite(LEDPIN,HIGH);
  }
}
void resetCounter1(){  
  counter[1]=0;
  MSpressed[1]=1;
  if (TESTLED) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN,LOW);
    else digitalWrite(LEDPIN,HIGH);
  }
}
void resetCounter2(){  
  counter[2]=0;
  MSpressed[2]=1;
  if (TESTLED) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN,LOW);
    else digitalWrite(LEDPIN,HIGH);
  }
}
void resetCounter3(){  
  counter[3]=0;
  MSpressed[3]=1;
  if (TESTLED) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN,LOW);
    else digitalWrite(LEDPIN,HIGH);
  }
}
void resetCounter4(){  
  counter[4]=0;
  MSpressed[4]=1;
  if (TESTLED) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN,LOW);
    else digitalWrite(LEDPIN,HIGH);
  }
}
void resetCounter5(){  
  counter[5]=0;
  MSpressed[5]=1;
  if (0) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN,LOW);
    else digitalWrite(LEDPIN,HIGH);
  }
}


int init(boolean initMTRCCW[6] ) {


  // —————— Init Motors 0,1,2
  for (int i=0;i<3;i++) {   
    Serial.print("Init Motor #");
    Serial.println(i);
    if (digitalRead(MS[i])) {
      boolean FWdir = initMTRCCW[i];
      if (FWdir)
        motorFW(M1[i],M2[i]);
      else
        motorBW(M1[i],M2[i]);
      MSpressed[i]=false;
      while ( !MSpressed[i] )  {
        if (Serial.available() > 0)  return 0;
      }; 
      motorST(M1[i],M2[i]);
    }
  }
  Serial.println("Init Motor #3+#4 (1)");
  int i=3;
  if (digitalRead(MS[i])) {
    boolean FWdir = initMTRCCW[i];
    if (FWdir) {
      motorFW(M1[i],M2[i]);
      motorBW(M1[i+1],M2[i+1]);
    }
    else
    {
      motorBW(M1[i],M2[i]);
      motorFW(M1[i+1],M2[i+1]);
    }
    MSpressed[i]=false;
    while ( !MSpressed[i] )        if (Serial.available() > 0)  return 0; 
    motorST(M1[i],M2[i]);
    motorST(M1[i+1],M2[i+1]);
  }

  Serial.println("Init Motor #3+#4 (2)");
  if (digitalRead(MS[i+1])) {
    boolean FWdir = initMTRCCW[i+1];
    if (FWdir) {
      motorFW(M1[i],M2[i]);
      motorFW(M1[i+1],M2[i+1]);
    }
    else
    {
      motorBW(M1[i],M2[i]);
      motorBW(M1[i+1],M2[i+1]);
    }
    MSpressed[i+1]=false;
    while ( !MSpressed[i+1] )        if (Serial.available() > 0)  return 0;
    motorST(M1[i],M2[i]);
    motorST(M1[i+1],M2[i+1]);
  }  

  i=5;
  Serial.println("Init GRIPPER");
  boolean FWdir = initMTRCCW[i];
  if (FWdir)
    motorFW(M1[i],M2[i]);
  else
    motorBW(M1[i],M2[i]);
  for (int t=0;t<10;t++) {
    if (Serial.available() > 0)  return 0;
    delay(100); 
  }
  motorST(M1[i],M2[i]);

  return 1;
}

void initWrapper()
{
  boolean initFlag=false;
  while(!initFlag) {  
    initFlag=true;
    Serial.println("select directions of motors 0 , 2 , 5 (example: 011)");
    Serial.println("Motor0 (0-CW , 1-CCW)");
    Serial.println("Motor2 (0-Down , 1-Up)");
    Serial.println("Motor5 (0-Open , 1-Close)");  
    int initMotors[3] ={
      0,2,5        };

    for (int i=0;i<3;i++) {
      while  (Serial.available() == 0);
      char dir = Serial.read();
      if (dir== '0' ) initMTRCCW[initMotors[i]] = false;
      else if (dir == '1' ) initMTRCCW[initMotors[i]] = true;
      else  {
        Serial.println("ERROR! Try Again!");
        initFlag=false;
        cleanSerial();
        break;  
      }
    }
  }
  if (init(initMTRCCW ))   Serial.println("Init DONE!"); 
  else   {
    Serial.println("Init FAIL!!"); 
   cleanSerial();
  } 

}


void cleanSerial() {
  delay(10);
   while  (Serial.available() > 0)    Serial.read(); 
}





