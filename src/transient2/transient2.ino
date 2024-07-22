#include <Arduino.h>
#include <math.h>

#include <MsTimer2.h>

volatile int p_now = 0;
void dataPrint() {
  p_now = map(analogRead(A5),  518, 823, 0, 500);
  //p_now = analogRead(A5);
  Serial.print(p_now);
  Serial.print(",");
  Serial.println(PORTB);
}

unsigned long checkPoint = 0;
void setup() {
  DDRD = DDRD | B11111100;
  DDRB = DDRB | B00000011;
  DDRC = DDRC & B00000011;
  
  Serial.begin(115200);
  delay(1000);

  MsTimer2::set(1, dataPrint);
  MsTimer2::start();
}

void allClose(unsigned long span){
  checkPoint = millis();
  PORTB = B00000000 | (PORTD & B11111100);
  while(millis() - checkPoint < span);
  }
void allOpen (unsigned long span){
  checkPoint = millis();
  PORTB = B00000011 | (PORTD & B11111100);
  while(millis() - checkPoint < span);
  }
void intake  (unsigned long span){
  checkPoint = millis();
  PORTB = B00000010 | (PORTD & B11111100);
  while(millis() - checkPoint < span);
  }
void exhaust (unsigned long span){
  checkPoint = millis();
  PORTB = B00000001 | (PORTD & B11111100);
  while(millis() - checkPoint < span);
  }
void set250kPa() {
  exhaust(500);
  checkPoint = millis();
  PORTB = B00000010 | (PORTD & B11111100);
  while(millis() - checkPoint < 62);
  PORTB = B00000000 | (PORTD & B11111100);
  delay(1000);
  while(p_now >= 235);
}

void loop() {
  exhaust(500);
  allClose(500);

  //給気開放1000ms
  intake(1000);
  allClose(2000);

  //排気開放600ms
  intake(1000);
  allClose(50);
  exhaust(1000);
  allClose(2000);

  //給気時間幅60ms
  intake(30);
  allClose(2000);

  //排気時間幅60ms
  intake(1000);
  allClose(30);
  exhaust(30);
  allClose(2000);
  
  

  MsTimer2::stop();
  while(true);
}
