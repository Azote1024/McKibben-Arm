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


unsigned long stopwatch = 0;
void loop() {
  stopwatch = millis();
  
  pressure = map(constrain(analogRead(A5),   513, 779),   513, 779, 0, 500);

  target = 250 + 250*sin((millis() * 2UL * PI)/2000);

  //三角波
  target = millis()*0.1 - stopwatch;
  if(target > 500)stopwatch = millis()*0.1;


  // ------------------------------------------------------ //

  //誤差を定数倍して開時間を決定する
  opentime = constrain(abs(target - pressure)*0.2, 0, 30);

  //段階的にduty比決定
//  if((target - pressure) > 40) opentime = 15;
//  else if ((target - pressure) > 100) opentime = 30;

  //一定以上の誤差があったら開くだけ
//  if ((target - pressure) > 100) opentime = 30;

  // ------------------------------------------------------ //
  
  if (pressure > target) {
    digitalWrite(8, 1);
    digitalWrite(9, 0);
  } 
  else if (pressure < target){
    digitalWrite(8, 0);
    digitalWrite(9, 1);
  }

  delay(opentime);
  
  digitalWrite(8, 0);
  digitalWrite(9, 0);

  Serial.println(pressure);
  if (opentime < 30) delay((stopwatch + 30) - millis());
}
