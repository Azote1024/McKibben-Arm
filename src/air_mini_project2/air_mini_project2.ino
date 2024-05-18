#include <Arduino.h>
#include <MsTimer2.h>

#define OPEN 1
#define CLOSE 0

#define intake 6
#define exhaust 7

unsigned long looptime;
volatile int data[850];
volatile int index = 0;

void push(int datam){
  if(index >= 800) {MsTimer2::stop();}
  data[index++] = datam;
}

void sampling() {
  push(analogRead(A5));
}

void setup() {
  DDRD = DDRD | B11111100;
  pinMode(A5, INPUT);

  Serial.begin(115200);
  delay(1000);

  MsTimer2::set(1, sampling);
  MsTimer2::start();
}

void loop() {
  index = 0;
  looptime = millis();
  
  PORTD = B00000000 | (PORTD & B00000011);
  while((long)(millis() - looptime) < 200UL) {
    delayMicroseconds(1);
  }

  PORTD = B01000000 | (PORTD & B00000011);
  while((long)(millis() - looptime) < 400UL) {
    delayMicroseconds(1);
  }
  
  PORTD = B00000000 | (PORTD & B00000011);
  while((long)(millis() - looptime) < 600UL) {
    delayMicroseconds(1);
  }
  
  PORTD = B10000000 | (PORTD & B00000011);
  while((long)(millis() - looptime) < 800UL) {
    delayMicroseconds(1);
  }
  
  for(int i=0; i<index; i++) {
    Serial.println(data[i]);
  }
  while(true);
}
