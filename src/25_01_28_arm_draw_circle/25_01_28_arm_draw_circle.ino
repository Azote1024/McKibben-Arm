// アームに円を描かせる実験

#include <Arduino.h>
#include <MsTimer2.h>

// 1月29日最新版

// --------------------------------------------- //

// 関節の目標剛性値[N/mm]
#define JOINT_K 7020.0f

// 目標円軌道の半径
#define CIRCLE_PHI 150.0f

// 目標円軌道の周期[ms]
#define CIRCLE_T 5000.0f

// 目標円軌道の分割数
#define CIRCLE_N 10.0f

// 周期/分割数が50msを切らないように！

// 開閉タイミングタイプ
//#define TYPE_OC 0

// 体積近似タイプ
// 0 目標位置と現在位置から体積を推定して線形補間（こっちがベース）
// 1 現在圧力のまま不変とする
#define TYPE_DELTAV 0

// --------------------------------------------- //

// 目標円軌道の中心位置
#define X_CENTRE 240.0f
#define Y_CENTRE 0.0f

#define A_0_EXH 0.95f
//#define A_0_EXH 1.30f  //供給圧100kPa用

#define A_0_INT 0.44f
//#define A_0_INT 0.70f;

//供給圧[kPa]
#define COMPRESSOR 500.0f

// 1ループで何s計算を進めるか
#define dt 0.001f

// リンク長
#define L1 240.0f
#define L2 240.0f


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

unsigned long startTime;

float theta1_target = 0;
float theta2_target = 0;

float p_target_ui = 0;
float p_target_uo = 0;
float p_target_li = 0;
float p_target_lo = 0;

unsigned long printTime;
// 10msに一回これを回す
void sampling() {
  printTime = micros();
  Serial.print((int)p_target_ui);
  Serial.print(",");
  Serial.print((int)getPressure(A0)); //UI
  Serial.print(",");
  Serial.print((int)p_target_uo);
  Serial.print(",");
  Serial.print((int)getPressure(A1)); //UO
  Serial.print(",");
  Serial.print((int)p_target_li);
  Serial.print(",");
  Serial.print((int)getPressure(A2)); //LI
  Serial.print(",");
  Serial.print((int)p_target_lo);
  Serial.print(",");
  Serial.print((int)getPressure(A3)); //LO
  Serial.print(",");
  Serial.print((int)degrees(theta1_target));
  Serial.print(",");
  Serial.print((int)getDegree(A5, 458)); //根元側
  Serial.print(",");
  Serial.print((int)degrees(theta2_target));
  Serial.print(",");
  Serial.print((int)getDegree(A4, 450)); //手先側
  Serial.println("");
}

void setup() {
  pin_init();

  // 流石に5msに一回とかは返せない
  MsTimer2::set(50, sampling);
  MsTimer2::start();

  startTime = millis();
}

// ここちゃんとポインタ使ってまとめる！
void posRig2Prs_U(float theta, float K) {
  float x_target = theta*D_pulley/2.0;
  float CA = -1.0f*(((2.0f*alpha*(d_loose + x_target))/(L_n*L_n)) - ((2.0f*alpha)/L_n));
  float CB = -1.0f*(((2.0f*alpha*(d_loose - x_target))/(L_n*L_n)) - ((2.0f*alpha)/L_n));
  float CC = alpha*pow(1.0f-((d_loose + x_target)/L_n), 2.0f) + beta;
  float CD = alpha*pow(1.0f-((d_loose - x_target)/L_n), 2.0f) + beta;
  p_target_ui = ((CD*K) / (CA*CD+CB*CC))/1000000.0f;
  p_target_uo = ((CC*K) / (CA*CD+CB*CC))/1000000.0f;
}

