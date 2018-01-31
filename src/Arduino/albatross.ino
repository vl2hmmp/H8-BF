#include <Servo.h>

#define SENSOR_TRG   2
#define SENSOR_ECHO  3
#define SENSOR_PWR   4
#define H8_CONTROLE  5
#define SERVO_CTR    6
#define SPEAKER      7
#define H8_ALLOW    13

#define H8_GO        0
#define H8_BACK     90
#define H8_STOP    128
#define H8_LEFT    160
#define H8_RIGHT   255

Servo servo;

void setup() {
  pinMode(SENSOR_TRG,OUTPUT);
  pinMode(SENSOR_ECHO,INPUT);
  pinMode(SENSOR_PWR,OUTPUT);
  pinMode(H8_CONTROLE, OUTPUT);
  pinMode(SPEAKER, OUTPUT);
  pinMode(H8_ALLOW, INPUT);

  servo.attach(SERVO_CTR);
  servo.write(66);

  digitalWrite(SENSOR_TRG, LOW);
  digitalWrite(SENSOR_PWR, HIGH);
  digitalWrite(SPEAKER, LOW);

  H8_controle(H8_STOP);

  Serial.begin(9600);
}

void loop() {

  // H8から指示があるまで停止
  digitalWrite(SPEAKER, LOW);
  H8_controle(H8_STOP);
  while (digitalRead(H8_ALLOW) == LOW);
  digitalWrite(SPEAKER, HIGH);

  // ボタンが見つかるまで左回転
  while (40 <= stableScan()) {
    turnL();
  }

  turnL();
  turnL();
  turnL();

  // ボタンに最適な距離まで近づく
  double dist;
  while (6 <= (dist = stableScan())) {
    H8_controle(H8_BACK);
    delay(300);
    H8_controle(H8_STOP);
    // もしボタンをロストしたら探索
    if (40 < dist) {
      turnL();
      if (40 < stableScan()) {
        turnR();
        turnR();
        if (40 < stableScan())
          break;
      }
    }
  }

  // ボタンを押す
  pressButton();

  // リセットする
  H8_controle(H8_GO);
  delay(400);
  H8_controle(H8_RIGHT);
  delay(800);
  H8_controle(H8_STOP);
}

double turnL() {
  H8_controle(H8_LEFT);
  delay(250);
  H8_controle(H8_STOP);
}

double turnR() {
  H8_controle(H8_RIGHT);
  delay(250);
  H8_controle(H8_STOP);
}

void pressButton() {
  servo.write(90);
  delay(4000);
  servo.write(66);
  delay(4000);
}

double stableScan() {
  delay(800);
  double minvalue = 500.0;
  for (int i = 0; i < 10; i++) {
    double distance = scanSensor();
    minvalue = distance < minvalue ? distance : minvalue;
  }
  Serial.print("dist : ");
  Serial.println(minvalue);
  return minvalue;
}

double scanSensor() {
  digitalWrite(SENSOR_PWR, LOW);
  delay(30);
  digitalWrite(SENSOR_TRG, HIGH);
  delayMicroseconds(9);
  digitalWrite(SENSOR_TRG, LOW);
  delay(10);
  
  digitalWrite(SENSOR_TRG, HIGH);
  delayMicroseconds(9);
  digitalWrite(SENSOR_TRG, LOW);
  double distance = pulseIn(SENSOR_ECHO, HIGH, 30000) * 0.017;
  
  digitalWrite(SENSOR_PWR, HIGH);
  delay(200);

  distance = distance == 0 ? 200 : distance;

  return distance;
}

void H8_controle(int value) {
  analogWrite(H8_CONTROLE, value);
}

