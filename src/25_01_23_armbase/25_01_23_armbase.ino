
void setup() {
  pinMode (A0, INPUT); // Lower In
  pinMode (A1, INPUT); // Lower Out
  pinMode (A2, INPUT); // Upper In
  pinMode (A3, INPUT); // Upper Out
  
  pinMode (A4, INPUT);
  pinMode (A5, INPUT);

  pinMode (2, OUTPUT);
  pinMode (3, OUTPUT);
  pinMode (8, OUTPUT);
  pinMode (9, OUTPUT);
  pinMode (10, OUTPUT);
  pinMode (11, OUTPUT);
  pinMode (12, OUTPUT);
  pinMode (13, OUTPUT);

  //いちど全部排気
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

  Serial.begin(115200);
  delay(1000);
}

void loop() {
  Serial.print(" A0:");
  Serial.print(analogRead(A0));
  Serial.print(" A1:");
  Serial.print(analogRead(A1));
  Serial.print(" A2:");
  Serial.print(analogRead(A2));
  Serial.print(" A3:");
  Serial.print(analogRead(A3));

//  Serial.print(" theta1:");
//  Serial.print(analogRead(A4));
//  Serial.print(" theta2:");
//  Serial.println(analogRead(A5));
  Serial.print(" theta1:");
  Serial.print((analogRead(A4)-422)*0.32727);
  Serial.print(" theta2:");
  Serial.println((analogRead(A5)-458)*-0.32727);

  valveAction(20, 20, 20, 20);
  delay(1000);
  valveAction(7, 0, 7, 0);
  delay(1000);
  valveAction(-7, 0, 14, -7);
  delay(1000);
  valveAction(-7, 7, 0, 0);
  delay(1000);
  valveAction(0, 7, 0, 7);
  delay(1000);
  valveAction(7, -7, 0, 7);
  delay(1000);
  valveAction(7, -7, 0, 7);
  delay(1000);
  valveAction(7, -7, 7, 7);
  delay(1000);

  while(true){
    Serial.print(" theta1:");
    Serial.print((analogRead(A4)-422)*0.32727);
    Serial.print(" theta2:");
    Serial.println((analogRead(A5)-458)*-0.32727);
  }
  
  
}

//引数で指定した値の分だけ開閉を行う
//負の値を指定すると排気
void valveAction(int UI, int UO, int LI, int LO){

  // 一番長い時間を記録しておいて，これをループ時間とする
  int maxActionTime = max(UI, max(UO, max(LI, LO)));
  unsigned long startTime = millis();
  
  while(startTime + maxActionTime + 10 > millis()){
    if(UI > 0) {
      if(startTime + UI > millis()) digitalWrite(2, HIGH);
      else digitalWrite(2, LOW);
    }
    else if (UI < 0){
      if(startTime - UI > millis()) digitalWrite(3, HIGH);
      else digitalWrite(3, LOW);
    }
    // 時間が0のときは無視

    if(UO > 0) {
      if(startTime + UO > millis()) digitalWrite(8, HIGH);
      else digitalWrite(8, LOW);
    }
    else if (UO < 0){
      if(startTime - UO > millis()) digitalWrite(9, HIGH);
      else digitalWrite(9, LOW);
    }
    // 時間が0のときは無視

    if(LI > 0) {
      if(startTime + LI > millis()) digitalWrite(10, HIGH);
      else digitalWrite(10, LOW);
    }
    else if (LI < 0){
      if(startTime - LI > millis()) digitalWrite(11, HIGH);
      else digitalWrite(11, LOW);
    }
    // 時間が0のときは無視

    if(LO > 0) {
      if(startTime + LO > millis()) digitalWrite(12, HIGH);
      else digitalWrite(12, LOW);
    }
    else if (LO < 0){
      if(startTime - LO > millis()) digitalWrite(13, HIGH);
      else digitalWrite(13, LOW);
    }
    // 時間が0のときは無視
    
  }
}
