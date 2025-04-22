// 単体のマッキベンの内圧にSINカーブを描かせて精度とかを確かめる実験
// 時間幅による制御が実現できたことを示す

//1－27最新版

#include <Arduino.h>
#include <MsTimer2.h>

// --------------------------------------------- //

// sinカーブの周期[ms]
#define SIN_T 4000.0f
// sinカーブの振幅[kPa]
#define SIN_A 150.0f
// sinカーブの中心圧力[kPa]
#define SIN_CENTRE 250.0f
// 1ステップごとの待ち時間[ms]
#define STEP_WAIT 400

// 使う人工筋の設定(Upper IN)
#define PIN_P_SENSOR A0
#define PIN_INTAKE 2
#define PIN_EXHAUST 3

#define A_0_EXH 0.95f
//#define A_0_EXH 1.30f  //供給圧100kPa用

#define A_0_INT 0.44f
//#define A_0_INT 0.70f;

//供給圧[kPa]
#define COMPRESSOR 500.0f


// --------------------------------------------- //

// 1ループで何s計算を進めるか
#define dt 0.001f

//環境設定
#define L_FIBRE 137.4f
#define N_TURNS 1.9833f
#define TEMPERATURE 293.0f
#define k 1.4f
#define R 287.0f
#define L_n 119.0f

#define PI 3.1415f

//マッキベンの近似式の係数
#define alpha 0.00079655f
#define beta -0.0004066f
#define gamma -15.2184f

float getPressure(int analogPin);

volatile float p_target = 0.0f;
volatile float p_current = 0.0f;


// 1msに一回これを回す
void sampling() {
  Serial.print(getPressure(PIN_P_SENSOR));
  Serial.print(",");
  Serial.println(p_target);
//  Serial.print(",");
  //Serial.println(PORTD);

  //sinカーブから目標圧を設定
  p_target = SIN_A * sin((millis() / SIN_T) * 2.0f * PI) + SIN_CENTRE;
}

void pin_init();

unsigned long startTime;
void setup() {
  pin_init();

  MsTimer2::set(5, sampling);
  MsTimer2::start();

  startTime = millis();
}

// マッキベンの全長から容積を推定する
float calcVolume(float mckibben_length) {
  if (mckibben_length > L_FIBRE) mckibben_length = L_FIBRE;
  else if (mckibben_length < 85.0f) mckibben_length = 85.0f;

  float d_muscle = (L_FIBRE * sin(acos(mckibben_length / L_FIBRE))) / (N_TURNS * PI);
  float d_inner = d_muscle - 2.8f;
  float volume = d_inner * d_inner * PI * mckibben_length / 4;
  return volume;
}

// 無負荷時の理論上の体積を圧力から求める
const float lca = -3.5185f * pow(10.0, -7);
const float lcb = 4.4603f * pow(10.0, -4);
const float lcc = -0.1973f;
const float lcd = 119.1270f;
float calcVolumeFree(float pressure) {
  float L = lca * pow(pressure, 3) + lcb * pow(pressure, 2) + lcc * pressure + lcd;
  return calcVolume(L);
}

// 上流圧力と下流圧力から質量流量を計算する
float calcDeltaM(float p_in, float p_out, bool isExhaust) {

  float A_0; //実験を元に決める
  if (isExhaust)
  {
    A_0 = A_0_EXH;
  }
  else
  {
    A_0 = A_0_INT;
  }

  float CA = (A_0 * p_in) / sqrt(TEMPERATURE);
  if (p_in * pow(2.0f / (k + 1.0f), k / (k - 1.0f)) >= p_out) {
    float CB = sqrt((k / R) * pow(2.0f / (k + 1.0f), (k + 1.0f) / (k - 1.0f)));
    return CA * CB;
  }
  float CC = sqrt((2.0f * k) / (R * (k - 1.0f)));
  float CD = pow(p_out / p_in, 1.0f / k);
  float CE = sqrt(1.0f - pow(p_out / p_in, (k - 1.0f) / k));
  return CA * CC * CD * CE;
}

float calcDeltaP(float delta_m, float v, float dv, float p) {
  float k1 = 1.4f;
  float k2 = 1.4f;
  return ((k1 * R * TEMPERATURE * delta_m * 1000.0f) / v - (k2 * dv * p) / v);
}


int calcTimeWidth(float p_current, float p_target, float v_current, float pipe_volume) {
  float p = p_current;
  float v = v_current + pipe_volume;
  float dv = 0.0f;
  int roop_count = 0;
  float dm;
  float dp;
  if (p_current < p_target) {
    while ((p <= p_target) && (roop_count < 300)) {
      dm = calcDeltaM(COMPRESSOR, p, false);
      dp = calcDeltaP(dm, v, dv, p);
      p += (dp * dt);
      roop_count++;

      float v_new = calcVolumeFree(p) + pipe_volume;
      dv = (v_new - v) / dt;
      v = v_new;
    }
  } else {
    while ((p >= p_target) && (roop_count > -300)) {
      dm = -1 * calcDeltaM(p, 0.0f, true);
      dp = calcDeltaP(dm, v, dv, p);
      p += (dp * dt);
      roop_count--;

      float v_new = calcVolumeFree(p) + pipe_volume;
      dv = (v_new - v) / dt;
      v = v_new;
    }
  }
  return roop_count;
}

unsigned long stopwatch;

//桁落ちするので，体積はmm^3で扱う
#define PIPE_U (4.9087f * 1300.0f)
#define PIPE_L (4.9087f * 1900.0f)

float v_current;
int time_width;
void loop() {
  //calcTimeWidth(0.0f, 500.0f, calcVolumeFree(0.0f), PIPE_U);
  //calcTimeWidth(500.0f, 0.0f, calcVolumeFree(500.0f), PIPE_U);
  //  while(true){
  //    Serial.println(SIN_A*sin((millis()/SIN_T)*2.0f*PI) + SIN_CENTRE);
  //  };
  
  stopwatch = millis();
  
  //現在圧を測定
  p_current = getPressure(PIN_P_SENSOR);

  //現在圧から無負荷時の理論上の容積を推定
  v_current = calcVolumeFree(p_current);

  time_width = calcTimeWidth(p_current, p_target, v_current, PIPE_U);


  if (time_width > 0) {
    if(time_width > STEP_WAIT) time_width = STEP_WAIT;
    digitalWrite(PIN_INTAKE, HIGH);
    delay(time_width);
    digitalWrite(PIN_INTAKE, LOW);
  } else {
    if(time_width < -STEP_WAIT) time_width = -STEP_WAIT;
    digitalWrite(PIN_EXHAUST, HIGH);
    delay(-time_width);
    digitalWrite(PIN_EXHAUST, LOW);
  }

//  Serial.print(p_target);
//  Serial.print(">");
//  Serial.print(p_current);
//  Serial.print(">");
//  Serial.println(time_width);

  if (millis() - startTime > 16000) {
    MsTimer2::stop();
    while (true);
  }

   //1ループの周期がすぎるのを待つ
  while (millis() - STEP_WAIT < stopwatch) {}
}

void pin_init() {
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
  // アナログ値を圧力に変換する（単位はkPa）
  int analogValue = analogRead(analogPin);
  float v = (5.0f * analogValue) / 1024.0f;
  return max(0.0f, ((v * 1034.120f) / 4.5f) - 51.0f);
}
