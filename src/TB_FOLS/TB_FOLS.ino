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
double tau_p_long = 46.0;
double tau_n_long = 41.0;

double tau_p_short = 60.0;
double tau_n_short = 60.0;


//内圧が増えていくときの過渡応答グラフから，あるpのときの対応フェーズを返す
double phase_p(double p, bool isLongMuscle) {
  if(p_max == p) return 100;
  double temp = log((p_max - p_min)/(p_max - p));
  if(isLongMuscle) return tau_p_long * temp;
  return tau_p_short * temp;
}
//内圧が減っていくときの過渡応答グラフから，あるpのときの対応フェーズを返す
double phase_n(double p, bool isLongMuscle) {
  if(p_min == p) return 100;
  double temp = log((p_max - p_min)/(p - p_min));
  if(isLongMuscle) return tau_n_long * temp;
  return tau_n_short * temp;
}

volatile int p_now = 0;
volatile int p_target = 500;

unsigned long stopwatch = 0;

void dataPrint(){
  //三角波
//  p_target = millis()*0.1 - stopwatch;

  //sinカーブ
 p_target = (int)(250.0 + 250.0*sin(((float)millis() * 2.0 * 3.1415)/200.0));
 //p_target = 0;

//  if(p_target > 500)stopwatch = millis()*0.1;

  //ステップ
//  if (millis() - stopwatch > 1000) {
//    stopwatch = millis();
//    if (p_target == 500) {
//      p_target = 0;
//    } else {
//      p_target = 500;
//    }
//  }
  
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

double setPressure() {

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
    if(openspan > 25) openspan = 25;
    digitalWrite(9, HIGH);
    delay(openspan);
    digitalWrite(9, LOW);
    }
  } else {
    //排気
    openspan = phase_n(p_target, LONG) - phase_n(p_now, LONG);
    if(openspan > 0) {
    //openspan *= 0.5;
    if(openspan > 25) openspan = 25;
    digitalWrite(8, HIGH);
    delay(openspan);
    digitalWrite(8, LOW);
    }
  }
}

double KI = 0.1;
double setPressure_P() {

  //-------------------------------------------------------------------------//
  p_now = map(constrain(analogRead(A5),   513, 779),   513, 779, 0, 500);
  //Serial.print(p_target);
  //Serial.print(",");
  //Serial.println(p_now);
  
  if(p_now < p_target) {
    //吸気
    openspan = (p_target - p_now) * KI;
    if(openspan > 0) {
    //openspan *= 0.5;
    if(openspan > 25) openspan = 25;
    digitalWrite(9, HIGH);
    delay(openspan);
    digitalWrite(9, LOW);
    }
  } else {
    //排気
    openspan = (p_now - p_target) * KI;
    if(openspan > 0) {
    //openspan *= 0.5;
    if(openspan > 25) openspan = 25;
    digitalWrite(8, HIGH);
    delay(openspan);
    digitalWrite(8, LOW);
    }
  }
}

double setPressure_H() {
  //-------------------------------------------------------------------------//
  p_now = map(constrain(analogRead(A5),   513, 779),   513, 779, 0, 500);
  //Serial.print(p_target);
  //Serial.print(",");
  //Serial.println(p_now);
  
  if(p_now < p_target) {
    //吸気
    if ((p_target - p_now) > 10) openspan = 10;
    else if ((p_target - p_now) > 50) openspan = 20;
    else openspan = 0;
    if(openspan > 0) {
    //openspan *= 0.5;
    if(openspan > 25) openspan = 25;
    digitalWrite(9, HIGH);
    delay(openspan);
    digitalWrite(9, LOW);
    }
  } else {
    //排気
    if ((p_now - p_target) > 10) openspan = 10;
    else if ((p_now - p_target) > 50) openspan = 20;
    else openspan = 0;
    if(openspan > 0) {
    //openspan *= 0.5;
    if(openspan > 25) openspan = 25;
    digitalWrite(8, HIGH);
    delay(openspan);
    digitalWrite(8, LOW);
    }
  }
}


void loop() {
  setPressure();
  delay(10);
}
