#include <Arduino.h>
#include <math.h>

void setup() {
  Serial.begin(115200);

  //このプログラムは基本的にUNO向けなので注意
  DDRD = DDRD | B11111100;
  DDRB = DDRB | B00000011;

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  
}

int target = 0;
int theta1;
int theta2;
void loop() {
  theta1 = (analogRead(A1)- 286.0) / 256.0 * 90;
  theta2 = (analogRead(A0) - 164.0) / 256.0 * 90;
  Serial.print(theta1);
  Serial.print(", ");
  Serial.println(theta2);

  target = 10;
  if (theta2 > (target + 5)) {
    digitalWrite(9, 0);
    digitalWrite(8, 1);
    digitalWrite(7, 1);
    digitalWrite(6, 0);
  } 
  else if (theta2 < (target - 5)){
    digitalWrite(9, 1);
    digitalWrite(8, 0);
    digitalWrite(7, 0);
    digitalWrite(6, 1);
  }
  else {
    digitalWrite(9, 0);
    digitalWrite(8, 0);
    digitalWrite(7, 0);
    digitalWrite(6, 0);
  }
  
}
