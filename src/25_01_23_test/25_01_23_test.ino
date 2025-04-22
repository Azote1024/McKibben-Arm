#include <Arduino.h>
#include <MsTimer2.h>


void pin_init();
float getPressure(int analogPin);
float getDegree(int analogPin, float offset);

// 10msに一回これを回す
void sampling() {
  Serial.print("p_UI:");
  Serial.print(getPressure(A0));
  Serial.print(" p_UO:");
  Serial.print(getPressure(A1));
  Serial.print(" p_LI:");
  Serial.print(getPressure(A2));
  Serial.print(" p_LO:");
  Serial.print(getPressure(A3));

  Serial.print(" theta1:");
  Serial.print(getDegree(A4, 422));
  Serial.print(" theta2:");
  Serial.print(getDegree(A5, 458));

  Serial.println("");
}

void setup() {
  pin_init();

  MsTimer2::set(10, sampling);
  MsTimer2::start();

  delay(500);
  digitalWrite(2, HIGH);
}

void loop() {
  
}

void pin_init() {
  pinMode (A0, INPUT); // Upper In
  pinMode (A1, INPUT); // Upper Out
  pinMode (A2, INPUT); // Lower In
  pinMode (A3, INPUT); // Lower Out
  
  pinMode (A4, INPUT); // theta1
  pinMode (A5, INPUT); // theta2

  pinMode (2, OUTPUT); // Upper In
  pinMode (3, OUTPUT);
  pinMode (8, OUTPUT); // Upper Out
  pinMode (9, OUTPUT);
  pinMode (10, OUTPUT); // Lower In
  pinMode (11, OUTPUT);
  pinMode (12, OUTPUT); // Lower Out
  pinMode (13, OUTPUT);

  Serial.begin(115200);

  //いちど全部排気してから，閉じる
  digitalWrite(2, LOW);
  digitalWrite(3, HIGH);
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH);
  digitalWrite(10, LOW);
  digitalWrite(11, HIGH);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  delay(1000);
}

float getPressure(int analogPin) {
  int analogValue = analogRead(analogPin);
  float v = (5.0f*(float)analogValue)/1024.0f;
  return max(0.0f, ((v*1034.12)/4.5) - 51.0f);
}

float getDegree(int analogPin, float offset) {
  return (analogRead(analogPin)-offset)*0.32727f;
}
