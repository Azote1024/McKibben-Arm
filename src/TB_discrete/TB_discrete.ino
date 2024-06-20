#include <Arduino.h>
#include <math.h>

void setup() {
  //このプログラムは基本的にUNO向けなので注意
  DDRD = DDRD | B11111100;
  DDRB = DDRB | B00000011;

  //人工筋内圧
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  //リンク内角
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  
  Serial.begin(115200);
  delay(1000);
}

unsigned long stopwatch = millis();
int p_target = 0;
int p_now = 0;
int error = 0;
int openspan = 0;
void loop() {

  //sinカーブ
  p_target = 400 + 100*sin((millis() * 2UL * PI)/500);

  //現在の圧力
  p_now = map(constrain(analogRead(A5),   513, 779),   513, 779, 0, 500);

  error = p_target - p_now;

  // ここの定数で動きが変わる
  if (abs(error) < 20) openspan = 0;
  else if (abs(error) > 50) openspan = 12;
  else openspan = 6;

  //目標のほうが大きいので，吸気する
  if (error > 0) {
    digitalWrite(9, HIGH);
    delay(openspan);
    digitalWrite(9, LOW);

  //目標のほうが小さいので，排気する
  } else if (error < 0) {
    digitalWrite(8, HIGH);
    delay(openspan);
    digitalWrite(8, LOW);
    
  } else {}

  Serial.print(p_target);
  Serial.print(",");
  Serial.println(p_now);

  //後ろのdelayも影響する
  delay(30);
}
