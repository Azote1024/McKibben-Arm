#include <Arduino.h>
#include <math.h>

#include <MsTimer2.h>

volatile int p_now = 0;
void dataPrint() {
  p_now = map(analogRead(A5),  516, 823, 0, 500);
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
  checkPoint = millis();
  PORTB = B00000010 | (PORTD & B11111100);
  while(millis() - checkPoint < 55);
  PORTB = B00000000 | (PORTD & B11111100);
  delay(1000);
  while(p_now >= 235);
}

void loop() {

  //250kPa始点でどれくらい上昇するか？

  //--初期設定--//
  exhaust(500UL);
  allClose(500UL);

  //--250始点1ms開閉--//
  set250kPa();
  intake(1UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);
  
  //--250始点2ms開閉--//
  set250kPa();
  intake(2UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点3ms開閉--//
  set250kPa();
  intake(3UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点5ms開閉--//
  set250kPa();
  intake(5UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点10ms開閉--//
  set250kPa();
  intake(10UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点20ms開閉--//
  set250kPa();
  intake(20UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点30ms開閉--//
  set250kPa();
  intake(30UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点40ms開閉--//
  set250kPa();
  intake(40UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点50ms開閉--//
  set250kPa();
  intake(50UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点60ms開閉--//
  set250kPa();
  intake(60UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点70ms開閉--//
  set250kPa();
  intake(70UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点80ms開閉--//
  set250kPa();
  intake(80UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点90ms開閉--//
  set250kPa();
  intake(90UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点100ms開閉--//
  set250kPa();
  intake(100UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点150ms開閉--//
  set250kPa();
  intake(150UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点200ms開閉--//
  set250kPa();
  intake(200UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点250ms開閉--//
  set250kPa();
  intake(250UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点300ms開閉--//
  set250kPa();
  intake(300UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点500ms開閉--//
  set250kPa();
  intake(500UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  //--250始点1000ms開閉--//
  set250kPa();
  intake(1000UL);
  allClose(2000UL);
  exhaust(500UL);
  allClose(500UL);

  MsTimer2::stop();
  while(true);
}
