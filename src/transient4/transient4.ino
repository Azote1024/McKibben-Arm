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

void loop() {

  //最大圧始点でどれくらい下降するか？

  //--初期設定--//
  exhaust(500UL);
  allClose(500UL);

  //--大気圧始点1ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(1UL);
  allClose(2000UL);
  
  //--大気圧始点2ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(2UL);
  allClose(2000UL);

  //--大気圧始点3ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(3UL);
  allClose(2000UL);

  //--大気圧始点4ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(4UL);
  allClose(2000UL);

  //--大気圧始点5ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(5UL);
  allClose(2000UL);

  //--大気圧始点6ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(6UL);
  allClose(2000UL);

  //--大気圧始点7ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(7UL);
  allClose(2000UL);

  //--大気圧始点8ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(8UL);
  allClose(2000UL);

  //--大気圧始点9ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(9UL);
  allClose(2000UL);

  //--大気圧始点10ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(10UL);
  allClose(2000UL);

  //--大気圧始点20ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(20UL);
  allClose(2000UL);

  //--大気圧始点30ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(30UL);
  allClose(2000UL);

  //--大気圧始点40ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(40UL);
  allClose(2000UL);

  //--大気圧始点50ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(50UL);
  allClose(2000UL);

  //--大気圧始点60ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(60UL);
  allClose(2000UL);

  //--大気圧始点70ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(70UL);
  allClose(2000UL);

  //--大気圧始点80ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(80UL);
  allClose(2000UL);

  //--大気圧始点90ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(90UL);
  allClose(2000UL);

  //--大気圧始点100ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(100UL);
  allClose(2000UL);

  //--大気圧始点200ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(200UL);
  allClose(2000UL);

  //--大気圧始点300ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(300UL);
  allClose(2000UL);

  //--大気圧始点500ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(500UL);
  allClose(2000UL);

  //--大気圧始点1000ms開閉--//
  intake(2000UL);
  allClose(20UL);
  exhaust(1000UL);
  allClose(2000UL);

  MsTimer2::stop();
  while(true);
}
