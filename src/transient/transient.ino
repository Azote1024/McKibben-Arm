//人工筋に圧力を加えた際の過渡応答を測って立ち上がり・立ち下がりの時定数を求める

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

  
}

void loop() {
  index = 0;
  looptime = millis();

  MsTimer2::set(1, sampling);
  MsTimer2::start();
  
  PORTB = B00000000 | (PORTD & B11111100);
  while((long)(millis() - looptime) < 200UL) {
    delayMicroseconds(1);
  }

  PORTB = B00000010 | (PORTD & B11111100);
  while((long)(millis() - looptime) < 400UL) {
    delayMicroseconds(1);
  }


  PORTB = B00000000 | (PORTD & B11111100);
  while((long)(millis() - looptime) < 600UL) {
    delayMicroseconds(1);
  }
  
  PORTB = B00000001 | (PORTD & B11111100);
  while((long)(millis() - looptime) < 800UL) {
    delayMicroseconds(1);
  }
  
  for(int i=0; i<index; i++) {
    Serial.println(data[i]);
  }

  long minor = 0;
  long maximum = 0;

  //圧力0のときを求めるためにデータの200番までを平均
  for(int i=0; i<200; i++) {
    minor += data[i];
  }
  minor /= 200;
  //Serial.print("min = ");
  //Serial.println(minor);
  
  //データ中の400番から600番までを静定後としてこれを平均
  for (int i=400; i<600; i++) {
      maximum += data[i];
  }
  maximum /= 200;
  //Serial.print("max = ");
  //Serial.println(maximum);

  //----------------//立ち上がり時の時定数を求める//----------------//

  int tau_p = 1;
  for(int i=0; i<400; i++) {
    //最小より10大きくなったら立ち上がりはじめとみなす
    if (data[i] > minor + 10) {
      tau_p++;
      //63.2%を上回ったら脱出
      int th = ((maximum - minor) * 0.632) + minor;
      if ( data[i] > th )break;
    }
  }
  //Serial.print("tau_p = ");
  //Serial.println(tau_p);
  
  //----------------//立ち下がり時の時定数を求める//----------------//

  int tau_n = 1;
  for(int i=400; i<800; i++) {
    //最大より10小さくなったら立ち上がりはじめとみなす
    if (data[i] < maximum - 10) {
      tau_n++;
      //63.2%を下回ったら脱出
      int th = ((maximum - minor) * 0.368) + minor;
      if ( data[i] < th )break;
    }
  }
  //Serial.print("tau_n = ");
  //Serial.println(tau_n);
  
  while(true);
}
