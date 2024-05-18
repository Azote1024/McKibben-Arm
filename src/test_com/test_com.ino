
 

int range = 20;
float ref = 50.0;
int pressure = 50;
int sig = 1;

void setRef(int target){
  pressure = analogRead(A0);
  if(target + range/2 < pressure) {
    digitalWrite(3, HIGH);
  } else {
    digitalWrite(3, LOW);
  }
  if(target - range/2 > pressure) {
    digitalWrite(2, HIGH);
  } else {
    digitalWrite(2, LOW);
  }
}

void setup() {
    Serial.begin(9600); // ボーレートを9600に設定
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(A0, INPUT);
}

void loop() {
//    if (ref > 200) sig = -1;
//    else if (ref < 50) sig = 1;
//
//    ref += (sig*0.1);
//    setRef((int)ref);
//    Serial.println(ref);
//    Serial.println(pressure);
digitalWrite(3, LOW);
digitalWrite(2, HIGH);
delay(1000);
digitalWrite(2, LOW);
digitalWrite(3, HIGH);
delay(1000);
}
