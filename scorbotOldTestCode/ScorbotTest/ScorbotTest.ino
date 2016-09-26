#define LEDPIN 49
#define TESTLED 1

#include "Encoder_Polling_V2.h"

unsigned long startTime;
// —————————————————————————  MicroSwitches
const int MS[6] = {
  2, 3, 21, 20, 19, 18
};
volatile boolean  MSpressed[6] = {
  0, 0, 0, 0, 0, 0
};
boolean initMTRCCW[6] = {false, false, false, false, true, true};

// —————————————————————————  Encoders
const int pin_P0[6] = {
  22, 24, 26, 28, 30, 32
};  // Encoder input pins
const int pin_P1[6] = {
  23, 25, 27, 29, 31, 33
};
int FW[6] = {
  0, 0, 0, 0, 0, 0
};
volatile long  counter[6] = {
  0, 0, 0, 0, 0, 0
};
// —————————————————————————  Motors
//const int M1[6] = {
//  14,4,6,8,10,12};
//const int M2[6] = {
//  15,5,7,9,11,13};
const int M2[6] = {14, 4, 44, 8, 10, 12};
const int M1[6] = {15, 5, 7, 9, 11, 13};

const byte  headerChars[3] = {255, 254, 100};
byte header[3] = {0, 0, 0};

// ————————————————————————— Setup
void setup() {
  Serial.begin(9600);
  encoder_begin();  // Start the library
  for (int i = 0; i < 6; i++) {
    attach_encoder(i, pin_P0[i], pin_P1[i]);  // Attach an encoder to pins A and B
    pinMode(pin_P0[i], INPUT_PULLUP);
    pinMode(pin_P1[i], INPUT_PULLUP);
    pinMode(M1[i], OUTPUT);
    pinMode(M2[i], OUTPUT);
    motorST(M1[i], M2[i]);
    pinMode(MS[i], INPUT);
  }
  for (int i = 0; i < 5; i++) {
    Serial.print(MSpressed[i]);
  }
  Serial.println(MSpressed[6]);

  attachInterrupt(0, resetCounter0, FALLING);
  attachInterrupt(1, resetCounter1, FALLING);
  attachInterrupt(2, resetCounter2, FALLING);
  attachInterrupt(3, resetCounter3, FALLING);
  attachInterrupt(4, resetCounter4, FALLING);
  //attachInterrupt(5, resetCounter5, FALLING);
  pinMode(LEDPIN, OUTPUT);

  for (int i = 0; i < 5; i++) {
    Serial.print(MSpressed[i]);
  }
  Serial.println(MSpressed[6]);

  // —————— Init Motors 0,1,2
  for (int i = 0; i < 3; i++) {
    Serial.print("Init Motor #");
    Serial.println(i);
    if (digitalRead(MS[i])) {
      boolean FWdir = initMTRCCW[i];
      if (FWdir)
        motorFW(M1[i], M2[i]);
      else
        motorBW(M1[i], M2[i]);
      MSpressed[i] = false;
      while ( !MSpressed[i] );
      motorST(M1[i], M2[i]);
    }
  }
  Serial.println("Init Motor #3+#4 (1)");
  int i = 3;
  if (digitalRead(MS[i])) {
    boolean FWdir = initMTRCCW[i];
    if (FWdir) {
      motorFW(M1[i], M2[i]);
      motorBW(M1[i + 1], M2[i + 1]);
    }
    else
    {
      motorBW(M1[i], M2[i]);
      motorFW(M1[i + 1], M2[i + 1]);
    }
    MSpressed[i] = false;
    while ( !MSpressed[i] );
    motorST(M1[i], M2[i]);
    motorST(M1[i + 1], M2[i + 1]);
  }

  Serial.println("Init Motor #3+#4 (2)");
  if (digitalRead(MS[i + 1])) {
    boolean FWdir = initMTRCCW[i + 1];
    if (FWdir) {
      motorFW(M1[i], M2[i]);
      motorFW(M1[i + 1], M2[i + 1]);
    }
    else
    {
      motorBW(M1[i], M2[i]);
      motorBW(M1[i + 1], M2[i + 1]);
    }
    MSpressed[i + 1] = false;
    while ( !MSpressed[i + 1] );
    motorST(M1[i], M2[i]);
    motorST(M1[i + 1], M2[i + 1]);
  }

  Serial.println("Init DONE!");
  for (int i = 0; i < 5; i++) {
    Serial.print(MSpressed[i]);
  }
  Serial.println(MSpressed[6]);
}








// ————————————————————————— MAIN



