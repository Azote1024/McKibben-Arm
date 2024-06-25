#include <Arduino.h>
#include <math.h>

#define OPEN 1
#define CLOSE 0

#define LONG 1
#define SHORT 0

void setup() {
  // put your setup code here, to run once:
  DDRD = DDRD | B11111100;
  pinMode(A5, INPUT);
  randomSeed(12345);

  Serial.begin(115200);
  delay(1000);
}

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


int counter = 0;
int p_target = 0;
int p_now = 0;
int openspan = 0;
void loop() {


  //最初にランダムに目標を決める
  p_target = random(100, 200);
  

  //現在気圧より大きいとき，空気を注入する
  p_now = map(constrain(analogRead(A5),   513, 779),   513, 779, 0, 500);
  
  if (p_target > p_now) {
    //吸気
    openspan = phase_p(p_target, LONG) - phase_p(p_now, LONG);
    if(openspan > 0) {
    //openspan *= 0.1;
    //if(openspan > 30) openspan = 30;
    digitalWrite(9, HIGH);
    delay(openspan);
    digitalWrite(9, LOW);
    }
    
  } else {
    //排気
    openspan = phase_n(p_target, LONG) - phase_n(p_now, LONG);
    if(openspan > 0) {
    //openspan *= 0.1;
    //if(openspan > 30) openspan = 30;
    digitalWrite(8, HIGH);
    delay(openspan);
    digitalWrite(8, LOW);
    }
  }

  //安定するまで待つ
  //delay(10);

  //誤差を出力
  p_now = map(constrain(analogRead(A5),   514, 808),   514, 808, 0, 500);

  Serial.print(p_target);
  Serial.print(",");
  Serial.println(p_now);
  //delay(100);


  counter++;
  //if(counter > 100) {while(true);}
}
