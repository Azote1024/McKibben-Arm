#include <Arduino.h>

#define OPEN 1
#define CLOSE 0

#define INTAKE 1
#define EXHAUST 0

// 一ループの動作にかけるミリ秒
#define FRAME 1000

#define DEBUG_MODE 1

class ArtificialMuscle {
  private:
    int intake;
    int exhaust;
    bool driveDirection;
    unsigned long openDuration;
    unsigned long startingTime;
    String muscleName;
    bool isDriving = false;

  public:
    // 人工筋肉に対応するピン番号で初期化する
    // 筋肉名はデバッグ用
    ArtificialMuscle(int in, int ex, String mn){
      intake = in;
      exhaust = ex;
      muscleName = mn;
      pinMode(intake, OUTPUT);
      pinMode(exhaust, OUTPUT);
      digitalWrite(intake, CLOSE);
      digitalWrite(exhaust, CLOSE);
    }

    // 動作方向と動作時間を予約する
    // 動作開始・停止のフラグはisDrivingで管理
    void set(bool di, unsigned long op) {
      openDuration = constrain(op, 0, FRAME);
      driveDirection = di;
      isDriving = true;
    }

    // 開始時間をインスタンスに保存しておく
    void start(unsigned int st) {
      startingTime = st;
      if (driveDirection == INTAKE && isDriving) {
        digitalWrite(intake, OPEN);
        #if DEBUG_MODE
          Serial.println(muscleName + " intake open");
        #endif
      } else if (driveDirection == EXHAUST && isDriving) {
        digitalWrite(exhaust, OPEN);
        #if DEBUG_MODE
          Serial.println(muscleName + " exhaust open");
        #endif
      }
    }

    // 閉じる時間の判定
    // openDurationに指定したミリ秒だけ該当する電磁弁を開放する
    void drive() {
      //millis()がtime + durationより大きかったらおわりにして閉じる
      if (millis() > (startingTime + openDuration) && isDriving) {
        digitalWrite(intake, CLOSE);
        digitalWrite(exhaust, CLOSE);
        isDriving = false;
        #if DEBUG_MODE
          Serial.println(muscleName + " closed");
        #endif
      }
    }

    // 終了処理，どんな場合でも一コマ分の作業が終わった後には閉じる
    // 連続的に開く場合でも閉じる時間が一瞬ならば動作には関係ない
    void kill() {
      digitalWrite(intake, CLOSE);
      digitalWrite(exhaust, CLOSE);
      #if DEBUG_MODE
          Serial.println(muscleName + " killed");
      #endif
    }
};

ArtificialMuscle muscleUI(2, 3, "Upper In"); // 吸気電磁弁ピン: 2, 排気電磁弁ピン: 3
ArtificialMuscle muscleUO(4, 5, "Upper Out"); // 吸気電磁弁ピン: 4, 排気電磁弁ピン: 5
ArtificialMuscle muscleLI(6, 7, "Lower In"); // 吸気電磁弁ピン: 6, 排気電磁弁ピン: 7
ArtificialMuscle muscleLO(8, 9, "Lower Out"); // 吸気電磁弁ピン: 8, 排気電磁弁ピン: 9

// あんまり良くない設計
// インスタンスを作るだけで実行キューに入るようにすべき
void play() {
  unsigned long currentTime = millis();
  muscleUI.start(millis());
  muscleUO.start(millis());
  muscleLI.start(millis());
  muscleLO.start(millis());
  //Todo whileで監視して順次閉じる，driveを回そう
  while(millis() < currentTime + FRAME) {
    muscleUI.drive();
    muscleUO.drive();
    muscleLI.drive();
    muscleLO.drive();
  }
  muscleUI.kill();
  muscleUO.kill();
  muscleLI.kill();
  muscleLO.kill();
}


void setup() {
  Serial.begin(115200);
}
void loop() {
  muscleUI.set(INTAKE, 500);
  muscleLI.set(INTAKE, 500); 
  play(); 

  muscleUI.set(EXHAUST, 500);
  muscleUO.set(INTAKE, 500);
  play();

  muscleLI.set(EXHAUST, 500);
  muscleLO.set(INTAKE, 500);
  play();

  muscleLO.set(EXHAUST, 500);
  play();
  //while(true) {}
}
