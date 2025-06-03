// การควบคุม Robot Car UNO R4 (ฉบับทดสอบ)
#include "Arduino_LED_Matrix.h"  // สำหรับ UNO R4 WiFi

// กำหนด pins
#define MOTOR_LEFT_PWM    9
#define MOTOR_RIGHT_PWM   10
#define MOTOR_LEFT_DIR1   2
#define MOTOR_LEFT_DIR2   3
#define MOTOR_RIGHT_DIR1  4
#define MOTOR_RIGHT_DIR2  5

ArduinoLEDMatrix matrix;  // สำหรับ UNO R4 WiFi

void setup() {
  Serial.begin(115200);

  // ตั้งค่า pins
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  pinMode(MOTOR_LEFT_DIR1, OUTPUT);
  pinMode(MOTOR_LEFT_DIR2, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR1, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR2, OUTPUT);

  // LED Matrix (สำหรับ UNO R4 WiFi)
  matrix.begin();

  Serial.println("=== Robot Car Test Started ===");
  delay(2000);  // รอ 2 วินาทีก่อนเริ่ม
}

void moveForward(int speed, int duration) {
  Serial.print("Moving Forward - Speed: ");
  Serial.print(speed);
  Serial.print(", Duration: ");
  Serial.println(duration);

  // มอเตอร์ซ้าย - เดินหน้า
  digitalWrite(MOTOR_LEFT_DIR1, HIGH);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);
  analogWrite(MOTOR_LEFT_PWM, speed);

  // มอเตอร์ขวา - เดินหน้า
  digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);
  analogWrite(MOTOR_RIGHT_PWM, speed);

  delay(duration);
  stopRobot();
}

void turnRight(int speed, int duration) {
  Serial.print("Turning Right - Speed: ");
  Serial.print(speed);
  Serial.print(", Duration: ");
  Serial.println(duration);

  // มอเตอร์ซ้าย - เดินหน้า (เร็ว)
  digitalWrite(MOTOR_LEFT_DIR1, HIGH);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);
  analogWrite(MOTOR_LEFT_PWM, speed);

  // มอเตอร์ขวา - ถอยหลัง (หรือหยุด)
  digitalWrite(MOTOR_RIGHT_DIR1, LOW);
  digitalWrite(MOTOR_RIGHT_DIR2, HIGH);
  analogWrite(MOTOR_RIGHT_PWM, speed);

  delay(duration);
  stopRobot();
}

void turnLeft(int speed, int duration) {
  Serial.print("Turning Left - Speed: ");
  Serial.print(speed);
  Serial.print(", Duration: ");
  Serial.println(duration);

  // มอเตอร์ซ้าย - ถอยหลัง (หรือหยุด)
  digitalWrite(MOTOR_LEFT_DIR1, LOW);
  digitalWrite(MOTOR_LEFT_DIR2, HIGH);
  analogWrite(MOTOR_LEFT_PWM, speed);

  // มอเตอร์ขวา - เดินหน้า (เร็ว)
  digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);
  analogWrite(MOTOR_RIGHT_PWM, speed);

  delay(duration);
  stopRobot();
}

void stopRobot() {
  analogWrite(MOTOR_LEFT_PWM, 0);
  analogWrite(MOTOR_RIGHT_PWM, 0);
  delay(500);  // รอครึ่งวินาที
}

void loop() {
  // ทดสอบการเคลื่อนที่พื้นฐาน
  Serial.println("=== Basic Movement Test ===");

  moveForward(150, 1000);   // เดินหน้า 1 วินาที
  delay(1000);

  turnRight(150, 500);      // เลี้ยวขวา 0.5 วินาที
  delay(1000);

  moveForward(150, 1000);   // เดินหน้าอีก 1 วินาที
  delay(1000);

  // หยุดรอจนกว่าจะกด Reset
  while(1) {
    stopRobot();
    delay(1000);
  }
}