void posRig2Prs_L(float theta, float K) {
  float x_target = theta*D_pulley/2.0;
  float CA = -1.0f*(((2.0f*alpha*(d_loose + x_target))/(L_n*L_n)) - ((2.0f*alpha)/L_n));
  float CB = -1.0f*(((2.0f*alpha*(d_loose - x_target))/(L_n*L_n)) - ((2.0f*alpha)/L_n));
  float CC = alpha*pow(1.0f-((d_loose + x_target)/L_n), 2.0f) + beta;
  float CD = alpha*pow(1.0f-((d_loose - x_target)/L_n), 2.0f) + beta;
  p_target_li = ((CD*K) / (CA*CD+CB*CC))/1000000.0f;
  p_target_lo = ((CC*K) / (CA*CD+CB*CC))/1000000.0f;
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

int calcTimeWidth(float p_current, float p_target, float v_current, float v_delta, float pipe_volume) {
  float p = p_current;
  float v = v_current + pipe_volume;
  float dv = v_delta;
  int roop_count = 0;
  float dm;
  float dp;
  float v_new;
//  Serial.println("---");
  if (p_current < p_target) {
    while ((p <= p_target) && (roop_count < CIRCLE_T/CIRCLE_N)) {
      dm = calcDeltaM(COMPRESSOR, p, false);
      dp = calcDeltaP(dm, v, dv, p);
      p += (dp * dt);
      roop_count++;
      v += (dv * dt);
//      Serial.print(p);
//      Serial.print(" ");
//      Serial.println(v-pipe_volume);
    }
  } else {
    while ((p >= p_target) && (roop_count > -CIRCLE_T/CIRCLE_N)) {
      dm = -1.0f * calcDeltaM(p, 0.0f, true);
      dp = calcDeltaP(dm, v, dv, p);
      p += (dp * dt);
      roop_count--;
      v += (dv * dt);
//      Serial.print(p);
//      Serial.print(" ");
//      Serial.println(v-pipe_volume);
    }
  }
  return roop_count;
}

unsigned long stopwatch;

float delta_x_1;
float delta_x_2;

float x_target;
float y_target;

float p_current_ui;
float p_current_uo;
float p_current_li;
float p_current_lo;

float v_current_ui;
float v_current_uo;
float v_current_li;
float v_current_lo;

float v_target_ui;
float v_target_uo;
float v_target_li;
float v_target_lo;

float time_width_ui;
float time_width_uo;
float time_width_li;
float time_width_lo;

int maxActionTime;

unsigned long roopTime;

//桁落ちするので，体積はmm^3で扱う
#define PIPE_U (4.9087f * 1300.0f)
#define PIPE_L (4.9087f * 1900.0f)

void loop() {

  // 各筋の現在の長さを得る
  delta_x_1 = (theta1_target+radians(60.0))*D_pulley/2;
  delta_x_2 = (theta2_target-radians(120.0))*D_pulley/2;
  v_current_ui = calcVolume(L_n-d_loose-delta_x_1);
  v_current_uo = calcVolume(L_n-d_loose+delta_x_1);
  v_current_li = calcVolume(L_n-d_loose-delta_x_2);
  v_current_lo = calcVolume(L_n-d_loose+delta_x_2);
  
  x_target = CIRCLE_PHI * cos(((millis()-startTime)/CIRCLE_T)*2.0f*PI) + X_CENTRE;
  y_target = CIRCLE_PHI * sin(((millis()-startTime)/CIRCLE_T)*2.0f*PI) + Y_CENTRE;

  float D = (pow(x_target,2.0f) + pow(y_target,2.0f) - pow(L1,2.0f) - pow(L2,2.0f)) / (2.0f*L1*L2);
  theta2_target = atan2(sqrt(1.0f - pow(D,2.0f)), D);
  theta1_target = atan2(y_target, x_target) - atan2(L2 * sin(theta2_target), L1 + L2 * cos(theta2_target));


  switch (TYPE_DELTAV) {
    case 0:
      // 各筋の目標の長さを得る
      delta_x_1 = (theta1_target+radians(60.0))*D_pulley/2;
      delta_x_2 = (theta2_target-radians(120.0))*D_pulley/2;
      v_target_ui = calcVolume(L_n-d_loose-delta_x_1);
      v_target_uo = calcVolume(L_n-d_loose+delta_x_1);
      v_target_li = calcVolume(L_n-d_loose-delta_x_2);
      v_target_lo = calcVolume(L_n-d_loose+delta_x_2);
      break;
    case 1:
      v_target_ui = v_current_ui;
      v_target_uo = v_current_uo;
      v_target_li = v_current_li;
      v_target_lo = v_current_lo;
      break;
  }

// 理想動作用
//  p_current_ui = p_target_ui;
//  p_current_uo = p_target_uo;

  posRig2Prs_U(theta1_target + radians(60.0f), JOINT_K);
  // UI
  p_current_ui = getPressure(A0);
  time_width_ui = calcTimeWidth(p_current_ui, p_target_ui, v_current_ui,
    (v_target_ui-v_current_ui)/((CIRCLE_T/CIRCLE_N)*0.001), PIPE_U);

  // UO
  p_current_uo = getPressure(A1);
  time_width_uo = calcTimeWidth(p_current_uo, p_target_uo, v_current_uo,
    (v_target_uo-v_current_uo)/((CIRCLE_T/CIRCLE_N)*0.001), PIPE_U);

// 理想動作用
//  p_current_li = p_target_li;
//  p_current_lo = p_target_lo;

  posRig2Prs_L(theta2_target - radians(120.0f), JOINT_K);
  // LI
  p_current_li = getPressure(A2);
  time_width_li = calcTimeWidth(p_current_li, p_target_li, v_current_li,
    (v_target_li-v_current_li)/((CIRCLE_T/CIRCLE_N)*0.001), PIPE_L);

  // LO
  p_current_lo = getPressure(A3);
  time_width_lo = calcTimeWidth(p_current_lo, p_target_lo, v_current_lo,
    (v_target_lo-v_current_lo)/((CIRCLE_T/CIRCLE_N)*0.001), PIPE_L);
  
//  Serial.print("theta1:");
//  Serial.print(degrees(theta1_target + radians(60.0f)));
//  Serial.print(" theta2:");
//  Serial.println(degrees(theta2_target - radians(120.0f)));

  maxActionTime = max(abs(time_width_ui), max(abs(time_width_uo), max(abs(time_width_li), abs(time_width_lo))));

//  Serial.print("t");
//  Serial.print(millis());
//  Serial.print(" ui:");
//  Serial.print(time_width_ui);
//  Serial.print(" uo:");
//  Serial.print(time_width_uo);
//  Serial.print(" li:");
//  Serial.print(time_width_li);
//  Serial.print(" lo:");
//  Serial.println(time_width_lo);

  stopwatch = millis();

  // 同時に開閉を行う
  while(millis() - roopTime < (CIRCLE_T/CIRCLE_N)){
    // UI
    if(time_width_ui > 0) {
      if(stopwatch + time_width_ui > millis()) digitalWrite(2, HIGH);
      else digitalWrite(2, LOW);
    }
    else if (time_width_ui < 0){
      if(stopwatch - time_width_ui > millis()) digitalWrite(3, HIGH);
      else digitalWrite(3, LOW);
    }
    // UO
    if(time_width_uo > 0) {
      if(stopwatch + time_width_uo > millis()) digitalWrite(8, HIGH);
      else digitalWrite(8, LOW);
    }
    else if (time_width_uo < 0){
      if(stopwatch - time_width_uo > millis()) digitalWrite(9, HIGH);
      else digitalWrite(9, LOW);
    }
    // LI
    if(time_width_li > 0) {
      if(stopwatch + time_width_li > millis()) digitalWrite(10, HIGH);
      else digitalWrite(10, LOW);
    }
    else if (time_width_li < 0){
      if(stopwatch - time_width_li > millis()) digitalWrite(11, HIGH);
      else digitalWrite(11, LOW);
    }
    // LO
    if(time_width_lo > 0) {
      if(stopwatch + time_width_lo > millis()) digitalWrite(12, HIGH);
      else digitalWrite(12, LOW);
    }
    else if (time_width_lo < 0){
      if(stopwatch - time_width_lo > millis()) digitalWrite(13, HIGH);
      else digitalWrite(13, LOW);
    }
  }

  roopTime = millis();
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
