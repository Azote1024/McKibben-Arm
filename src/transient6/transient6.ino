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
  exhaust(500);
  checkPoint = millis();
  PORTB = B00000010 | (PORTD & B11111100);
  while(millis() - checkPoint < 62);
  PORTB = B00000000 | (PORTD & B11111100);
  delay(1000);
  while(p_now >= 235);
}

void loop() {

  //250kPa始点でどれくらい下降するか？

  //--初期設定--//
  exhaust(500UL);
  allClose(500UL);

  //--250始点1ms開閉--//
  set250kPa();
  exhaust(1UL);
  allClose(2000UL);
  
  //--250始点2ms開閉--//
  set250kPa();
  exhaust(2UL);
  allClose(2000UL);

  //--250始点3ms開閉--//
  set250kPa();
  exhaust(3UL);
  allClose(2000UL);

  //--250始点4ms開閉--//
  set250kPa();
  exhaust(4UL);
  allClose(2000UL);

  //--250始点5ms開閉--//
  set250kPa();
  exhaust(5UL);
  allClose(2000UL);

  //--250始点6ms開閉--//
  set250kPa();
  exhaust(6UL);
  allClose(2000UL);

  //--250始点7ms開閉--//
  set250kPa();
  exhaust(7UL);
  allClose(2000UL);

  //--250始点8ms開閉--//
  set250kPa();
  exhaust(8UL);
  allClose(2000UL);

  //--250始点9ms開閉--//
  set250kPa();
  exhaust(9UL);
  allClose(2000UL);

  //--250始点10ms開閉--//
  set250kPa();
  exhaust(10UL);
  allClose(2000UL);

  //--250始点20ms開閉--//
  set250kPa();
  exhaust(20UL);
  allClose(2000UL);

  //--250始点30ms開閉--//
  set250kPa();
  exhaust(30UL);
  allClose(2000UL);

  //--250始点40ms開閉--//
  set250kPa();
  exhaust(40UL);
  allClose(2000UL);

  //--250始点50ms開閉--//
  set250kPa();
  exhaust(50UL);
  allClose(2000UL);

  //--250始点60ms開閉--//
  set250kPa();
  exhaust(60UL);
  allClose(2000UL);

  //--250始点70ms開閉--//
  set250kPa();
  exhaust(70UL);
  allClose(2000UL);

  //--250始点80ms開閉--//
  set250kPa();
  exhaust(80UL);
  allClose(2000UL);

  //--250始点90ms開閉--//
  set250kPa();
  exhaust(90UL);
  allClose(2000UL);

  //--250始点100ms開閉--//
  set250kPa();
  exhaust(100UL);
  allClose(2000UL);

  //--250始点200ms開閉--//
  set250kPa();
  exhaust(200UL);
  allClose(2000UL);

  //--250始点300ms開閉--//
  set250kPa();
  exhaust(300UL);
  allClose(2000UL);

  //--250始点500ms開閉--//
  set250kPa();
  exhaust(500UL);
  allClose(2000UL);

  //--250始点1000ms開閉--//
  set250kPa();
  exhaust(1000UL);
  allClose(2000UL);

  MsTimer2::stop();
  while(true);
}