void loop() {
  char cmd = 0;
  int dir0[6] = {
    0, 0, 0, 0, 0, 0
  };
  unsigned long testTime = 300;

  while (Serial.available() > 0)
  {
    header[0] = (byte)Serial.read();
    if (header[0] != headerChars[0]) {
      break;
    }
    while (Serial.available() <= 0) {}
    header[1] =  (byte)Serial.read();
    if (header[1] != headerChars[1]) {
      break;
    }

    while (Serial.available() <= 0) {}
    header[2] =  (byte)Serial.read();
    if (header[2] != headerChars[2]) {
      break;
    }


    short encoder_positions[6];
    byte * encoder_position_ptr = &encoder_positions;
    int ptr = 0;

    ptr = 0;
    while (ptr < 6*sizeof(short)){
      while (Serial.available() <= 0) {}
      encoder_position_ptr[ptr++] = (byte)Serial.read();
    }
    
  }



  if ( millis() - startTime  < testTime  ) {

    for (int i = 0; i < 6; i++) {
      dir0[i] = encoder_data(i);  // First
      if (dir0[i] != 0) // Check for rotation
      {
        counter[i] += dir0[i];
      }
    }

  }
  else
  {
    for (int i = 0; i < 6; i++) {
      motorST(M1[i], M2[i]);
      FW[i] = 0;
    }
  }

  if ( (millis() - startTime  > testTime + 50 ) && ( millis() - startTime < testTime + 53 ) ) {
    for (int i = 0; i < 6 - 1; i++) {
      Serial.print(counter[i]);
      Serial.print("  ,  ");
    }
    Serial.println(counter[5]);
  }

  if (Serial.available() > 0)
  {

    cmd = Serial.read();
    if (cmd == 'q') {
      for (int i = 0; i < 6; i++) {
        motorFW(M1[i], M2[i]);
        FW[i] = 1;
      }
    }
    else if (cmd == 'Q')  {
      for (int i = 0; i < 6; i++) {
        motorBW(M1[i], M2[i]);
        FW[i] = -1;
      }
    }
    else if (cmd == 'a')  {
      int i = 0;
      motorFW(M1[i], M2[i]);
      // motorFW(M1[i+1],M2[i+1]);
    }
    else if (cmd == 'A')  {
      int i = 0;
      motorBW(M1[i], M2[i]);
      //   motorBW(M1[i+1],M2[i+1]);
    }
    else if (cmd == 's')  {
      int i = 1;
      motorFW(M1[i], M2[i]);
      // motorFW(M1[i+1],M2[i+1]);
    }
    else if (cmd == 'S')  {
      int i = 1;
      motorBW(M1[i], M2[i]);
      //   motorBW(M1[i+1],M2[i+1]);
    }
    else if (cmd == 'd')  {
      int i = 2;
      motorFW(M1[i], M2[i]);
      // motorFW(M1[i+1],M2[i+1]);
    }
    else if (cmd == 'D')  {
      int i = 2;
      motorBW(M1[i], M2[i]);
      //   motorBW(M1[i+1],M2[i+1]);
    }
    else if (cmd == 'f')  {
      int i = 3;
      motorFW(M1[i], M2[i]);
      motorFW(M1[i + 1], M2[i + 1]);
    }
    else if (cmd == 'F')  {
      int i = 3;
      motorBW(M1[i], M2[i]);
      motorBW(M1[i + 1], M2[i + 1]);
    }
    else if (cmd == 'g')  {
      int i = 3;
      motorFW(M1[i], M2[i]);
      motorBW(M1[i + 1], M2[i + 1]);
    }
    else if (cmd == 'G')  {
      int i = 3;
      motorBW(M1[i], M2[i]);
      motorFW(M1[i + 1], M2[i + 1]);
    }
    else if (cmd == 'h')  {
      int i = 5;
      motorFW(M1[i], M2[i]);
      //    motorFW(M1[i+1],M2[i+1]);
    }
    else if (cmd == 'H')  {
      int i = 5;
      motorBW(M1[i], M2[i]);
      //   motorBW(M1[i+1],M2[i+1]);
    }
    startTime = millis();
  }

}



void motorFW(const int mtr1, const int mtr2  ) {
  digitalWrite(mtr1, HIGH);
  digitalWrite(mtr2, LOW);
}
void motorBW(const int mtr1, const int mtr2  ) {
  digitalWrite(mtr2, HIGH);
  digitalWrite(mtr1, LOW);
}
void motorST(const int mtr1, const int mtr2  ) {
  digitalWrite(mtr1, LOW);
  digitalWrite(mtr2, LOW);
}

void resetCounter0() {
  counter[0] = 0;
  MSpressed[0] = 1;
  if (TESTLED) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN, LOW);
    else digitalWrite(LEDPIN, HIGH);
  }
}
void resetCounter1() {
  counter[1] = 0;
  MSpressed[1] = 1;
  if (TESTLED) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN, LOW);
    else digitalWrite(LEDPIN, HIGH);
  }
}
void resetCounter2() {
  counter[2] = 0;
  MSpressed[2] = 1;
  if (TESTLED) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN, LOW);
    else digitalWrite(LEDPIN, HIGH);
  }
}
void resetCounter3() {
  counter[3] = 0;
  MSpressed[3] = 1;
  if (TESTLED) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN, LOW);
    else digitalWrite(LEDPIN, HIGH);
  }
}
void resetCounter4() {
  counter[4] = 0;
  MSpressed[4] = 1;
  if (TESTLED) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN, LOW);
    else digitalWrite(LEDPIN, HIGH);
  }
}
void resetCounter5() {
  counter[5] = 0;
  MSpressed[5] = 1;
  if (0) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN, LOW);
    else digitalWrite(LEDPIN, HIGH);
  }
}




