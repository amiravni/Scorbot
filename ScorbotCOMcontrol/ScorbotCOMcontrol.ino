// NOTES:
// arm2 gets only negative values
// pitch gets only negative values

#define LEDPIN 49
#define TESTLED 1

#include "Encoder_Polling_V2.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 43
Adafruit_NeoPixel strip = Adafruit_NeoPixel(55, PIN, NEO_GRB + NEO_KHZ800);

unsigned long timeStart;
volatile bool calibration = true;
// —————————————————————————  MicroSwitches
const int MS[6] = {
  2, 3, 21, 20, 19, 18
};
volatile boolean  MSpressed[6] = {
  0, 0, 0, 0, 0, 0
};
boolean initMTRCCW[6] = {false, false, true, false, true, true};

// —————————————————————————  Encoders
const int pin_P0[6] = {
  22, 24, 26, 28, 30, 32
};  // Encoder input pins
const int pin_P1[6] = {
  23, 25, 27, 29, 31, 33
};
int motors_current_state[6] = {
  0, 0, 0, 0, 0, 0
};
volatile short counter[6] = {
  0, 0, 0, 0, 0, 0
};
// —————————————————————————  Motors
//const int M1[6] = {
//  14,4,6,8,10,12};
//const int M2[6] = {
//  15,5,7,9,11,13};
const int M2[6] = {14, 4, 44, 8, 11, 12};
const int M1[6] = {15, 5, 7, 9, 10, 13};

const byte  headerChars[3] = {255, 254, 100};
const byte  headerInitChars[3] = {255, 254, 200};
const byte  headerPolyChars[3] = {255, 254, 150};
byte header[3] = {0, 0, 0};
short encoder_positions[6] = {0 , 0, 0, 0, 0, 0};
byte* encoder_position_ptr = (byte*)&encoder_positions;
short init_positions[6] = {0 , 0, 0, 0, 0, 0};
byte* init_position_ptr = (byte*)&init_positions;
short poly_positions[37];
byte* poly_position_ptr = (byte*)&poly_positions;
const int numOfCommandsPoly = 100;
int ptr = 0;

// ————————————————————————— Setup
void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
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
  for (int iii = 0; iii < 37; iii++) {
    poly_positions[iii] = 0;
  }
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
 //rainbow(20);

  /*
    for (int i = 0; i < 5; i++) {
    Serial.print(MSpressed[i]);
    }
    Serial.println("");

    Serial.println("Current Counter:");

    for (int i = 0; i < 6 - 1; i++) {
    Serial.print(counter[i]);
    Serial.print("  ,  ");
    }
    Serial.println(counter[5]);

    Serial.println("Future Counter:");

    for (int i = 0; i < 6 - 1; i++) {
    Serial.print(encoder_positions[i]);
    Serial.print("  ,  ");
    }
    Serial.println(encoder_positions[5]);
  */
}

// ————————————————————————— MAIN



