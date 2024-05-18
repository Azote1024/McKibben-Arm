#include <Arduino.h>
#include <math.h>


double length_LI (double theta2) {
  double L1 = 150.0;
  double L2 = 20.0;
  double d = 15.0;
  double l_2 = pow(L2*sin(theta) - d*cos(theta) - d, 2);
  l_2 += pow(L1 - L2*cos(theta) - d*sin(theta), 2);
  return sqrt(l_2);
}

double length_LO (double theta2) {
  double L1 = 20.0;
  double L2 = 20.0;
  double d = 15.0;
  double l_2 = pow(-L2*sin(theta) - d*cos(theta) - d, 2);
  l_2 += pow(L1 - L2*cos(theta) + d*sin(theta), 2);
  return sqrt(l_2);
}

double length_UI (double theta1) {
  double L1 = 150.0;
  double L2 = 20.0;
  double d = 15.0;
  double l_2 = pow(L2*sin(theta) - d*cos(theta) - d, 2);
  l_2 += pow(L1 - L2*cos(theta) - d*sin(theta), 2);
  return sqrt(l_2);
}

double length_UO (double theta1) {
  double L1 = 20.0;
  double L2 = 20.0;
  double d = 15.0;
  double l_2 = pow(-L2*sin(theta) - d*cos(theta) - d, 2);
  l_2 += pow(L1 - L2*cos(theta) + d*sin(theta), 2);
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
  Serial.begin(115200);

  
  //目標値をtheta1=45,theta2=45とする
  double theta1 = 45.0;
  double theta2 = 45.0;
  
  
}


void loop() {
  // put your main code here, to run repeatedly:

}
