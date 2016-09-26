int m1[6] = {14, 4, 22, 8, 10, 12};
int m2[6] = {15, 5, 7, 9, 11, 13};
int rgb[3] = {44, 45, 46};
void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < 6; i++)
  {
    pinMode(m1[i], OUTPUT);
    digitalWrite(m1[i], LOW);
    pinMode(m2[i], OUTPUT);
    digitalWrite(m1[2], LOW);

  }
  for (int i = 0; i < 3; i++)
  {
    pinMode(rgb[i], OUTPUT);
    digitalWrite(rgb[i], LOW);
  }

  Serial.begin(38400);
  delay(10);

  Serial.println("Starting...");
for (int i = 0; i < 3; i++)
  {
    Serial.print("RGB ");
    Serial.print(i);
    Serial.println(" on");
     digitalWrite(rgb[i], HIGH);
    delay(1000);
    Serial.print("RGB ");
    Serial.print(i);
    Serial.println(" off");
     digitalWrite(rgb[i], LOW);
     delay(1000);
  }
  for (int i = 0; i < 6; i++)
  {
    Serial.print("Motor ");
    Serial.print(i);
    Serial.println(" Left");
    motorOn(i, 1);
    delay(1000);
    Serial.print("Motor ");
    Serial.print(i);
    Serial.println(" Right");
    motorOn(i, 2);
    delay(1000);
    Serial.print("Motor ");
    Serial.print(i);
    Serial.println(" stop");
    motorOff(i);
    delay(1000);


  }
}

void loop() {
  // put your main code here, to run repeatedly:

}

void motorOn(int motorNum, int dir)
{
  if (dir == 1)
  {
    digitalWrite(m1[motorNum], LOW);
    digitalWrite(m2[motorNum], HIGH);
  } else   {
    digitalWrite(m2[motorNum], LOW);
    digitalWrite(m1[motorNum], HIGH);
  }
}


void motorOff(int motorNum)
{
  digitalWrite(m1[motorNum], LOW);
  digitalWrite(m2[motorNum], LOW);
}