void loop() {
  short dir0[6] = {
    0, 0, 0, 0, 0, 0
  };
  ptr = 0;
  int point_or_poly = -1;
  bool print_encoders = 0;

  /// Serial read
  while (Serial.available() > 0)
  {
    header[0] = (byte)Serial.read();
    if (header[0] != headerChars[0]) {
      Serial.println("break0!");      
      break;
    }
    while (Serial.available() <= 0) {}
    header[1] =  (byte)Serial.read();
    if (header[1] != headerChars[1]) {
      Serial.println("break1!");      
      break;
    }

    while (Serial.available() <= 0) {}
    header[2] =  (byte)Serial.read();
    if (header[2] != headerChars[2] && header[2] != headerInitChars[2] && header[2] != headerPolyChars[2]) {
      Serial.println("break2!");
      break;
    }
    Serial.println("PASS!");
    if (header[2] == headerChars[2]) {
      point_or_poly = 0;
      while (ptr < 6 * sizeof(short)) {
        while (Serial.available() <= 0) {}
        encoder_position_ptr[ptr++] = (byte)Serial.read();  // ptr to byte array of 12, and then we get 6 shorts
      }

      if (encoder_positions[3] != encoder_positions[4]) {
        encoder_positions[4] = encoder_positions[3];
      }
      Serial.print("Srl: ");
      for (int i = 0; i < 5; i++) {
        Serial.print(encoder_positions[i]);
        Serial.print("  ,  ");
      }
      Serial.println(encoder_positions[5]);
      print_encoders = 1;

    }
    if (header[2] == headerInitChars[2]) {
      while (ptr < 6 * sizeof(short)) {
        while (Serial.available() <= 0) {}
        init_position_ptr[ptr++] = (byte)Serial.read();  // ptr to byte array of 12, and then we get 6 shorts
      }

      for ( int iii = 0; iii < 6 ; iii++) {
        if (init_positions[iii]) {
          initMTRCCW[iii] = true;
        }
        else {
          initMTRCCW[iii] = false;
        }
      }
      Serial.print("SrlHome: ");
      for (int i = 0; i < 5; i++) {
        encoder_positions[i] = 0;
        counter[i] = 0;
        Serial.print(init_positions[i]);
        Serial.print("  ,  ");
      }
      Serial.println(init_positions[5]);
      goHome();

      calibration = false;
    }
    if (header[2] == headerPolyChars[2]) {
      point_or_poly = 1;
      while (ptr < (6 * 6 + 1) * sizeof(short)) {
        while (Serial.available() <= 0) {}
        poly_position_ptr[ptr++] = (byte)Serial.read();  // ptr to byte array of 12, and then we get 6 shorts
        //Serial.print((int)poly_position_ptr[ptr - 1]);
        //Serial.print(",");
      }
      Serial.print("SrlPoly: ");
      for (int i = 0; i < 36; i++) {
        Serial.print(poly_position_ptr[i]);
        Serial.print("  ,  ");
      }
      Serial.println(poly_position_ptr[36]);
    }
  }

  int numOfCommands = 1;
  float dT = 0;
  if (point_or_poly == 1) {
    numOfCommands = numOfCommandsPoly;
    dT =  (float)poly_positions[36] / (float)numOfCommands;
    Serial.print("Srldt: ");
    Serial.println(dT);
  }
  if (point_or_poly == 0) numOfCommands = 1;


  for (int nnn = 0; nnn < numOfCommands; nnn++) {
    if (point_or_poly == 1) {
      for (int e = 0; e < 3; e++) {
        float newTime = dT * (float)nnn;
        float encoderTmp = 0;
        encoder_positions[e] = 0;
        for (int p = 0; p < 5; p++) {
          //encoder_positions[e] = (encoder_positions[e] + poly_position[ e*6 + p]/10 ) * newTime;
          encoderTmp = (encoderTmp + (float)poly_positions[ e * 6 + p] / 10.0 ) * newTime;
        }
        encoder_positions[e] = (short)floor(encoderTmp + (float)poly_positions[ e * 6 + 5] / 10.0);
      }
    }

    bool finish_movement_flag = 0;
    timeStart = millis();
    while (!finish_movement_flag) {

      /// arms 0-4 control
      finish_movement_flag = 1;
      for (int i = 0; i < 3; i++) {
        if ( abs(counter[i] - encoder_positions[i]) > 1 ) {

          finish_movement_flag = 0;
          if (counter[i] < encoder_positions[i])  {
            if (motors_current_state[i] != 1) {
              motorFW(M1[i], M2[i]);
              motors_current_state[i] = 1;
            }
          }
          else {
            if (motors_current_state[i] != -1) {
              motorBW(M1[i], M2[i]);
              motors_current_state[i] = -1;
            }
          }
        }
        else {
          if (motors_current_state[i] != 0) {
            motorST(M1[i], M2[i]);
            motors_current_state[i] = 0;
          }
        }

        /// Encoder read
        dir0[i] = encoder_data(i);  // First
        if (dir0[i] != 0) // Check for rotation
        {
          counter[i] += dir0[i];
        }

      }

      /// gripper pitch
      int i = 3;
      if ( abs(counter[i] - encoder_positions[i]) > 5 ) {

        finish_movement_flag = 0;
        if (counter[i] < encoder_positions[i])  {
          if (motors_current_state[i] != 1) {
            motorFW(M1[i], M2[i]);
            motorFW(M1[i + 1], M2[i + 1]);
            motors_current_state[i] = 1;
          }
        }
        else {
          if (motors_current_state[i] != -1) {
            motorBW(M1[i], M2[i]);
            motorBW(M1[i + 1], M2[i + 1]);
            motors_current_state[i] = -1;
          }
        }
      }
      else {
        if (motors_current_state[i] != 0) {
          motorST(M1[i], M2[i]);
          motorST(M1[i + 1], M2[i + 1]);
          motors_current_state[i] = 0;
        }
      }

      /// Encoder read
      dir0[i] = encoder_data(i);  // First
      if (dir0[i] != 0) // Check for rotation
      {
        counter[i] += dir0[i];
      }

      /// Gripper control
      if ( abs(counter[5] - encoder_positions[5]) > 5 ) {
        finish_movement_flag = 0;
        if (millis() -  timeStart <  abs(encoder_positions[5])) {
          if (counter[5] < encoder_positions[5])  {
            if (motors_current_state[5] != 1) {
              motorFW(M1[5], M2[5]);
              motors_current_state[5] = 1;
            }
          }
          else {
            if (motors_current_state[5] != -1) {
              motorBW(M1[5], M2[5]);
              motors_current_state[5] = -1;
            }
          }
        }
        else {
          if (motors_current_state[5] != 0) {
            motorST(M1[5], M2[5]);
            motors_current_state[5] = 0;
            counter[5] = encoder_positions[5];
          }
        }
      }
    }
  }
  /// print encoders
  if (print_encoders) {
    Serial.print("Enc: ");
    for (int i = 0; i < 5; i++) {
      Serial.print(counter[i]);
      Serial.print("  ,  ");
    }
    Serial.println(counter[5]);
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
  if (calibration)  counter[0] = 0;
  MSpressed[0] = 1;
  if (TESTLED) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN, LOW);
    else digitalWrite(LEDPIN, HIGH);
  }
}
void resetCounter1() {
  if (calibration)  counter[1] = 0;
  MSpressed[1] = 1;
  if (TESTLED) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN, LOW);
    else digitalWrite(LEDPIN, HIGH);
  }
}
void resetCounter2() {
  if (calibration)  counter[2] = 0;
  MSpressed[2] = 1;
  if (TESTLED) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN, LOW);
    else digitalWrite(LEDPIN, HIGH);
  }
}
void resetCounter3() {
  if (calibration) counter[3] = 0;
  if (calibration) counter[4] = 0;  // We obly use pitch and not roll
  MSpressed[3] = 1;
  if (TESTLED) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN, LOW);
    else digitalWrite(LEDPIN, HIGH);
  }
}
void resetCounter4() {
  //counter[4] = 0;
  MSpressed[4] = 1;
  if (TESTLED) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN, LOW);
    else digitalWrite(LEDPIN, HIGH);
  }
}
void resetCounter5() {
  if (calibration) counter[5] = 0;
  MSpressed[5] = 1;
  if (0) {
    if (digitalRead(LEDPIN)) digitalWrite(LEDPIN, LOW);
    else digitalWrite(LEDPIN, HIGH);
  }
}


void goHome() {
  // —————— Init Motors 2
  for (int i = 2; i < 3; i++) {
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
      motorFW(M1[i + 1], M2[i + 1]);
    }
    else
    {
      motorBW(M1[i], M2[i]);
      motorBW(M1[i + 1], M2[i + 1]);
    }
    MSpressed[i] = false;
    while ( !MSpressed[i] );
    motorST(M1[i], M2[i]);
    motorST(M1[i + 1], M2[i + 1]);
  }

/*
  Serial.println("Init Motor #3+#4 (2)");
  if (digitalRead(MS[i + 1])) {
    boolean FWdir = initMTRCCW[i + 1];
    if (FWdir) {
      motorFW(M1[i], M2[i]);
      motorBW(M1[i + 1], M2[i + 1]);
    }
    else
    {
      motorBW(M1[i], M2[i]);
      motorFW(M1[i + 1], M2[i + 1]);
    }
    MSpressed[i + 1] = false;
    while ( !MSpressed[i + 1] );
    motorST(M1[i], M2[i]);
    motorST(M1[i + 1], M2[i + 1]);
  }
*/
  Serial.println("Init DONE!");
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
