#include <Arduino.h>
#include <math.h>

unsigned long startmillis = 0;

void setup() {
  Serial.begin(115200);

  //このプログラムは基本的にUNO向けなので注意
  DDRD = DDRD | B11111100;
  DDRB = DDRB | B00000011;

  //関節ポテンショ
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  //圧力センサ
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  startmillis = millis();
}

int target = 600;
int pressure = 0;

unsigned long stopwatch = 0;

int opentime = 0;


const int ms = 1000;

void loop() {
  stopwatch = millis();
  
  pressure = map(constrain(analogRead(A5),   513, 779),   513, 779, 0, 500);

  target = 250 + 250*sin((millis() * 2UL * PI)/4000);

  Serial.println(pressure);

  
  if (pressure > target) {
    digitalWrite(8, 1);
    digitalWrite(9, 0);
  } 
  else if (pressure < target){
    digitalWrite(8, 0);
    digitalWrite(9, 1);
    
  }

  opentime = constrain(abs(target - pressure)*0.2, 0, 30);
  delay(opentime);
  
  digitalWrite(8, 0);
  digitalWrite(9, 0);

  if (opentime < 30) delay((stopwatch + 30) - millis());
}
