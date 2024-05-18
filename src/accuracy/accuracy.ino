#include <Arduino.h>
#include <math.h>

#define OPEN 1
#define CLOSE 0

void setup() {
  // put your setup code here, to run once:
  DDRD = DDRD | B11111100;
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


  //最初にランダムに目標を決める
  //p_target = random(p_min, p_max);
  //p_target += 4;
  l_target += random(0, 35);
  
  if(p_target > p_max) p_target = 80;
  if(l_target > 35) l_target = 0;

  //現在気圧より大きいとき，空気を注入する
  p_now = analogRead(A5);
  
  if (length2pressure(l_target, BIG) > p_now) {
    double t_1 = phase_p(length2pressure(l_target, BIG));
    double t_0 = phase_p(p_now);
    open_span = (int)(t_1 - t_0);
    PORTD = B01000000 | (PORTD & B00000011);
    delay(open_span);
    PORTD = B00000000 | (PORTD & B00000011);

    //安定するまで待つ
    delay(200);
  } else {
    double t_1 = phase_n(length2pressure(l_target, BIG));
    double t_0 = phase_n(p_now);
    open_span = (int)(t_1 - t_0);
    PORTD = B10000000 | (PORTD & B00000011);
    delay(open_span);
    PORTD = B00000000 | (PORTD & B00000011);
    
    //安定するまで待つ
    delay(200);
  }

  //誤差を出力
  p_now = analogRead(A5);
  //Serial.print(p_target);
  //Serial.print(" -> ");
  //Serial.println(p_now);
  //Serial.println(p_target - p_now);
  //Serial.print(pressure2length(p_now ,BIG));
  //Serial.println(" mm");
  Serial.print(length2pressure(l_target, BIG));
  Serial.print(", ");
  Serial.println(l_target);
  delay(300);


  counter++;
  //if(counter > 100) {while(true);}
}
