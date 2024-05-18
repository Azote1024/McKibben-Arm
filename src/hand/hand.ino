#include <Arduino.h>
#include <math.h>

#define OPEN 1
#define CLOSE 0

double length_LI (double theta2) {
  double L1 = 150.0;
  double L2 = 20.0;
  double d = 15.0;
  double l_2 = pow(L2*sin(theta2) - d*cos(theta2) - d, 2);
  l_2 += pow(L1 - L2*cos(theta2) - d*sin(theta2), 2);
  return sqrt(l_2);
}

double length_LO (double theta2) {
  double L1 = 20.0;
  double L2 = 20.0;
  double d = 15.0;
  double l_2 = pow(-L2*sin(theta2) - d*cos(theta2) - d, 2);
  l_2 += pow(L1 - L2*cos(theta2) + d*sin(theta2), 2);
  return sqrt(l_2);
}

double length_UI (double theta1) {
  double L1 = 150.0;
  double L2 = 20.0;
  double d = 15.0;
  double l_2 = pow(L2*sin(theta1) - d*cos(theta1) - d, 2);
  l_2 += pow(L1 - L2*cos(theta1) - d*sin(theta1), 2);
  return sqrt(l_2);
}

double length_UO (double theta1) {
  double L1 = 20.0;
  double L2 = 20.0;
  double d = 15.0;
  double l_2 = pow(-L2*sin(theta1) - d*cos(theta1) - d, 2);
  l_2 += pow(L1 - L2*cos(theta1) + d*sin(theta1), 2);
  return sqrt(l_2);
}

double INNER_ARM_LENGTH = 250;
double OUTER_ARM_LENGTH = 250;

//theta must be radian
double FK_x (double theta1, double theta2) {
  return INNER_ARM_LENGTH*cos(theta1) + OUTER_ARM_LENGTH*cos(theta1 + theta2);
}

double FK_y (double theta1, double theta2) {
  return INNER_ARM_LENGTH*sin(theta1) + OUTER_ARM_LENGTH*sin(theta1 + theta2);
}

double IK_theta2 (double x, double y) {
  return acos((pow(x,2) + pow(y,2) - pow(INNER_ARM_LENGTH,2) - pow(OUTER_ARM_LENGTH,2))/(2*INNER_ARM_LENGTH*OUTER_ARM_LENGTH));
}

double IK_theta1 (double x, double y) {
  double let = -OUTER_ARM_LENGTH*sin(IK_theta2(x,y))*x;
  let += (INNER_ARM_LENGTH+OUTER_ARM_LENGTH*cos(IK_theta2(x,y)))*y;
  double let2 = (INNER_ARM_LENGTH + OUTER_ARM_LENGTH*cos(IK_theta2(x,y)))*x;
  let2 += OUTER_ARM_LENGTH*sin(IK_theta2(x,y))*y;
  return atan2(let, let2);
}


void setup() {
  // put your setup code here, to run once:
  DDRD = DDRD | B11111100;
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  randomSeed(12345);

  Serial.begin(115200);
  delay(1000);
}

double p_max = 360.0;
double p_min = 50.0;
double p_target = 50.0;
double p_now = 50.0;

double l_target = 0;
double l_now = 0;

double tau_p = 42.0;
double tau_n = 44.0;

int open_span = 0;

//内圧が増えていくときのt
double phase_p(double p) {
  double temp = log((p_max - p_min)/(p_max - p));
  return tau_p * temp;
}

//内圧が減っていくときのt
double phase_n(double p) {
  double temp = log((p_max - p_min)/(p - p_min));
  return tau_n * temp;
}


# define BIG 1
# define SMALL 0
double pressure2length (double p, bool isBigMuscle) {
  double l = 0;
  if (isBigMuscle) {
    l = -4.18603921*pow(p/100.0, 2.0) + 0.309201499*p  - 22.7761843;
  } else {
    l = -4.18603921*pow(p/100.0, 2.0) + 0.309201499*p  - 22.7761843;
  }
  return constrain(l, 0, 40);
}

double length2pressure (double l, bool isBigMuscle) {
  double p = 0;
  if (isBigMuscle) {
    p = 0.0110877718*pow(l, 3.0) - 0.237524052*pow(l, 2.0) + 2.84832974*l  + 110.973994;
  } else {
    p = 0.52336117*pow(l, 2.0) - 3.88669594*l  + 118.45379718;
  }
  return constrain(p, 40, 450);
}



int counter = 0;
void loop() {
  PORTD = B10011000 | (PORTD & B00000011);
  PORTB = B00000001 | (PORTD & B11111100);
  delay(1000);
  PORTD = B00000000 | (PORTD & B00000011);
  PORTD = B00000000 | (PORTD & B11111100);
  
  Serial.print("A2:");
  Serial.print(analogRead(A2));
  Serial.print(" A3:");
  Serial.print(analogRead(A3));
  Serial.print(" A4:");
  Serial.print(analogRead(A4));
  Serial.print(" A5:");
  Serial.println(analogRead(A5));
  delay(1000);

  PORTD = B01100100 | (PORTD & B00000011);
  PORTB = B00000010 | (PORTD & B11111100);
  delay(1000);
  PORTD = B00000000 | (PORTD & B00000011);
  PORTD = B00000000 | (PORTD & B11111100);
  
  Serial.print("A2:");
  Serial.print(analogRead(A2));
  Serial.print(" A3:");
  Serial.print(analogRead(A3));
  Serial.print(" A4:");
  Serial.print(analogRead(A4));
  Serial.print(" A5:");
  Serial.println(analogRead(A5));
  delay(1000);
  
  
//  //最初にランダムに目標を決める
//  //p_target = random(p_min, p_max);
//  //p_target += 4;
//  l_target += random(0, 35);
//  
//  if(p_target > p_max) p_target = 80;
//  if(l_target > 35) l_target = 0;
//
//  //現在気圧より大きいとき，空気を注入する
//  p_now = analogRead(A5);
//  
//  if (length2pressure(l_target, BIG) > p_now) {
//    double t_1 = phase_p(length2pressure(l_target, BIG));
//    double t_0 = phase_p(p_now);
//    open_span = (int)(t_1 - t_0);
//    PORTD = B01000000 | (PORTD & B00000011);
//    delay(open_span);
//    PORTD = B00000000 | (PORTD & B00000011);
//
//    //安定するまで待つ
//    delay(200);
//  } else {
//    double t_1 = phase_n(length2pressure(l_target, BIG));
//    double t_0 = phase_n(p_now);
//    open_span = (int)(t_1 - t_0);
//    PORTD = B10000000 | (PORTD & B00000011);
//    delay(open_span);
//    PORTD = B00000000 | (PORTD & B00000011);
//    
//    //安定するまで待つ
//    delay(200);
//  }

}
