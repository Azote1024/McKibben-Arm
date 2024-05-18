#include <Arduino.h>
#include <math.h>

#define OPEN 1
#define CLOSE 0

void setup() {
  // put your setup code here, to run once:
  DDRD = DDRD | B11111100;
  pinMode(A5, INPUT);
  randomSeed(12345);

  Serial.begin(115200);
  delay(5000);
}

void loop() {
  PORTD = B10000000 | (PORTD & B00000011);
  delay(1000);
  PORTD = B00000000 | (PORTD & B00000011);
  
  for (int i=0; i<15; i++) {
    PORTD = B01000000 | (PORTD & B00000011);
    delay(5);
    PORTD = B00000000 | (PORTD & B00000011);
    delay(50);
    Serial.println(analogRead(A5));
    delay(1000);
  }
  
  //排気して終了
  PORTD = B10000000 | (PORTD & B00000011);
  delay(1000);
  PORTD = B00000000 | (PORTD & B00000011);
}
