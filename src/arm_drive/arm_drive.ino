#include <Arduino.h>
#include <math.h>

#define OPEN 1
#define CLOSE 0

#define INNER_ARM_LENGTH 230
#define OUTER_ARM_LENGTH 225

double p_max = 410.0;
double p_min = 50.0;
double p_target = 50.0;
double p_now = 50.0;

double l_target = 0;
double l_now = 0;

double tau_p_long = 60.0;
double tau_n_long = 60.0;

double tau_p_short = 60.0;
double tau_n_short = 60.0;

double FK_x (double theta1, double theta2) {
  return INNER_ARM_LENGTH*cos(theta1) + OUTER_ARM_LENGTH*cos(theta1 + theta2);
}

double FK_y (double theta1, double theta2) {
  return INNER_ARM_LENGTH*sin(theta1) + OUTER_ARM_LENGTH*sin(theta1 + theta2);
}

double IK_theta1(double x, double y) {

  double d1 = pow(x, 2) + pow(y, 2) + pow(INNER_ARM_LENGTH, 2) - pow(OUTER_ARM_LENGTH, 2);
  double d2 = 2 * INNER_ARM_LENGTH * sqrt(pow(x, 2) + pow(y, 2));
  return -acos(d1 / d2) + atan2(y, x);  // atan2からacosに変更して、角度θ2を正しく計算
}


double IK_theta2(double x, double y) {

  double theta1 = IK_theta1(x, y);

  double d1 = y - INNER_ARM_LENGTH*sin(theta1);
  double d2 = x - INNER_ARM_LENGTH*cos(theta1);

  return atan2(d1, d2) - theta1;
}

double length_LI (double theta2) {
  double L1 = 175.0;
  double L2 = 50.0;
  double d = 15.0;
  double l_2 = pow(L1*cos(theta2) - d*sin(theta2) + L2, 2);
        l_2 += pow(L1*sin(theta2) + d*cos(theta2) - d, 2);
  return constrain(225 - sqrt(l_2), 0, 35);
}

double length_LO (double theta2) {
  double L = 95.0;
  double d = 15.0;
  double r60 = radians(60);
  double l_2  = pow(d*cos(theta2 + r60) - L, 2);
         l_2 += pow(d*sin(theta2 + r60) - d, 2);
  return constrain(25 - (sqrt(l_2)- 95 + 7.5), 0, 25);
}

double length_UI (double theta1) {
  double L1 = 155.0;
  double L2 = 45.0;
  double d = 15.0;
  double l_2  = pow(L1*cos(theta1) - d*sin(theta1) + L2, 2);
         l_2 += pow(L1*sin(theta1) + d*cos(theta1) - d, 2);
  return constrain(210 - sqrt(l_2), 0, 35);
}

double length_UO (double theta1) {
  double L1 = 20.0;
  double L2 = 20.0;
  double d = 15.0;
  double l_2  = pow(L2*cos(theta1) - d*sin(theta1) + L1, 2);
         l_2 += pow(L2*sin(theta1) - d*cos(theta1) + d, 2);
  return constrain(sqrt(l_2) - 15, 0, 25);
}

# define LONG  1
# define SHORT 0
//圧力から無負荷時の長さを推定する．今回は簡単のために多項式近似（あまり高次になると桁落ちの影響が出やすいため3次）
double pressure2length (double p, bool isLongMuscle) {
  double l = 0;
  if (isLongMuscle) l = 1.598922190*pow(p/100, 3.0) - 16.6702994*pow(p/100, 2.0) + 0.608702503*p  - 44.1744767;
  else l = 0.789519805*pow(p/100, 3.0) - 8.81438244*pow(p/100, 2.0) + 0.358093604*p  - 26.1634126;
  return constrain(l, 0, 40);
}
//長さから無負荷時の圧力を推定する．今回は簡単のために多項式近似（あまり高次になると桁落ちの影響が出やすいため3次）
double length2pressure (double l, bool isLongMuscle) {
  double p = 0;
  if (isLongMuscle) p = 20.8631736*pow(l/10, 3.0) - 86.0164498*pow(l/10, 2.0) + 14.2270649*l  + 60.7685131;
  else p = 29.3302257*pow(l/10, 3.0) - 77.8265821*pow(l/10, 2.0) + 11.9162234*l  + 79.1244845;
  return constrain(p, 50, 450);
}

//内圧が増えていくときのt
double phase_p(double p, bool isLongMuscle) {
  double temp = log((p_max - p_min)/(p_max - p));
  if(isLongMuscle) return tau_p_long * temp;
  return tau_p_short * temp;
}
//内圧が減っていくときのt
double phase_n(double p, bool isLongMuscle) {
  double temp = log((p_max - p_min)/(p - p_min));
  if(isLongMuscle) return tau_n_long * temp;
  return tau_n_short * temp;
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
}


