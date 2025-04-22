//単に給排気をやってデータを記録するだけ

#include <Arduino.h>
#include <MsTimer2.h>

//1-27最新版

// --------------------------------------------------//
// 吸気圧力はコンプレッサーから指定する
// 500kPa給気のときは，減圧弁を530kPaくらいにしておくと良い（減圧弁の影響をなくすため）

// 初期圧力の設定（この圧力になったら，記録と弁動作が始まる）
#define P_START 0.0f
#define P_END   500.0f
// 初期圧にするためにどれくらい待つか
//250->500なら50msくらい
//100->500なら15msくらい
#define SHORT_DELAY 0

// --------------------------------------------------//

// 記録するミリ秒の設定
#define RECORD_TIME 550

// 使う人工筋の設定
#define PIN_P_SENSOR A0
#define PIN_INTAKE 2
#define PIN_EXHAUST 3

unsigned long stopWatch;
volatile int data[RECORD_TIME+50];
volatile int index = 0;

void pin_init();
float getPressure(int analogPin);

void push(int datam){
  if(index >= RECORD_TIME) {
      MsTimer2::stop();
      //データを全部吐き出す
      for(int i=0; i<index; i++) {
        Serial.println(data[i]);
      }
  }
  data[index++] = datam;
}

// 1msに一回これを回す
void sampling() {
  push((int)getPressure(PIN_P_SENSOR));
}

void setup() {
  pin_init();

  // 実験条件の出力
  Serial.print("simpleInEx ");
  Serial.print(P_START);
  Serial.print(" -> ");
  Serial.println(P_END);
  
  //所定の圧力にする
  digitalWrite(PIN_INTAKE, HIGH);
  delay(SHORT_DELAY);
  digitalWrite(PIN_INTAKE, LOW);
  while((getPressure(PIN_P_SENSOR) < P_START-1.0f) ||
        (getPressure(PIN_P_SENSOR) > P_START+1.0f)) {
          //Serial.println(getPressure(PIN_P_SENSOR));
        }
  
  MsTimer2::set(1, sampling);
  MsTimer2::start();

  delay(50);
  if (P_START < P_END) {
    digitalWrite(PIN_INTAKE, HIGH);
  } else {
    digitalWrite(PIN_EXHAUST, HIGH);
  }


  delay(2000);
  digitalWrite(2, LOW);
  digitalWrite(3, HIGH);
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH);
  digitalWrite(10, LOW);
  digitalWrite(11, HIGH);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
}

void loop() {}

void pin_init() {
  // ピンの初期化とシリアルの初期化と残圧の吐き出しをする
  
  pinMode (A0, INPUT); // Upper In
  pinMode (A1, INPUT); // Upper Out
  pinMode (A2, INPUT); // Lower In
  pinMode (A3, INPUT); // Lower Out
  
  pinMode (A4, INPUT); // theta1
  pinMode (A5, INPUT); // theta2

  pinMode (2, OUTPUT); // Upper In
  pinMode (3, OUTPUT);
  pinMode (8, OUTPUT); // Upper Out
  pinMode (9, OUTPUT);
  pinMode (10, OUTPUT); // Lower In
  pinMode (11, OUTPUT);
  pinMode (12, OUTPUT); // Lower Out
  pinMode (13, OUTPUT);

  Serial.begin(115200);

  //いちど全部排気してから，閉じる
  digitalWrite(2, LOW);
  digitalWrite(3, HIGH);
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH);
  digitalWrite(10, LOW);
  digitalWrite(11, HIGH);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  delay(1000);
}

float getPressure(int analogPin) {
  // アナログ値を圧力（単位はkPa）に変換する
  int analogValue = analogRead(analogPin);
  float v = (5.0f*(float)analogValue)/1024.0f;
  return max(0.0f, ((v*1034.12)/4.5) - 51.0f);
}
