#include <Arduino.h>
#include <math.h>

#include<MsTimer2.h>

//sinカーブとかを描く用
unsigned long startmillis = 0;

#define OPEN 1
#define CLOSE 0

#define LONG 1
#define SHORT 0


//本来はここには供給圧力（タンク圧力）を入れる（今はセンシングしていないのでだめだが）
double p_max = 500.0;
double p_min = 0.0;


//時定数
double tau_p_long = 47.0;
double tau_n_long = 42.0;

double tau_p_short = 60.0;
double tau_n_short = 60.0;


//内圧が増えていくときの過渡応答グラフから，あるpのときの対応フェーズを返す
double phase_p(double p, bool isLongMuscle) {
  double temp = log((p_max - p_min)/(p_max - p));
  if(isLongMuscle) return tau_p_long * temp;
  return tau_p_short * temp;
}
//内圧が減っていくときの過渡応答グラフから，あるpのときの対応フェーズを返す
double phase_n(double p, bool isLongMuscle) {
  double temp = log((p_max - p_min)/(p - p_min));
  if(isLongMuscle) return tau_n_long * temp;
  return tau_n_short * temp;
}

int p_now;
int p_target;

void dataPrint(){
  Serial.print(p_target);
  Serial.print(",");
  Serial.println(p_now);
  
}

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

  MsTimer2::set(1, dataPrint);
  MsTimer2::start();
}

int openspan = 0;
double setPressure(int p_target) {

  //-------------------------------------------------------------------------//
  p_now = map(constrain(analogRead(A5),   513, 779),   513, 779, 0, 500);
  //Serial.print(p_target);
  //Serial.print(",");
  //Serial.println(p_now);
  
  if(p_now < p_target) {
    //吸気
    openspan = phase_p(p_target, LONG) - phase_p(p_now, LONG);
    if(openspan > 0) {
    //openspan *= 0.5;
    if(openspan > 30) openspan = 30;
    digitalWrite(9, HIGH);
    delay(openspan);
    digitalWrite(9, LOW);
    }
  } else {
    //排気
    openspan = phase_n(p_target, LONG) - phase_n(p_now, LONG);
    if(openspan > 0) {
    //openspan *= 0.5;
    if(openspan > 30) openspan = 30;
    digitalWrite(8, HIGH);
    delay(openspan);
    digitalWrite(8, LOW);
    }
  }
  delay(10);
}

unsigned long stopwatch = millis();

void loop() {

  //sinカーブ
  p_target = 100 + 100*sin((millis() * 2UL * PI)/2000);

  //ステップ
//  if (millis() - stopwatch > 1000) {
//    stopwatch = millis();
//    if (target == 400) {
//      target = 100;
//    } else {
//      target = 400;
//    }
//  }
  
  setPressure(p_target);
  //delay(5);
}
