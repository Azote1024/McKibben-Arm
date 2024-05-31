#include <Arduino.h>
#include <math.h>

#define INNER_ARM_LENGTH 230
#define OUTER_ARM_LENGTH 225

double length_LI (double theta2) {
  double L1 = 175.0;
  double L2 = 50.0;
  double d = 15.0;
  double l_2 = pow(L1*cos(theta2) - d*sin(theta2) + L2, 2);
        l_2 += pow(L1*sin(theta2) + d*cos(theta2) - d, 2);
  return constrain(225 - sqrt(l_2), 0, 35);
}

double length_LO (double theta2) {
  double L = 95.0;
  double d = 15.0;
  double r60 = radians(60);
  double l_2  = pow(d*cos(theta2 + r60) - L, 2);
         l_2 += pow(d*sin(theta2 + r60) - d, 2);
  return constrain(25 - (sqrt(l_2)- 95 + 7.5), 0, 25);
}

double length_UI (double theta1) {
  double L1 = 155.0;
  double L2 = 45.0;
  double d = 15.0;
  double l_2  = pow(L1*cos(theta1) - d*sin(theta1) + L2, 2);
         l_2 += pow(L1*sin(theta1) + d*cos(theta1) - d, 2);
  return constrain(210 - sqrt(l_2), 0, 35);
}

double length_UO (double theta1) {
  double L1 = 20.0;
  double L2 = 20.0;
  double d = 15.0;
  double l_2  = pow(L2*cos(theta1) - d*sin(theta1) + L1, 2);
         l_2 += pow(L2*sin(theta1) - d*cos(theta1) + d, 2);
  return constrain(sqrt(l_2) - 15, 0, 25);
}

void setup() {
  //このプログラムは基本的にUNO向けなので注意
  DDRD = DDRD | B11111100;
  DDRB = DDRB | B00000011;

  //人工筋内圧
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  //リンク内角
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  
  Serial.begin(115200);
  delay(1000);
}

int theta1 = 0;
int theta2 = 0;
void loop() {
  theta1 = (analogRead(A1)- 286.0) / 256.0 * 90;
  theta2 = (analogRead(A0) - 164.0) / 256.0 * 90;

  Serial.println(length_UI(theta1));
}