void swap (int& x, int& y) {
  x ^= y;
  y ^= x;
  x ^= y;
}
void *sort (int t[4][2]) {
  //n = 4固定なので謎ソートでいいや
  if (t[0][0] > t[1][0]) {
    swap (t[0][0], t[1][0]);
    swap (t[0][1], t[1][1]);
  }
  if (t[2][0] > t[3][0]) {
    swap (t[2][0], t[3][0]);
    swap (t[2][1], t[3][1]);
  }
  if (t[0][0] > t[2][0]) {
    swap (t[0][0], t[2][0]);
    swap (t[0][1], t[2][1]);
  }
  if (t[1][0] > t[3][0]) {
    swap (t[1][0], t[3][0]);
    swap (t[1][1], t[3][1]);
  }
  if (t[1][0] > t[2][0]) {
    swap (t[1][0], t[2][0]);
    swap (t[1][1], t[2][1]);
  }
}

//(x, y)に向けてハンドを動かす関数
//実行時間は最も長い吸気/排気命令の長さに依存する
double movehand(int x, int y) {


  //(x, y)から目標アーム角(theta1, theta2)を算出
  double theta1 = IK_theta1(x, y);
  double theta2 = IK_theta2(x, y);

//  Serial.println(degrees(theta1));
//  Serial.println(degrees(theta2));

  //(theta1, theta2)から目標筋長L_UI, L_UO, L_LI, L_LOを算出
  double L_UI = length_UI(theta1);
  double L_UO = length_UO(theta1);
  double L_LI = length_LI(theta2);
  double L_LO = length_LO(theta2);
  
  //目標筋長L_UI, L_UO, L_LI, L_LOから目標圧力P_UI, P_UO, P_LI, P_LOを算出
  int P_UI = length2pressure(L_UI, LONG);
  int P_UO = length2pressure(L_UO, SHORT);
  int P_LI = length2pressure(L_LI, LONG);
  int P_LO = length2pressure(L_LO, SHORT);

  //目標圧力P_UI, P_UO, P_LI, P_LOから開閉時間を算出
  //同時に電磁弁のidも設定
  int P_UI_now = map(constrain(analogRead(A2), 127, 376), 127, 376, 50, 400);
  int P_UO_now = map(constrain(analogRead(A3), 501, 776), 501, 776, 50, 400);
  int P_LI_now = map(constrain(analogRead(A4),   0, 234),   0, 234, 50, 400);
  int P_LO_now = map(constrain(analogRead(A5), 512, 786), 512, 786, 50, 400);
  
  int t[4][2] = {{0, B00000100}, {0, B00010000}, {0, B01000000}, {0, B00000001}};
  if(P_UI_now < P_UI) {
    t[0][0] = phase_p(P_UI, LONG) - phase_p(P_UI_now, LONG);
  } else {
    t[0][0] = phase_n(P_UI, LONG) - phase_n(P_UI_now, LONG);
    t[0][1] = B00001000;
  }
  if(P_UO_now < P_UO) {
    t[1][0] = phase_p(P_UO, SHORT) - phase_p(P_UO_now, SHORT);
  } else {
    t[1][0] = phase_n(P_UO, SHORT) - phase_n(P_UO_now, SHORT);
    t[1][1] = B00100000;
  }
  if(P_LI_now < P_LI) {
    t[2][0] = phase_p(P_LI, LONG) - phase_p(P_LI_now, LONG);
  } else {
    t[2][0] = phase_n(P_LI, LONG) - phase_n(P_LI_now, LONG);
    t[2][1] = B10000000;
  }
  if(P_LO_now < P_LO) {
    t[3][0] = phase_p(P_LO, SHORT) - phase_p(P_LO_now, SHORT);
  } else {
    t[3][0] = phase_n(P_LO, SHORT) - phase_n(P_LO_now, SHORT);
    t[3][1] = B00000010;
  }

  //時間スパンと電磁弁idをソートして短い順に並べ直す
  sort(t);

  //Serial.println(t[0][0]);
  //Serial.println(t[1][0]);
  //Serial.println(t[2][0]);
  //Serial.println(t[3][0]);
  

  //対象電磁弁を開閉
  PORTD = ((t[0][1] | t[1][1] | t[2][1] | t[3][1]) & B11111100) | (PORTD & B00000011);
  PORTB = ((t[0][1] | t[1][1] | t[2][1] | t[3][1]) & B00000011) | (PORTB & B11111100);
  //Serial.println(PORTB, BIN);
  delay(t[0][0]);
  PORTD = (~t[0][1] & PORTD);
  PORTB = (~t[0][1] & PORTB);
  //Serial.println(PORTB, BIN);
  delay(t[1][0] - t[0][0]);
  PORTD = (~t[1][1] & PORTD);
  PORTB = (~t[1][1] & PORTB);
  //Serial.println(PORTB, BIN);
  delay(t[2][0] - t[1][0]);
  PORTD = (~t[2][1] & PORTD);
  PORTB = (~t[2][1] & PORTB);
  //Serial.println(PORTB, BIN);
  delay(t[3][0] - t[2][0]);
  PORTD = (PORTD & B00000011);
  PORTB = (PORTB & B11111100);
  //Serial.println(PORTB, BIN);
}

void loop() {
//
//  for (int i=0; i<60; i++) {
//    Serial.println(length_LO(radians(i)));
//  } while (1);

  movehand(320, 320);
  delay(500);
  movehand(290, 320);
  delay(500);
  movehand(290, 290);
  delay(500);
  movehand(320, 290);
  delay(500);


  //while(true);
}
