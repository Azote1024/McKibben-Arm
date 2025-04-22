// 関節単体の目標剛性と目標中立位置を達成できているか確かめる実験

#include <Arduino.h>
#include <MsTimer2.h>

// --------------------------------------------- //

// 関節の目標剛性値[N/mm]
#define JOINT_K 7020.0f
// 関節の目標中立位置
#define JOINT_THETA 0.0f

// 圧力が下がっていくはずなので一定時間ごとに空気をいれて維持する
#define STEP_WAIT 1000

// --------------------------------------------- //

#define A_0_EXH 0.95f
//#define A_0_EXH 1.30f  //供給圧100kPa用

#define A_0_INT 0.44f
//#define A_0_INT 0.70f;

//供給圧[kPa]
#define COMPRESSOR 500.0f

// 1ループで何s計算を進めるか
#define dt 0.001f

// 使う人工筋の設定(UI)
#define PIN_P_SENSOR_IN A2
#define PIN_INTAKE_IN 10
#define PIN_EXHAUST_IN 11

// 使う人工筋の設定(UO)
#define PIN_P_SENSOR_OUT A3
#define PIN_INTAKE_OUT 12
#define PIN_EXHAUST_OUT 13

//環境設定
#define L_FIBRE 137.4f
#define N_TURNS 1.9833f
#define TEMPERATURE 293.0f
#define k 1.4f
#define R 287.0f
#define L_n 119.0f
#define d_loose 15.0f
#define D_pulley 14.0f

#define PI 3.1415f

//マッキベンの近似式
#define alpha 0.00079655f
#define beta -0.0004066f
#define gamma -15.2184f

float getPressure(int analogPin);
float getDegree(int analogPin, float offset);

void pin_init();

// 10msに一回これを回す
void sampling() {
//  Serial.print(" theta1:");
//  Serial.print(getDegree(A4, 422));
//  Serial.print(" theta2:");
//  Serial.print(getDegree(A5, 458));
//  Serial.println("");
}

void setup() {
  pin_init();

//  MsTimer2::set(10, sampling);
//  MsTimer2::start();

}

// ここちゃんとポインタ使ってまとめる！
float p_UI = 0;
float p_UO = 0;
void posRig2Prs_U(float theta, float K) {
  float x_target = theta*D_pulley/2.0;
  float CA = -1.0f*(((2.0f*alpha*(d_loose + x_target))/(L_n*L_n)) - ((2.0f*alpha)/L_n));
  float CB = -1.0f*(((2.0f*alpha*(d_loose - x_target))/(L_n*L_n)) - ((2.0f*alpha)/L_n));
  float CC = alpha*pow(1.0f-((d_loose + x_target)/L_n), 2.0f) + beta;
  float CD = alpha*pow(1.0f-((d_loose - x_target)/L_n), 2.0f) + beta;

  p_UI = ((CD*K) / (CA*CD+CB*CC))/1000000.0f;
  p_UO = ((CC*K) / (CA*CD+CB*CC))/1000000.0f;
}

// マッキベンの全長から容積を推定する
float calcVolume(float mckibben_length) {
  if (mckibben_length > L_FIBRE) mckibben_length = L_FIBRE;
  else if (mckibben_length < 85.0f) mckibben_length = 85.0f;

  float d_muscle = (L_FIBRE * sin(acos(mckibben_length / L_FIBRE))) / (N_TURNS * PI);
  float d_inner = d_muscle - 2.8f;
  float volume = d_inner * d_inner * PI * mckibben_length / 4;
  return volume;
  d_inner*d_inner*PI*mckibben_length/4;
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

float p_target_in;
float p_target_out;

float p_current_in;
float v_current_in;
int time_width_in;

float p_current_out;
float v_current_out;
int time_width_out;

int maxActionTime;

//桁落ちするので，体積はmm^3で扱う
#define PIPE_U (4.9087f * 1300.0f)
#define PIPE_L (4.9087f * 1900.0f)

void loop() {

  // 関節剛性と中立位置から，圧力ふたつを求める
  posRig2Prs_U(radians(JOINT_THETA), JOINT_K);
  p_target_in = p_UI;
  p_target_out = p_UO;

//  p_target_in = 150.0;
//  p_target_out = 150.0;


  // in側時間幅
  p_current_in = getPressure(PIN_P_SENSOR_IN);
  v_current_in = calcVolumeFree(p_current_in);
  time_width_in = calcTimeWidth(p_current_in, p_target_in, v_current_in, PIPE_L);

  // out側時間幅
  p_current_out = getPressure(PIN_P_SENSOR_OUT);
  v_current_out = calcVolumeFree(p_current_out);
  time_width_out = calcTimeWidth(p_current_out, p_target_out, v_current_out, PIPE_L);

  maxActionTime = max(abs(time_width_in), abs(time_width_out));

  Serial.print("pi:");
  Serial.print(p_target_in);
  Serial.print(" po:");
  Serial.print(p_target_out);
  Serial.print(" theta:");
  Serial.println(getDegree(A4, 422));
  //Serial.println(getDegree(A5, 458.0f));

  // 1ループの周期がすぎるのを待つ
  while (millis() - STEP_WAIT < stopwatch) {}
  stopwatch = millis();
  
  // 同時に開閉を行う
  while(stopwatch + maxActionTime + 10 > millis()){
    if(time_width_in > 0) {
      if(stopwatch + time_width_in > millis()) digitalWrite(PIN_INTAKE_IN, HIGH);
      else digitalWrite(PIN_INTAKE_IN, LOW);
    }
    else if (time_width_in < 0){
      if(stopwatch - time_width_in > millis()) digitalWrite(PIN_EXHAUST_IN, HIGH);
      else digitalWrite(PIN_EXHAUST_IN, LOW);
    }


    if(time_width_out > 0) {
      if(stopwatch + time_width_out > millis()) digitalWrite(PIN_INTAKE_OUT, HIGH);
      else digitalWrite(PIN_INTAKE_OUT, LOW);
    }
    else if (time_width_out < 0){
      if(stopwatch - time_width_out > millis()) digitalWrite(PIN_EXHAUST_OUT, HIGH);
      else digitalWrite(PIN_EXHAUST_OUT, LOW);
    }

  }
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

float getDegree(int analogPin, float offset) {
  return (analogRead(analogPin)-offset)*0.32727f;
}
