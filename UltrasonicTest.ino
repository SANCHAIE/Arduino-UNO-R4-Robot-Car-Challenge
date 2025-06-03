// ทดสอบ Ultrasonic Sensor HC-SR04 สำหรับ Arduino UNO R4
// เชื่อมต่อ: VCC->5V, GND->GND, Trig->Pin 6, Echo->Pin 7

#define TRIG_PIN 6
#define ECHO_PIN 7

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  Serial.println("=== Ultrasonic Sensor Test ===");
  Serial.println("Trig: Pin 6, Echo: Pin 7");
  Serial.println("Distance readings every 500ms");
  Serial.println("==============================");
}

float getDistance() {
  // ส่งสัญญาณ Trigger
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // อ่านค่าจาก Echo pin
  long duration = pulseIn(ECHO_PIN, HIGH);
  
  // คำนวณระยะทาง (เสียงเดินทาง 2 เท่า)
  float distance = duration * 0.034 / 2;
  
  return distance;
}

void loop() {
  float distance = getDistance();
  
  Serial.print("Distance: ");
  Serial.print(distance, 1);
  Serial.print(" cm");
  
  // แสดงสถานะ
  if (distance < 5) {
    Serial.println(" - TOO CLOSE!");
  } else if (distance < 20) {
    Serial.println(" - OBSTACLE DETECTED");
  } else if (distance < 50) {
    Serial.println(" - CAUTION");
  } else if (distance > 400) {
    Serial.println(" - OUT OF RANGE");
  } else {
    Serial.println(" - CLEAR");
  }
  
  delay(500);
}
