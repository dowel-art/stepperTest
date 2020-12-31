/*
   Dowel
   跳ね返し機構　テスト用プログラム
   20.12.31 Thu.
*/

#include <SPI.h>
#include <HighPowerStepperDriver.h>

#define PIN_SENSOR 2 //デジタルピン
#define DIR_FLIP 0
#define DIR_RESET 1

const uint8_t DirPin = 2;
const uint8_t StepPin = 3;
const uint8_t CSPin = 4;

const uint16_t StepPeriodUs_flip = 4;
const uint16_t StepPeriodUs_reset = 100;

HighPowerStepperDriver sd;

void setup() {
  SPI.begin();
  sd.setChipSelectPin(CSPin);

  // Drive the STEP and DIR pins low initially.
  pinMode(StepPin, OUTPUT);
  digitalWrite(StepPin, LOW);
  pinMode(DirPin, OUTPUT);
  digitalWrite(DirPin, LOW);

  //電源ONのための遅延
  delay(1);

  sd.resetSettings();
  sd.clearStatus();

  sd.setDecayMode(HPSDDecayMode::AutoMixed);
  sd.setCurrentMilliamps36v4(1000);

  //分解能の設定
  sd.setStepMode(HPSDStepMode::MicroStep64);

  sd.enableDriver();

  Serial.begin(115200);
  Serial.println("position resetting...");
  posReset();
  Serial.println("system ready!");
}

void loop() {
  if (Serial.available() > 0) {
    char ch = Serial.read();
    if (ch == 'p') {
      flip(30); //打ち出し(引数:角度)
      delay(500);
      posReset(); //位置リセット
    }
  }
  delay(100);
}

//1ステップ回転
void step() {
  // The STEP minimum high pulse width is 1.9 microseconds.
  digitalWrite(StepPin, HIGH);
  delayMicroseconds(3);
  digitalWrite(StepPin, LOW);
  delayMicroseconds(3);
}

//回転方向設定
void setDirection(bool dir) {
  delayMicroseconds(1);
  digitalWrite(DirPin, dir);
  delayMicroseconds(1);
}

//フリップ
void flip(int deg) {
  Serial.println("flip");
  setDirection(DIR_FLIP);
  unsigned int stp = 400.0 * 64.0 * double(deg / 360.0);
  Serial.println(stp);
  for (unsigned int i = 0; i < stp; i++) {
    step();
    delayMicroseconds(StepPeriodUs_flip);
  }
}

//位置リセット
void posReset() {
  Serial.println("position resetting...");
  setDirection(DIR_RESET);
  while (checkSensor() == LOW) {
    step();
    delayMicroseconds(StepPeriodUs_reset);
  }
}

//センサチェック
bool checkSensor() {
  if (digitalRead(PIN_SENSOR)) return true;
  else return false;
}
