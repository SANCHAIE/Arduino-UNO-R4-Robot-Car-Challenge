// การควบคุม Robot Car UNO R4 (ฉบับทดสอบสำหรับ 4-Pin Motor Driver)
// สำหรับ Motor Driver ที่มี PWM built-in (4 pins: 2 สำหรับมอเตอร์ซ้าย, 2 สำหรับมอเตอร์ขวา)
#include "Arduino_LED_Matrix.h"  // สำหรับ UNO R4 WiFi

// กำหนด pins สำหรับ 4-Pin Motor Driver (Built-in PWM)
// Motor Driver แบบนี้ใช้ PWM signal ในการควบคุมทั้งความเร็วและทิศทาง
#define MOTOR_LEFT_FORWARD    9   // PWM pin สำหรับมอเตอร์ซ้าย - เดินหน้า
#define MOTOR_LEFT_BACKWARD   10  // PWM pin สำหรับมอเตอร์ซ้าย - ถอยหลัง
#define MOTOR_RIGHT_FORWARD   5   // PWM pin สำหรับมอเตอร์ขวา - เดินหน้า
#define MOTOR_RIGHT_BACKWARD  6   // PWM pin สำหรับมอเตอร์ขวา - ถอยหลัง

ArduinoLEDMatrix matrix;  // สำหรับ UNO R4 WiFi

void setup() {
  Serial.begin(115200);

  // ตั้งค่า pins สำหรับ 4-Pin Motor Driver
  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACKWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACKWARD, OUTPUT);

  // LED Matrix (สำหรับ UNO R4 WiFi)
  matrix.begin();

  Serial.println("=== Robot Car Test Started (4-Pin Motor Driver) ===");
  delay(2000);  // รอ 2 วินาทีก่อนเริ่ม
}

// ฟังก์ชันเคลื่อนที่ไปข้างหน้า
void moveForward(int speed, int duration) {
  Serial.print("Moving Forward - Speed: ");
  Serial.print(speed);
  Serial.print(", Duration: ");
  Serial.println(duration);

  // มอเตอร์ซ้าย - เดินหน้า
  analogWrite(MOTOR_LEFT_FORWARD, speed);
  analogWrite(MOTOR_LEFT_BACKWARD, 0);

  // มอเตอร์ขวา - เดินหน้า
  analogWrite(MOTOR_RIGHT_FORWARD, speed);
  analogWrite(MOTOR_RIGHT_BACKWARD, 0);

  delay(duration);
  stopRobot();
}

// ฟังก์ชันเลี้ยวขวา
void turnRight(int speed, int duration) {
  Serial.print("Turning Right - Speed: ");
  Serial.print(speed);
  Serial.print(", Duration: ");
  Serial.println(duration);

  // มอเตอร์ซ้าย - เดินหน้า (เร็ว)
  analogWrite(MOTOR_LEFT_FORWARD, speed);
  analogWrite(MOTOR_LEFT_BACKWARD, 0);

  // มอเตอร์ขวา - ถอยหลัง (หรือหยุด)
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACKWARD, speed);

  delay(duration);
  stopRobot();
}

// ฟังก์ชันเลี้ยวซ้าย
void turnLeft(int speed, int duration) {
  Serial.print("Turning Left - Speed: ");
  Serial.print(speed);
  Serial.print(", Duration: ");
  Serial.println(duration);

  // มอเตอร์ซ้าย - ถอยหลัง (หรือหยุด)
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACKWARD, speed);

  // มอเตอร์ขวา - เดินหน้า (เร็ว)
  analogWrite(MOTOR_RIGHT_FORWARD, speed);
  analogWrite(MOTOR_RIGHT_BACKWARD, 0);

  delay(duration);
  stopRobot();
}

void stopRobot() {
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACKWARD, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACKWARD, 0);
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
