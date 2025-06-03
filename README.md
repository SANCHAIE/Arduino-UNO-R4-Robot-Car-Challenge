# ใบงาน Arduino UNO R4 Robot Car Challenge (ฉบับปฏิบัติ)

**ระดับ: มัธยมปลาย | ระยะเวลา: 60 นาที | เน้นการทดสอบจริง**

## 🎯 วัตถุประสงค์หลัก

- ควบคุม Robot Car ด้วย Arduino UNO R4 ให้เคลื่อนที่ได้
- **ทดสอบหาค่าความเร็วและเวลาสำหรับการหมุน 90 องศาที่แม่นยำ**
- สร้าง Square Spiral ด้วยการเลี้ยวที่แม่นยำ

## 🛠 อุปกรณ์

- Arduino UNO R4 (WiFi หรือ ธรรมดา)
- Motor Driver Board (L298N หรือ Maker Drive)
- Robot Car Chassis + 2 DC Motors + 2 Wheels
- Jumper Wires
- แบตเตอรี่ 6V หรือ Power Bank
- ไม้บรรทัด/เทปวัด (สำหรับวัดระยะทาง)
- ปากกาเพื่อทำเครื่องหมาย

## 🔌 การเชื่อมต่อ

```
Motor Driver → Arduino UNO R4
PWM1 (ENA)   → Pin 9   (ควบคุมความเร็วมอเตอร์ซ้าย)
PWM2 (ENB)   → Pin 10  (ควบคุมความเร็วมอเตอร์ขวา)
M1A (IN1)    → Pin 2   (ทิศทางมอเตอร์ซ้าย)
M1B (IN2)    → Pin 3
M2A (IN3)    → Pin 4   (ทิศทางมอเตอร์ขวา)
M2B (IN4)    → Pin 5
VCC          → 5V
GND          → GND

```

## 📝 ส่วนที่ 1: โค้ดพื้นฐาน (10 นาที)

```cpp
// การควบคุม Robot Car UNO R4 (ฉบับทดสอบ)

// กำหนด pins
#define MOTOR_LEFT_PWM    9
#define MOTOR_RIGHT_PWM   10
#define MOTOR_LEFT_DIR1   2
#define MOTOR_LEFT_DIR2   3
#define MOTOR_RIGHT_DIR1  4
#define MOTOR_RIGHT_DIR2  5

void setup() {
  Serial.begin(115200);

  // ตั้งค่า pins
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  pinMode(MOTOR_LEFT_DIR1, OUTPUT);
  pinMode(MOTOR_LEFT_DIR2, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR1, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR2, OUTPUT);

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

```

**งานที่ 1:** อัปโหลดโค้ดนี้และทดสอบว่า Robot Car เคลื่อนที่ได้หรือไม่

---

## 🧪 ส่วนที่ 2: การทดสอบหาค่าผ่าน Advanced Dashboard (20 นาที)

### วิธีการทดสอบแบบใหม่:

### **ขั้นตอนที่ 1: เตรียมการทดสอบ**

1. **เตรียมพื้นที่ทดสอบ:** หาพื้นเรียบ วางกระดาษ A4 หรือใช้ไม้บรรทัด
2. **Setup Advanced Dashboard:** อัปโหลดโค้ด Advanced Dashboard จากส่วนที่ 5
3. **เชื่อมต่อ:** เปิดมือถือไปที่ IP Address ที่ได้

### **ขั้นตอนที่ 2: ทดสอบหาค่า Turn Duration แบบ Interactive**

```
การทดสอบผ่าน Advanced Dashboard:
1. 📱 เปิด WiFi Dashboard ในมือถือ
2. ⚙️ ไปที่ Motor Settings Panel
3. 🔧 ลองปรับค่า Turn Duration (เริ่มจาก 500ms)
4. 💾 กด "Apply Settings"
5. 🎯 วางหุ่นยนต์ที่จุดเริ่มต้น ทำเครื่องหมายทิศทาง
6. 🎮 กดปุ่ม "➡️ RIGHT" ใน Direct Control
7. 📝 ดู Live Log ว่าใช้เวลาเท่าไหร่
8. 📏 วัดมุมที่หมุนได้จริง
9. 🔄 ปรับค่า Turn Duration และทดสอบซ้ำ

```

### **ขั้นตอนที่ 3: ทดสอบโดยใช้ Sequence Builder**

```
วิธีทดสอบอย่างเป็นระบบ:
1. 📝 สร้าง Test Sequence:
   - เพิ่ม FORWARD (2000ms)
   - เพิ่ม RIGHT (ค่าที่ต้องการทดสอบ)
   - เพิ่ม FORWARD (2000ms)
   - เพิ่ม RIGHT (ค่าเดิม)
   - เพิ่ม FORWARD (2000ms)
   - เพิ่ม RIGHT (ค่าเดิม)
   - เพิ่ม FORWARD (2000ms)
   - เพิ่ม RIGHT (ค่าเดิม)

2. 🚀 กด "Execute All Commands"
3. 📐 ดูว่าหุ่นยนต์กลับมาตำแหน่งเดิมหรือไม่
4. 📊 ดู Live Log ข้อมูลการทำงานทั้งหมด
5. 🔧 แก้ไขค่าและทดสอบใหม่

```

### 📊 ตัวอย่างการบันทึกผลการทดสอบ:

| การทดสอบ | Turn Duration | มุมที่หมุนได้ | หมายเหตุ |
| --- | --- | --- | --- |
| ครั้งที่ 1 | 500ms | ~70° | น้อยไป |
| ครั้งที่ 2 | 700ms | ~85° | ใกล้แล้ว |
| ครั้งที่ 3 | 750ms | ~92° | มากไปนิด |
| ครั้งที่ 4 | 720ms | ~90° | ✅ พอดี! |

### 💡 ข้อดีของการทดสอบผ่าน Advanced Dashboard:

- ✅ **ปรับค่าได้ทันที:** ไม่ต้องแก้โค้ดและอัปโหลดใหม่
- ✅ **ดูผลแบบ Real-time:** Live Log แสดงทุกขั้นตอน
- ✅ **ทดสอบแบบ Sequence:** สร้างลำดับการทดสอบที่ซับซ้อน
- ✅ **บันทึกค่าอัตโนมัติ:** ระบบจำค่าที่ตั้งไว้
- ✅ **ไม่ต้องเสียบ USB:** ใช้แบตเตอรี่อิสระได้

### 📊 ตารางบันทึกผลการทดสอบ:

| ครั้งที่ | Speed | Duration (ms) | มุมที่หมุนจริง | ความแม่นยำ | หมายเหตุ |
| --- | --- | --- | --- | --- | --- |
| 1 | 150 | 500 | ___° | ____% |  |
| 2 | 150 | 600 | ___° | ____% |  |
| 3 | 150 | 700 | ___° | ____% |  |
| 4 | 150 | 800 | ___° | ____% |  |
| 5 | 150 | ___ | 90° | 100% | ค่าที่ดีที่สุด |

### 💡 เทคนิคการวัดมุม:

**วิธีที่ 1: ใช้กระดาษตาราง**

- วางกระดาษตารางใต้ Robot Car
- ทำเครื่องหมายทิศทางเริ่มต้น
- หลังหมุน ดูว่าหันไปทิศทางไหน

**วิธีที่ 2: ใช้วัตถุอ้างอิง**

- ตั้ง Robot Car ให้หันไปหาผนัง
- หลังหมุน 90° ควรหันไปข้างๆ ผนัง

**วิธีที่ 3: ใช้เข็มทิศหรือแอพมือถือ**

- ใช้แอพ compass บนมือถือ
- วัดทิศทางก่อนและหลังหมุน

### 🔧 การปรับแต่งค่า:

```
ถ้าหมุนได้ไม่ถึง 90° → เพิ่มค่า duration
ถ้าหมุนเกิน 90°      → ลดค่า duration

ตัวอย่าง:
- หมุนได้ 60° (500ms) → ลองเพิ่มเป็น 700ms
- หมุนได้ 120° (800ms) → ลองลดเป็น 650ms

```

---

## 🎯 ส่วนที่ 3: การทดสอบความเร็วและระยะทาง (15 นาที)

### ทดสอบความเร็วผ่าน WiFi Dashboard:

```
การทดสอบใหม่:
1. 📱 เปิด WiFi Dashboard
2. 📏 วางไม้บรรทัดหรือวัดระยะ 30cm
3. 🎮 กดปุ่ม "⬆️ FORWARD" ในหน้าเว็บ
4. 📝 ดู Live Log เวลาการเคลื่อนที่
5. 📐 วัดระยะทางจริงที่เคลื่อนที่ได้
6. 🔢 คำนวณความเร็ว

```

### 📱 การใช้ WiFi Dashboard ดูผลการทดสอบ:

**ตัวอย่าง Live Log:**

```
[15.2s] 🎮 Command: FORWARD
[15.3s] ⬆️ Moving Forward (Speed: 200)
[16.3s] ✅ Forward Movement Completed

```

**จาก Log นี้:**

- เวลาเคลื่อนที่ = 16.3 - 15.3 = 1.0 วินาที
- ถ้าระยะทางจริง = 25 cm
- ความเร็ว = 25 cm ÷ 1.0 s = 25 cm/s

### 📏 ตารางบันทึกการทดสอบความเร็ว:

| PWM Speed | เวลา (ms) | ระยะทางจริง (cm) | ความเร็ว (cm/s) | หมายเหตุ |
| --- | --- | --- | --- | --- |
| 100 | 2000 | _____ cm | _____ cm/s |  |
| 150 | 2000 | _____ cm | _____ cm/s |  |
| 200 | 2000 | _____ cm | _____ cm/s | แนะนำ |
| 255 | 2000 | _____ cm | _____ cm/s |  |

**การคำนวณ:**

```
ความเร็ว (cm/s) = ระยะทางจริง ÷ (เวลา ÷ 1000)

```

---

## 🏁 ส่วนที่ 4: Square Spiral Challenge (15 นาที)

เมื่อหาค่าที่เหมาะสมแล้ว ให้นำไปใส่ในโค้ด WiFi Dashboard ส่วนที่ 5:

### 🎮 การใช้ WiFi Dashboard ทำ Square Spiral:

```
ขั้นตอนการใช้งาน:
1. 📱 เปิด WiFi Dashboard ในมือถือ
2. ⚙️ ตรวจสอบ Settings ที่แสดงในหน้าเว็บ:
   - Move Speed: 200
   - Turn Speed: 150
   - Turn Duration: 700ms (ค่าที่ทดสอบได้)
3. 🎯 วางหุ่นยนต์ที่จุดเริ่มต้น
4. 🔄 กดปุ่ม "Execute Square Spiral"
5. 📝 ดู Live Log การทำงานแบบ real-time

```

### 📝 ตัวอย่าง Live Log ขณะทำ Square Spiral:

```
[45.2s] 🎮 Command: SQUARE
[45.3s] 🔄 Starting Square Spiral Mission
[45.4s] 🏁 Square Spiral: Side 1 (Long)
[45.5s] ⬆️ Moving Forward (Speed: 200)
[48.5s] ✅ Forward Movement Completed
[49.0s] 🔄 Turn 1: Right 90°
[49.1s] ➡️ Turning Right (Duration: 700ms)
[49.8s] ✅ Right Turn Completed
[50.3s] 🏁 Square Spiral: Side 2 (Medium)
[50.4s] ⬆️ Moving Forward (Speed: 200)
[52.4s] ✅ Forward Movement Completed
[52.9s] 🔄 Turn 2: Right 90°
...
[65.2s] 🎉 Square Spiral Completed Successfully!

```

### 🔧 การปรับแต่งผ่าน WiFi Dashboard:

```cpp
// ในโค้ด WiFi Dashboard (ส่วนที่ 5) ให้แก้ค่าเหล่านี้:
int turnDuration90 = 700;  // ใส่ค่าที่ทดสอบได้จากส่วนที่ 2
int moveSpeed = 200;       // ปรับตามต้องการ
int turnSpeed = 150;       // ปรับตามต้องการ

```

**งานที่ต้องทำ:**

1. ทดสอบหาค่า turnDuration90 ที่แม่นยำจากส่วนที่ 2
2. นำค่าที่ได้มาใส่ในโค้ด WiFi Dashboard
3. ทดสอบ Square Spiral ผ่านปุ่มในหน้าเว็บ
4. ดู Live Log เพื่อติดตามการทำงาน

---

## 💡 เทคนิคแก้ปัญหา

### ปัญหาที่พบบ่อย:

1. **Robot Car ไม่เคลื่อนที่:**
    - ตรวจสอบการเชื่อมต่อสาย
    - ตรวจสอบแบตเตอรี่
    - ลองเพิ่มค่า PWM
2. **หมุนไม่แม่นยำ:**
    - ตรวจสอบความเรียบของพื้นผิว
    - ปรับค่า duration ทีละนิด
    - ตรวจสอบล้อว่าแน่นหรือไม่
3. **เคลื่อนที่คดโค้ง:**
    - ปรับค่า PWM ของมอเตอร์แต่ละข้าง
    - ตรวจสอบการติดตั้งล้อ

### 🔧 การปรับแต่งขั้นสูง:

```cpp
// ปรับความเร็วมอเตอร์แต่ละข้างให้เท่ากัน
void moveForwardCalibrated(int baseSpeed, int duration) {
  int leftSpeed = baseSpeed + 5;   // ปรับให้มอเตอร์ซ้าย
  int rightSpeed = baseSpeed - 3;  // ปรับให้มอเตอร์ขวา

  // ใช้ค่าที่ปรับแล้ว
  analogWrite(MOTOR_LEFT_PWM, leftSpeed);
  analogWrite(MOTOR_RIGHT_PWM, rightSpeed);

  delay(duration);
  stopRobot();
}

```

---

## 🚀 ความท้าทายเพิ่มเติม (สำหรับทีมที่เสร็จเร็ว)

1. **หมุน 45° และ 30°:** หาค่าสำหรับมุมที่ซับซ้อนขึ้น
2. **สร้างรูปทรงอื่น:** สามเหลี่ยม, หกเหลี่ยม, วงกลม
3. **ควบคุมด้วยระยะทาง:** ใช้เวลาที่คำนวณจากระยะทางจริง
4. **เพิ่ม LED Pattern:** แสดงทิศทางบน LED Matrix (UNO R4 WiFi)

---

## 📱 ส่วนที่ 5: WiFi Dashboard + Live Monitoring (UNO R4 WiFi Only)

### 🌐 การใช้ WiFi Dashboard แทน Serial Monitor

**ข้อดี:** ไม่ต้องเสียบ USB! ใช้แบตเตอรี่อิสระ + ดูสถานะผ่านมือถือได้

### 🛠 Setup WiFi Dashboard พร้อม Live Log

```cpp
// โค้ดสมบูรณ์สำหรับ WiFi Dashboard + Sequence Programming
#include <WiFiS3.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_NAME";      // ใส่ชื่อ WiFi
const char* password = "YOUR_PASSWORD";   // ใส่รหัส WiFi

WiFiServer server(80);

// Motor pins (เหมือนเดิม)
#define MOTOR_LEFT_PWM    9
#define MOTOR_RIGHT_PWM   10
#define MOTOR_LEFT_DIR1   2
#define MOTOR_LEFT_DIR2   3
#define MOTOR_RIGHT_DIR1  4
#define MOTOR_RIGHT_DIR2  5

// ตัวแปรสำหรับการตั้งค่า (ปรับได้ผ่าน WiFi Dashboard)
int moveSpeed = 200;
int turnSpeed = 150;
int moveDuration = 1000;
int turnDuration = 700;

// ตัวแปรสำหรับ Sequence Programming
struct Command {
  String action;
  int speed;
  int duration;
};

Command commandSequence[20];  // รองรับ sequence ได้สูงสุด 20 คำสั่ง
int sequenceLength = 0;
bool isExecutingSequence = false;

// ตัวแปรสำหรับ Dashboard + Live Log
String lastCommand = "STOP";
String systemLog = "";
unsigned long commandTime = 0;
int totalCommands = 0;
unsigned long startTime = 0;

void setup() {
  Serial.begin(115200);
  startTime = millis();

  // ตั้งค่า motor pins
  setupMotorPins();

  // เชื่อมต่อ WiFi
  connectToWiFi();

  addLog("🤖 Robot Car Advanced System Started");
  addLog("📡 WiFi Dashboard Ready with Sequence Programming");
  addLog("🔗 URL: http://" + WiFi.localIP().toString());
  addLog("⚙️ Features: Speed control, Sequence builder, Preset patterns");

  Serial.println("=== Advanced WiFi Dashboard Ready ===");
  Serial.print("Dashboard URL: http://");
  Serial.println(WiFi.localIP());
  Serial.println("เปิดมือถือไปที่ URL นี้เพื่อใช้ Advanced Features");
}

void setupMotorPins() {
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  pinMode(MOTOR_LEFT_DIR1, OUTPUT);
  pinMode(MOTOR_LEFT_DIR2, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR1, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR2, OUTPUT);
}

void connectToWiFi() {
  addLog("🔌 Connecting to WiFi: " + String(ssid));

  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    addLog(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    addLog("✅ WiFi Connected!");
    addLog("📶 IP Address: " + WiFi.localIP().toString());
    server.begin();
  } else {
    addLog("❌ WiFi Connection Failed");
  }
}

// ฟังก์ชัน Log ที่แสดงใน WiFi Dashboard
void addLog(String message) {
  String timestamp = String((millis() - startTime) / 1000.0, 1) + "s";
  String logEntry = "[" + timestamp + "] " + message;

  // เก็บ log ใน string
  systemLog += logEntry + "<br>";

  // เก็บแค่ 20 บรรทัดล่าสุด
  int lineCount = 0;
  for (int i = 0; i < systemLog.length(); i++) {
    if (systemLog.substring(i, i+4) == "<br>") lineCount++;
  }

  while (lineCount > 20) {
    int firstBr = systemLog.indexOf("<br>");
    if (firstBr >= 0) {
      systemLog = systemLog.substring(firstBr + 4);
      lineCount--;
    } else {
      break;
    }
  }

  // ยังส่งไป Serial Monitor ด้วย (ถ้ามีการเชื่อมต่อ)
  Serial.println(logEntry);
}

// ฟังก์ชันควบคุมหุ่นยนต์ แบบใหม่ รองรับ Custom Speed/Duration
void executeCommand(String command, int customSpeed = -1, int customDuration = -1) {
  lastCommand = command;
  commandTime = millis();
  totalCommands++;

  // ใช้ค่าที่กำหนดเอง หรือค่า default
  int useSpeed = (customSpeed > 0) ? customSpeed :
                 (command == "FORWARD" || command == "BACKWARD") ? moveSpeed : turnSpeed;
  int useDuration = (customDuration > 0) ? customDuration :
                    (command == "FORWARD" || command == "BACKWARD") ? moveDuration : turnDuration;

  addLog("🎮 Command: " + command + " (Speed: " + String(useSpeed) + ", Duration: " + String(useDuration) + "ms)");

  if (command == "FORWARD") {
    moveForward(useSpeed, useDuration);
  }
  else if (command == "BACKWARD") {
    moveBackward(useSpeed, useDuration);
  }
  else if (command == "LEFT") {
    turnLeft(useSpeed, useDuration);
  }
  else if (command == "RIGHT") {
    turnRight(useSpeed, useDuration);
  }
  else if (command == "STOP") {
    stopRobot();
    addLog("⏹️ Robot Stopped");
  }
  else if (command == "DELAY") {
    addLog("⏱️ Waiting " + String(useDuration) + "ms...");
    delay(useDuration);
    addLog("✅ Wait completed");
  }
}

// ฟังก์ชันสำหรับ Sequence Programming
void addToSequence(String action, int speed, int duration) {
  if (sequenceLength < 20) {
    commandSequence[sequenceLength].action = action;
    commandSequence[sequenceLength].speed = speed;
    commandSequence[sequenceLength].duration = duration;
    sequenceLength++;
    addLog("📝 Added to sequence: " + action + " (" + String(duration) + "ms)");
  } else {
    addLog("⚠️ Sequence full! Maximum 20 commands.");
  }
}

void executeSequence() {
  if (sequenceLength == 0) {
    addLog("⚠️ No commands in sequence to execute");
    return;
  }

  isExecutingSequence = true;
  addLog("🚀 Executing sequence (" + String(sequenceLength) + " commands)");

  for (int i = 0; i < sequenceLength; i++) {
    if (!isExecutingSequence) break;  // ถ้าถูกหยุดระหว่างทาง

    addLog("📋 Step " + String(i+1) + "/" + String(sequenceLength) + ": " + commandSequence[i].action);
    executeCommand(commandSequence[i].action, commandSequence[i].speed, commandSequence[i].duration);

    delay(200);  // เว้นระยะระหว่างคำสั่ง
  }

  addLog("🎉 Sequence execution completed!");
  isExecutingSequence = false;
}

void clearSequence() {
  sequenceLength = 0;
  isExecutingSequence = false;
  addLog("🧹 Sequence cleared");
}

void loadPresetSequence(String preset) {
  clearSequence();

  if (preset == "square") {
    addToSequence("FORWARD", moveSpeed, 2000);
    addToSequence("RIGHT", turnSpeed, turnDuration);
    addToSequence("FORWARD", moveSpeed, 2000);
    addToSequence("RIGHT", turnSpeed, turnDuration);
    addToSequence("FORWARD", moveSpeed, 2000);
    addToSequence("RIGHT", turnSpeed, turnDuration);
    addToSequence("FORWARD", moveSpeed, 2000);
    addToSequence("RIGHT", turnSpeed, turnDuration);
    addLog("📦 Loaded preset: Square pattern");
  }
  else if (preset == "spiral") {
    addToSequence("FORWARD", moveSpeed, 3000);
    addToSequence("RIGHT", turnSpeed, turnDuration);
    addToSequence("FORWARD", moveSpeed, 2000);
    addToSequence("RIGHT", turnSpeed, turnDuration);
    addToSequence("FORWARD", moveSpeed, 2000);
    addToSequence("RIGHT", turnSpeed, turnDuration);
    addToSequence("FORWARD", moveSpeed, 1000);
    addLog("📦 Loaded preset: Square Spiral pattern");
  }
  else if (preset == "figure8") {
    addToSequence("FORWARD", moveSpeed, 1500);
    addToSequence("RIGHT", turnSpeed, turnDuration * 2);
    addToSequence("FORWARD", moveSpeed, 1500);
    addToSequence("LEFT", turnSpeed, turnDuration * 2);
    addToSequence("FORWARD", moveSpeed, 1500);
    addToSequence("LEFT", turnSpeed, turnDuration * 2);
    addToSequence("FORWARD", moveSpeed, 1500);
    addToSequence("RIGHT", turnSpeed, turnDuration * 2);
    addLog("📦 Loaded preset: Figure-8 pattern");
  }
}

void moveForward(int speed, int duration) {
  addLog("⬆️ Moving Forward (Speed: " + String(speed) + ", Duration: " + String(duration) + "ms)");

  digitalWrite(MOTOR_LEFT_DIR1, HIGH);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);
  analogWrite(MOTOR_LEFT_PWM, speed);

  digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);
  analogWrite(MOTOR_RIGHT_PWM, speed);

  delay(duration);
  stopRobot();
  addLog("✅ Forward Movement Completed");
}

void moveBackward(int speed, int duration) {
  addLog("⬇️ Moving Backward (Speed: " + String(speed) + ", Duration: " + String(duration) + "ms)");

  digitalWrite(MOTOR_LEFT_DIR1, LOW);
  digitalWrite(MOTOR_LEFT_DIR2, HIGH);
  analogWrite(MOTOR_LEFT_PWM, speed);

  digitalWrite(MOTOR_RIGHT_DIR1, LOW);
  digitalWrite(MOTOR_RIGHT_DIR2, HIGH);
  analogWrite(MOTOR_RIGHT_PWM, speed);

  delay(duration);
  stopRobot();
  addLog("✅ Backward Movement Completed");
}

void turnLeft(int speed, int duration) {
  addLog("⬅️ Turning Left (Speed: " + String(speed) + ", Duration: " + String(duration) + "ms)");

  digitalWrite(MOTOR_LEFT_DIR1, LOW);
  digitalWrite(MOTOR_LEFT_DIR2, HIGH);
  analogWrite(MOTOR_LEFT_PWM, speed);

  digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);
  analogWrite(MOTOR_RIGHT_PWM, speed);

  delay(duration);
  stopRobot();
  addLog("✅ Left Turn Completed");
}

void turnRight(int speed, int duration) {
  addLog("➡️ Turning Right (Speed: " + String(speed) + ", Duration: " + String(duration) + "ms)");

  digitalWrite(MOTOR_LEFT_DIR1, HIGH);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);
  analogWrite(MOTOR_LEFT_PWM, speed);

  digitalWrite(MOTOR_RIGHT_DIR1, LOW);
  digitalWrite(MOTOR_RIGHT_DIR2, HIGH);
  analogWrite(MOTOR_RIGHT_PWM, speed);

  delay(duration);
  stopRobot();
  addLog("✅ Right Turn Completed");
}

void stopRobot() {
  analogWrite(MOTOR_LEFT_PWM, 0);
  analogWrite(MOTOR_RIGHT_PWM, 0);
  delay(100);
}

// สร้าง HTML Dashboard + Sequence Programming
String createAdvancedDashboardHTML() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>🤖 Robot Car Advanced Dashboard</title>";
  html += "<style>";
  // CSS styles (เหมือนใน preview)
  html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 15px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; min-height: 100vh; }";
  html += ".container { max-width: 1200px; margin: 0 auto; }";
  html += ".header { text-align: center; margin-bottom: 20px; }";
  html += ".header h1 { font-size: 2.2em; margin: 0; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); }";
  html += ".main-grid { display: grid; grid-template-columns: 1fr 2fr; gap: 20px; margin-bottom: 20px; }";
  html += ".card { background: rgba(255,255,255,0.1); backdrop-filter: blur(10px); border-radius: 15px; padding: 15px; box-shadow: 0 8px 32px rgba(0,0,0,0.1); }";
  html += ".btn { background: rgba(255,255,255,0.2); border: 2px solid rgba(255,255,255,0.3); color: white; padding: 12px; border-radius: 8px; font-size: 1em; cursor: pointer; transition: all 0.3s; text-decoration: none; display: block; }";
  html += ".btn:hover { background: rgba(255,255,255,0.3); transform: translateY(-2px); }";
  html += ".input-group { margin: 10px 0; } .input-group label { display: block; margin-bottom: 5px; font-weight: bold; font-size: 0.9em; }";
  html += ".input-group input, .input-group select { width: 100%; padding: 8px; border: 2px solid rgba(255,255,255,0.3); border-radius: 5px; background: rgba(255,255,255,0.1); color: white; font-size: 1em; }";
  html += ".control-grid { display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 8px; max-width: 280px; margin: 15px auto; }";
  html += ".sequence-container { max-height: 200px; overflow-y: auto; border: 2px solid rgba(255,255,255,0.2); border-radius: 10px; padding: 10px; margin: 10px 0; background: rgba(0,0,0,0.2); }";
  html += ".log-container { background: rgba(0,0,0,0.3); padding: 15px; border-radius: 10px; max-height: 300px; overflow-y: auto; font-family: 'Courier New', monospace; font-size: 0.85em; line-height: 1.4; }";
  html += "@media (max-width: 900px) { .main-grid { grid-template-columns: 1fr; } }";
  html += "</style>";
  html += "<script>";
  html += "function sendCommand(cmd) { fetch('/command?action=' + cmd).then(() => { setTimeout(() => { location.reload(); }, 300); }); }";
  html += "function updateSettings() { var ms=document.getElementById('moveSpeed').value; var ts=document.getElementById('turnSpeed').value; var md=document.getElementById('moveDuration').value; var td=document.getElementById('turnDuration').value; fetch('/settings?ms='+ms+'&ts='+ts+'&md='+md+'&td='+td).then(() => { location.reload(); }); }";
  html += "function addToSequence() { var cmd=document.getElementById('commandSelect').value; var dur=document.getElementById('customDuration').value; fetch('/sequence?action=add&cmd='+cmd+'&dur='+dur).then(() => { location.reload(); }); }";
  html += "function executeSequence() { fetch('/sequence?action=execute').then(() => { location.reload(); }); }";
  html += "function clearSequence() { fetch('/sequence?action=clear').then(() => { location.reload(); }); }";
  html += "function loadPreset() { var preset=document.getElementById('presetSelect').value; if(preset) fetch('/sequence?action=preset&pattern='+preset).then(() => { location.reload(); }); }";
  html += "setTimeout(() => { location.reload(); }, 5000);";  // Auto refresh ทุก 5 วินาที
  html += "</script>";
  html += "</head><body>";

  html += "<div class='container'>";
  html += "<div class='header'>";
  html += "<h1>🤖 Robot Car Advanced Dashboard</h1>";
  html += "<p>Arduino UNO R4 WiFi - Programming & Real-time Control</p>";
  html += "</div>";

  html += "<div class='main-grid'>";

  // Left Panel: Settings & Controls
  html += "<div>";

  // Settings Panel
  html += "<div class='card'>";
  html += "<h3>⚙️ Motor Settings</h3>";
  html += "<div class='input-group'><label>Move Speed (0-255):</label><input type='number' id='moveSpeed' value='" + String(moveSpeed) + "' min='0' max='255'></div>";
  html += "<div class='input-group'><label>Turn Speed (0-255):</label><input type='number' id='turnSpeed' value='" + String(turnSpeed) + "' min='0' max='255'></div>";
  html += "<div class='input-group'><label>Move Duration (ms):</label><input type='number' id='moveDuration' value='" + String(moveDuration) + "' min='100' max='10000' step='100'></div>";
  html += "<div class='input-group'><label>Turn Duration (ms):</label><input type='number' id='turnDuration' value='" + String(turnDuration) + "' min='100' max='5000' step='50'></div>";
  html += "<button class='btn' onclick='updateSettings()'>💾 Apply Settings</button>";
  html += "</div>";

  // Direct Controls
  html += "<div class='card' style='text-align: center;'>";
  html += "<h3>🎮 Direct Control</h3>";
  html += "<div class='control-grid'>";
  html += "<div></div><a href='#' class='btn' onclick='sendCommand(\"FORWARD\")'>⬆️<br>FORWARD</a><div></div>";
  html += "<a href='#' class='btn' onclick='sendCommand(\"LEFT\")'>⬅️<br>LEFT</a>";
  html += "<a href='#' class='btn' onclick='sendCommand(\"STOP\")'>⏹️<br>STOP</a>";
  html += "<a href='#' class='btn' onclick='sendCommand(\"RIGHT\")'>➡️<br>RIGHT</a>";
  html += "<div></div><a href='#' class='btn' onclick='sendCommand(\"BACKWARD\")'>⬇️<br>BACKWARD</a><div></div>";
  html += "</div></div>";

  // Status
  html += "<div class='card'>";
  html += "<h3>📊 Status</h3>";
  html += "<div>Commands: <strong>" + String(totalCommands) + "</strong></div>";
  html += "<div>Uptime: <strong>" + String((millis()-startTime)/1000) + "s</strong></div>";
  html += "<div>Sequence: <strong>" + String(sequenceLength) + " commands</strong></div>";
  html += "</div>";

  html += "</div>";

  // Right Panel: Sequence Builder & Log
  html += "<div>";

  // Sequence Builder
  html += "<div class='card'>";
  html += "<h3>📝 Command Sequence Builder</h3>";
  html += "<div style='display: grid; grid-template-columns: 1fr 1fr auto; gap: 10px; margin-bottom: 15px;'>";
  html += "<select id='commandSelect'>";
  html += "<option value='FORWARD'>⬆️ Forward</option>";
  html += "<option value='BACKWARD'>⬇️ Backward</option>";
  html += "<option value='LEFT'>⬅️ Turn Left</option>";
  html += "<option value='RIGHT'>➡️ Turn Right</option>";
  html += "<option value='STOP'>⏹️ Stop</option>";
  html += "<option value='DELAY'>⏱️ Wait</option>";
  html += "</select>";
  html += "<input type='number' id='customDuration' placeholder='Duration (ms)' min='100' max='10000' step='100'>";
  html += "<button class='btn' onclick='addToSequence()'>➕</button>";
  html += "</div>";

  // Preset Patterns
  html += "<div style='display: grid; grid-template-columns: 1fr auto; gap: 10px; margin-bottom: 15px;'>";
  html += "<select id='presetSelect'>";
  html += "<option value=''>Choose Preset...</option>";
  html += "<option value='square'>🔲 Square</option>";
  html += "<option value='spiral'>🌀 Spiral</option>";
  html += "<option value='figure8'>∞ Figure-8</option>";
  html += "</select>";
  html += "<button class='btn' onclick='loadPreset()'>📦</button>";
  html += "</div>";

  // Sequence Display
  html += "<div class='sequence-container'>";
  if (sequenceLength == 0) {
    html += "<div style='text-align: center; opacity: 0.7; padding: 20px;'>📝 No commands in sequence</div>";
  } else {
    for (int i = 0; i < sequenceLength; i++) {
      String emoji = "🎮";
      if (commandSequence[i].action == "FORWARD") emoji = "⬆️";
      else if (commandSequence[i].action == "BACKWARD") emoji = "⬇️";
      else if (commandSequence[i].action == "LEFT") emoji = "⬅️";
      else if (commandSequence[i].action == "RIGHT") emoji = "➡️";
      else if (commandSequence[i].action == "STOP") emoji = "⏹️";
      else if (commandSequence[i].action == "DELAY") emoji = "⏱️";

      html += "<div style='display: flex; justify-content: space-between; padding: 8px; margin: 5px 0; background: rgba(255,255,255,0.1); border-radius: 5px; font-family: monospace;'>";
      html += "<span>" + String(i+1) + ". " + emoji + " " + commandSequence[i].action + "</span>";
      html += "<span>" + String(commandSequence[i].duration) + "ms</span>";
      html += "</div>";
    }
  }
  html += "</div>";

  // Sequence Controls
  html += "<div style='display: flex; gap: 5px; margin: 10px 0;'>";
  html += "<button class='btn' onclick='executeSequence()' style='flex: 2; background: rgba(255,69,0,0.4); border-color: rgba(255,69,0,0.6);'>🚀 Execute All</button>";
  html += "<button class='btn' onclick='clearSequence()' style='flex: 1;'>🧹 Clear</button>";
  html += "</div>";

  html += "</div>";

  // Live Log
  html += "<div class='card'>";
  html += "<h3>📝 Live System Log</h3>";
  html += "<div class='log-container'>";
  html += systemLog;
  html += "</div>";
  html += "</div>";

  html += "</div></div>";
  html += "<div style='font-size: 0.8em; opacity: 0.7; text-align: center; margin-top: 10px;'>🔄 Auto-refresh every 5 seconds | Advanced Programming Mode</div>";
  html += "</div></body></html>";

  return html;
}

void handleWebServer() {
  WiFiClient client = server.available();

  if (client) {
    String request = "";
    while (client.connected() && client.available()) {
      String line = client.readStringUntil('\n');
      if (line.length() == 1 && line[0] == '\n') break;
      request += line;
    }

    // ตรวจสอบคำสั่งที่ส่งมา
    if (request.indexOf("GET /command?action=") >= 0) {
      String command = extractParameter(request, "action");
      executeCommand(command);

      sendResponse(client, "Command executed: " + command);
    }
    else if (request.indexOf("GET /settings?") >= 0) {
      // อัปเดตการตั้งค่า
      String ms = extractParameter(request, "ms");
      String ts = extractParameter(request, "ts");
      String md = extractParameter(request, "md");
      String td = extractParameter(request, "td");

      if (ms.length() > 0) moveSpeed = ms.toInt();
      if (ts.length() > 0) turnSpeed = ts.toInt();
      if (md.length() > 0) moveDuration = md.toInt();
      if (td.length() > 0) turnDuration = td.toInt();

      addLog("⚙️ Settings updated - Move: " + String(moveSpeed) + "/" + String(moveDuration) + "ms, Turn: " + String(turnSpeed) + "/" + String(turnDuration) + "ms");

      sendResponse(client, "Settings updated");
    }
    else if (request.indexOf("GET /sequence?") >= 0) {
      String action = extractParameter(request, "action");

      if (action == "add") {
        String cmd = extractParameter(request, "cmd");
        String durStr = extractParameter(request, "dur");
        int duration = (durStr.length() > 0) ? durStr.toInt() :
                      (cmd == "FORWARD" || cmd == "BACKWARD") ? moveDuration : turnDuration;
        int speed = (cmd == "FORWARD" || cmd == "BACKWARD") ? moveSpeed : turnSpeed;

        addToSequence(cmd, speed, duration);
      }
      else if (action == "execute") {
        executeSequence();
      }
      else if (action == "clear") {
        clearSequence();
      }
      else if (action == "preset") {
        String pattern = extractParameter(request, "pattern");
        loadPresetSequence(pattern);
      }

      sendResponse(client, "Sequence action completed");
    }
    else {
      // แสดง Dashboard
      String html = createAdvancedDashboardHTML();

      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html; charset=UTF-8");
      client.println("Connection: close");
      client.println();
      client.println(html);
    }

    client.stop();
  }
}

String extractParameter(String request, String paramName) {
  int start = request.indexOf(paramName + "=");
  if (start == -1) return "";

  start += paramName.length() + 1;
  int end = request.indexOf("&", start);
  if (end == -1) end = request.indexOf(" ", start);
  if (end == -1) end = request.length();

  return request.substring(start, end);
}

void sendResponse(WiFiClient client, String message) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println("Connection: close");
  client.println();
  client.println(message);
}

void loop() {
  handleWebServer();
  delay(10);  // ลด CPU usage
}

void setupMotorPins() {
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  pinMode(MOTOR_LEFT_DIR1, OUTPUT);
  pinMode(MOTOR_LEFT_DIR2, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR1, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR2, OUTPUT);
}

void connectToWiFi() {
  addLog("🔌 Connecting to WiFi: " + String(ssid));

  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    addLog(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    addLog("✅ WiFi Connected!");
    addLog("📶 IP Address: " + WiFi.localIP().toString());
    server.begin();
  } else {
    addLog("❌ WiFi Connection Failed");
  }
}

// ฟังก์ชัน Log ที่แสดงใน WiFi Dashboard
void addLog(String message) {
  String timestamp = String((millis() - startTime) / 1000.0, 1) + "s";
  String logEntry = "[" + timestamp + "] " + message;

  // เก็บ log ใน string
  systemLog += logEntry + "<br>";

  // เก็บแค่ 15 บรรทัดล่าสุด
  int lineCount = 0;
  for (int i = 0; i < systemLog.length(); i++) {
    if (systemLog.substring(i, i+4) == "<br>") lineCount++;
  }

  while (lineCount > 15) {
    int firstBr = systemLog.indexOf("<br>");
    if (firstBr >= 0) {
      systemLog = systemLog.substring(firstBr + 4);
      lineCount--;
    } else {
      break;
    }
  }

  // ยังส่งไป Serial Monitor ด้วย (ถ้ามีการเชื่อมต่อ)
  Serial.println(logEntry);
}

// ฟังก์ชันควบคุมหุ่นยนต์ พร้อม Live Log
void executeCommand(String command) {
  lastCommand = command;
  commandTime = millis();
  totalCommands++;

  addLog("🎮 Command: " + command);

  if (command == "FORWARD") {
    addLog("⬆️ Moving Forward (Speed: " + String(moveSpeed) + ")");
    moveForward(moveSpeed, 1000);
    addLog("✅ Forward Movement Completed");
  }
  else if (command == "BACKWARD") {
    addLog("⬇️ Moving Backward (Speed: " + String(moveSpeed) + ")");
    moveBackward(moveSpeed, 1000);
    addLog("✅ Backward Movement Completed");
  }
  else if (command == "LEFT") {
    addLog("⬅️ Turning Left (Duration: " + String(turnDuration90) + "ms)");
    turnLeft(turnSpeed, turnDuration90);
    addLog("✅ Left Turn Completed");
  }
  else if (command == "RIGHT") {
    addLog("➡️ Turning Right (Duration: " + String(turnDuration90) + "ms)");
    turnRight(turnSpeed, turnDuration90);
    addLog("✅ Right Turn Completed");
  }
  else if (command == "STOP") {
    addLog("⏹️ Robot Stopped");
    stopRobot();
  }
  else if (command == "SQUARE") {
    addLog("🔄 Starting Square Spiral Mission");
    squareSpiral();
    addLog("🏁 Square Spiral Mission Completed");
  }
  else if (command == "TEST_TURN") {
    addLog("🧪 Testing 90° Turn");
    testTurn90();
    addLog("📊 Turn Test Completed");
  }
}

void moveForward(int speed, int duration) {
  digitalWrite(MOTOR_LEFT_DIR1, HIGH);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);
  analogWrite(MOTOR_LEFT_PWM, speed);

  digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);
  analogWrite(MOTOR_RIGHT_PWM, speed);

  delay(duration);
  stopRobot();
}

void moveBackward(int speed, int duration) {
  digitalWrite(MOTOR_LEFT_DIR1, LOW);
  digitalWrite(MOTOR_LEFT_DIR2, HIGH);
  analogWrite(MOTOR_LEFT_PWM, speed);

  digitalWrite(MOTOR_RIGHT_DIR1, LOW);
  digitalWrite(MOTOR_RIGHT_DIR2, HIGH);
  analogWrite(MOTOR_RIGHT_PWM, speed);

  delay(duration);
  stopRobot();
}

void turnLeft(int speed, int duration) {
  digitalWrite(MOTOR_LEFT_DIR1, LOW);
  digitalWrite(MOTOR_LEFT_DIR2, HIGH);
  analogWrite(MOTOR_LEFT_PWM, speed);

  digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);
  analogWrite(MOTOR_RIGHT_PWM, speed);

  delay(duration);
  stopRobot();
}

void turnRight(int speed, int duration) {
  digitalWrite(MOTOR_LEFT_DIR1, HIGH);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);
  analogWrite(MOTOR_LEFT_PWM, speed);

  digitalWrite(MOTOR_RIGHT_DIR1, LOW);
  digitalWrite(MOTOR_RIGHT_DIR2, HIGH);
  analogWrite(MOTOR_RIGHT_PWM, speed);

  delay(duration);
  stopRobot();
}

void stopRobot() {
  analogWrite(MOTOR_LEFT_PWM, 0);
  analogWrite(MOTOR_RIGHT_PWM, 0);
  delay(100);
}

void testTurn90() {
  addLog("📐 Place robot at starting position");
  addLog("📐 Mark the direction it's facing");
  delay(3000);

  addLog("🔄 Testing 90° turn with current settings");
  turnRight(turnSpeed, turnDuration90);

  addLog("📏 Check if robot turned exactly 90°");
  addLog("⚙️ Adjust turnDuration90 value if needed");
}

void squareSpiral() {
  addLog("🏁 Square Spiral: Side 1 (Long)");
  moveForward(moveSpeed, 3000);
  delay(500);

  addLog("🔄 Turn 1: Right 90°");
  turnRight(turnSpeed, turnDuration90);
  delay(500);

  addLog("🏁 Square Spiral: Side 2 (Medium)");
  moveForward(moveSpeed, 2000);
  delay(500);

  addLog("🔄 Turn 2: Right 90°");
  turnRight(turnSpeed, turnDuration90);
  delay(500);

  addLog("🏁 Square Spiral: Side 3 (Medium)");
  moveForward(moveSpeed, 2000);
  delay(500);

  addLog("🔄 Turn 3: Right 90°");
  turnRight(turnSpeed, turnDuration90);
  delay(500);

  addLog("🏁 Square Spiral: Side 4 (Short)");
  moveForward(moveSpeed, 1000);

  addLog("🎉 Square Spiral Completed Successfully!");
}

// สร้าง HTML Dashboard + Remote Control + Live Log
String createDashboardHTML() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>🤖 Robot Car Live Dashboard</title>";
  html += "<style>";
  html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 15px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; }";
  html += ".container { max-width: 900px; margin: 0 auto; }";
  html += ".header { text-align: center; margin-bottom: 20px; }";
  html += ".header h1 { font-size: 2.2em; margin: 0; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); }";
  html += ".dashboard { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; margin-bottom: 20px; }";
  html += ".card { background: rgba(255,255,255,0.1); backdrop-filter: blur(10px); border-radius: 15px; padding: 15px; box-shadow: 0 8px 32px rgba(0,0,0,0.1); }";
  html += ".card h3 { margin-top: 0; font-size: 1.1em; }";
  html += ".status-value { font-size: 1.8em; font-weight: bold; margin: 8px 0; }";
  html += ".controls { text-align: center; }";
  html += ".control-grid { display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 8px; max-width: 280px; margin: 15px auto; }";
  html += ".btn { background: rgba(255,255,255,0.2); border: 2px solid rgba(255,255,255,0.3); color: white; padding: 12px; border-radius: 8px; font-size: 1em; cursor: pointer; transition: all 0.3s; text-decoration: none; display: block; }";
  html += ".btn:hover { background: rgba(255,255,255,0.3); transform: translateY(-2px); }";
  html += ".btn-large { grid-column: span 3; background: rgba(255,215,0,0.3); border-color: rgba(255,215,0,0.5); margin-top: 10px; }";
  html += ".btn-test { grid-column: span 3; background: rgba(0,255,127,0.3); border-color: rgba(0,255,127,0.5); }";
  html += ".empty { grid-column: 1; } .forward { grid-column: 2; } .turn-left { grid-column: 1; } .stop { grid-column: 2; } .turn-right { grid-column: 3; } .backward { grid-column: 2; }";
  html += ".log-container { background: rgba(0,0,0,0.3); padding: 15px; border-radius: 10px; max-height: 300px; overflow-y: auto; font-family: 'Courier New', monospace; font-size: 0.85em; line-height: 1.4; }";
  html += ".log-container::-webkit-scrollbar { width: 8px; } .log-container::-webkit-scrollbar-track { background: rgba(255,255,255,0.1); border-radius: 4px; } .log-container::-webkit-scrollbar-thumb { background: rgba(255,255,255,0.3); border-radius: 4px; }";
  html += "@media (max-width: 600px) { .dashboard { grid-template-columns: 1fr; } .control-grid { max-width: 250px; } }";
  html += ".auto-refresh { font-size: 0.8em; opacity: 0.7; text-align: center; margin-top: 10px; }";
  html += ".full-width { grid-column: span 2; }";
  html += "</style>";
  html += "<script>";
  html += "function sendCommand(cmd) { fetch('/command?action=' + cmd).then(() => { setTimeout(() => { location.reload(); }, 300); }); }";
  html += "setTimeout(() => { location.reload(); }, 3000);";  // Auto refresh ทุก 3 วินาที
  html += "</script>";
  html += "</head><body>";

  html += "<div class='container'>";
  html += "<div class='header'>";
  html += "<h1>🤖 Robot Car Live Dashboard</h1>";
  html += "<p>Arduino UNO R4 WiFi - Real-time Monitoring</p>";
  html += "</div>";

  // Dashboard Status
  html += "<div class='dashboard'>";
  html += "<div class='card'>";
  html += "<h3>📊 System Status</h3>";
  html += "<div>Last Command: <span class='status-value'>" + lastCommand + "</span></div>";
  html += "<div>Total Commands: <span class='status-value'>" + String(totalCommands) + "</span></div>";
  html += "<div>Uptime: <span class='status-value'>" + String((millis()-startTime)/1000) + "s</span></div>";
  html += "</div>";

  html += "<div class='card'>";
  html += "<h3>⚙️ Settings</h3>";
  html += "<div>Move Speed: <span class='status-value'>" + String(moveSpeed) + "</span></div>";
  html += "<div>Turn Speed: <span class='status-value'>" + String(turnSpeed) + "</span></div>";
  html += "<div>Turn Duration: <span class='status-value'>" + String(turnDuration90) + "ms</span></div>";
  html += "</div>";
  html += "</div>";

  // Remote Control
  html += "<div class='card controls'>";
  html += "<h3>🎮 Remote Control</h3>";
  html += "<div class='control-grid'>";
  html += "<div class='empty'></div>";
  html += "<a href='#' class='btn forward' onclick='sendCommand(\"FORWARD\")'>⬆️<br>FORWARD</a>";
  html += "<div class='empty'></div>";

  html += "<a href='#' class='btn turn-left' onclick='sendCommand(\"LEFT\")'>⬅️<br>LEFT</a>";
  html += "<a href='#' class='btn stop' onclick='sendCommand(\"STOP\")'>⏹️<br>STOP</a>";
  html += "<a href='#' class='btn turn-right' onclick='sendCommand(\"RIGHT\")'>➡️<br>RIGHT</a>";

  html += "<div class='empty'></div>";
  html += "<a href='#' class='btn backward' onclick='sendCommand(\"BACKWARD\")'>⬇️<br>BACKWARD</a>";
  html += "<div class='empty'></div>";
  html += "</div>";

  html += "<a href='#' class='btn btn-test' onclick='sendCommand(\"TEST_TURN\")'>🧪 Test 90° Turn</a>";
  html += "<a href='#' class='btn btn-large' onclick='sendCommand(\"SQUARE\")'>🔄 Execute Square Spiral</a>";
  html += "</div>";

  // Live Log Display
  html += "<div class='card full-width'>";
  html += "<h3>📝 Live System Log</h3>";
  html += "<div class='log-container'>";
  html += systemLog;
  html += "</div>";
  html += "</div>";

  html += "<div class='auto-refresh'>🔄 Auto-refresh every 3 seconds | WiFi: " + WiFi.localIP().toString() + "</div>";
  html += "</div>";
  html += "</body></html>";

  return html;
}

void handleWebServer() {
  WiFiClient client = server.available();

  if (client) {
    String request = "";
    while (client.connected() && client.available()) {
      String line = client.readStringUntil('\n');
      if (line.length() == 1 && line[0] == '\n') break;
      request += line;
    }

    // ตรวจสอบคำสั่งที่ส่งมา
    if (request.indexOf("GET /command?action=") >= 0) {
      String command = "";
      int actionStart = request.indexOf("action=") + 7;
      int actionEnd = request.indexOf(" ", actionStart);
      if (actionEnd == -1) actionEnd = request.indexOf("&", actionStart);
      if (actionEnd == -1) actionEnd = request.length();

      command = request.substring(actionStart, actionEnd);
      command.toUpperCase();

      // ดำเนินการตามคำสั่ง
      executeCommand(command);

      // ส่งคำตอบกลับ
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println("Command executed: " + command);
    }
    else {
      // แสดง Dashboard
      String html = createDashboardHTML();

      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html; charset=UTF-8");
      client.println("Connection: close");
      client.println();
      client.println(html);
    }

    client.stop();
  }
}

void loop() {
  handleWebServer();
  delay(10);  // ลด CPU usage
}

```

### 🎯 วิธีใช้งาน WiFi Dashboard (แทน Serial Monitor):

### **ขั้นตอนที่ 1: Setup แรก**

```
1. เสียบ USB กับคอม (สำหรับอัปโหลดโค้ด)
2. แก้ WiFi name/password ในโค้ด
3. อัปโหลดโค้ด
4. เปิด Serial Monitor ครั้งเดียว เพื่อดู IP Address
5. จด IP Address ลงกระดาษ (เช่น 192.168.1.45)

```

### **ขั้นตอนที่ 2: ใช้งานจริง**

```
1. ถอด USB จากคอม
2. เสียบ Power Bank หรือแบตเตอรี่
3. เปิดมือถือ พิมพ์ IP Address ในเบราว์เซอร์
4. ดู Live Log แทน Serial Monitor!

```

### 📱 ฟีเจอร์ WiFi Dashboard:

### **📊 Real-time Monitoring:**

- **System Status:** คำสั่งล่าสุด, จำนวนคำสั่ง, เวลาทำงาน
- **Settings Display:** ความเร็ว, เวลาหมุน
- **Live Log:** แสดงกิจกรรมแบบ real-time (15 บรรทัดล่าสุด)

### **🎮 Remote Control:**

- **6 ปุ่มพื้นฐาน:** เดินหน้า, ถอยหลัง, เลี้ยว, หยุด
- **🧪 Test 90° Turn:** สำหรับทดสอบหาค่าที่เหมาะสม
- **🔄 Square Spiral:** รัน mission อัตโนมัติ

### **📝 Live Log Examples:**

```
[2.3s] 🤖 Robot Car System Started
[4.1s] 📡 WiFi Dashboard Ready
[15.7s] 🎮 Command: FORWARD
[15.8s] ⬆️ Moving Forward (Speed: 200)
[16.9s] ✅ Forward Movement Completed
[22.3s] 🎮 Command: RIGHT
[22.4s] ➡️ Turning Right (Duration: 700ms)
[23.2s] ✅ Right Turn Completed

```

### 🔧 การทดสอบหาค่า Turn Duration:

### **ผ่าน WiFi Dashboard:**

```
1. กด "🧪 Test 90° Turn"
2. ดู Live Log: "📐 Place robot at starting position"
3. จัดตำแหน่งหุ่นยนต์
4. ดู Live Log: "🔄 Testing 90° turn..."
5. ตรวจสอบมุมที่หมุนจริง
6. แก้ค่า turnDuration90 ในโค้ด
7. อัปโหลดใหม่และทดสอบซ้ำ

```

### 💡 ข้อดีของ WiFi Dashboard:

- ✅ **ไม่ต้องเสียบ USB:** ใช้แบตเตอรี่อิสระได้
- ✅ **ดูได้หลายคน:** ครู + นักเรียนดูพร้อมกัน
- ✅ **Mobile Friendly:** ใช้มือถือได้สะดวก
- ✅ **Real-time:** อัปเดตทุก 3 วินาที
- ✅ **บันทึกสมบูรณ์:** เห็นทุกขั้นตอนใน Live Log

// Motor pins (เหมือนเดิม)
#define MOTOR_LEFT_PWM    9
#define MOTOR_RIGHT_PWM   10
#define MOTOR_LEFT_DIR1   2
#define MOTOR_LEFT_DIR2   3
#define MOTOR_RIGHT_DIR1  4
#define MOTOR_RIGHT_DIR2  5

// ค่าที่ได้จากการทดสอบ
int turnDuration90 = 700;  // ใส่ค่าที่ทดสอบได้
int moveSpeed = 200;
int turnSpeed = 150;

// ตัวแปรสำหรับ Dashboard
String lastCommand = "STOP";
unsigned long commandTime = 0;
int batteryLevel = 100;  // จำลอง
int totalCommands = 0;

void setup() {
Serial.begin(115200);

// ตั้งค่า motor pins
setupMotorPins();

// เชื่อมต่อ WiFi
connectToWiFi();

Serial.println("WiFi Robot Car Dashboard Ready!");
Serial.print("Dashboard URL: http://");
Serial.println(WiFi.localIP());
}

void setupMotorPins() {
pinMode(MOTOR_LEFT_PWM, OUTPUT);
pinMode(MOTOR_RIGHT_PWM, OUTPUT);
pinMode(MOTOR_LEFT_DIR1, OUTPUT);
pinMode(MOTOR_LEFT_DIR2, OUTPUT);
pinMode(MOTOR_RIGHT_DIR1, OUTPUT);
pinMode(MOTOR_RIGHT_DIR2, OUTPUT);
}

void connectToWiFi() {
Serial.print("Connecting to WiFi: ");
Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}

Serial.println("");
Serial.println("WiFi connected!");
server.begin();
}

void showWiFiConnected() {
// แสดงสัญลักษณ์ WiFi บน LED Matrix
byte wifiIcon[8][12] = {
{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
{0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0},
{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
{1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1},
{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
matrix.loadFrame(wifiIcon);
}

// ฟังก์ชันควบคุมหุ่นยนต์ (เหมือนเดิมแต่เพิ่ม logging)
void executeCommand(String command) {
lastCommand = command;
commandTime = millis();
totalCommands++;

Serial.print("Executing: ");
Serial.println(command);

if (command == "FORWARD") {
moveForward(moveSpeed, 1000);
}
else if (command == "BACKWARD") {
moveBackward(moveSpeed, 1000);
}
else if (command == "LEFT") {
turnLeft(turnSpeed, turnDuration90);
}
else if (command == "RIGHT") {
turnRight(turnSpeed, turnDuration90);
}
else if (command == "STOP") {
stopRobot();
}
else if (command == "SQUARE") {
squareSpiral();
}
}

void moveForward(int speed, int duration) {
digitalWrite(MOTOR_LEFT_DIR1, HIGH);
digitalWrite(MOTOR_LEFT_DIR2, LOW);
analogWrite(MOTOR_LEFT_PWM, speed);

digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
digitalWrite(MOTOR_RIGHT_DIR2, LOW);
analogWrite(MOTOR_RIGHT_PWM, speed);

delay(duration);
stopRobot();
}

void moveBackward(int speed, int duration) {
digitalWrite(MOTOR_LEFT_DIR1, LOW);
digitalWrite(MOTOR_LEFT_DIR2, HIGH);
analogWrite(MOTOR_LEFT_PWM, speed);

digitalWrite(MOTOR_RIGHT_DIR1, LOW);
digitalWrite(MOTOR_RIGHT_DIR2, HIGH);
analogWrite(MOTOR_RIGHT_PWM, speed);

delay(duration);
stopRobot();
}

void turnLeft(int speed, int duration) {
digitalWrite(MOTOR_LEFT_DIR1, LOW);
digitalWrite(MOTOR_LEFT_DIR2, HIGH);
analogWrite(MOTOR_LEFT_PWM, speed);

digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
digitalWrite(MOTOR_RIGHT_DIR2, LOW);
analogWrite(MOTOR_RIGHT_PWM, speed);

delay(duration);
stopRobot();
}

void turnRight(int speed, int duration) {
digitalWrite(MOTOR_LEFT_DIR1, HIGH);
digitalWrite(MOTOR_LEFT_DIR2, LOW);
analogWrite(MOTOR_LEFT_PWM, speed);

digitalWrite(MOTOR_RIGHT_DIR1, LOW);
digitalWrite(MOTOR_RIGHT_DIR2, HIGH);
analogWrite(MOTOR_RIGHT_PWM, speed);

delay(duration);
stopRobot();
}

void stopRobot() {
analogWrite(MOTOR_LEFT_PWM, 0);
analogWrite(MOTOR_RIGHT_PWM, 0);
delay(100);
}

void squareSpiral() {
executeCommand("FORWARD"); delay(1000);
executeCommand("RIGHT");   delay(500);
executeCommand("FORWARD"); delay(1000);
executeCommand("RIGHT");   delay(500);
executeCommand("FORWARD"); delay(1000);
executeCommand("RIGHT");   delay(500);
executeCommand("FORWARD"); delay(1000);
executeCommand("STOP");
}

void showDirectionLED(String direction) {
// แสดงทิศทางบน LED Matrix
if (direction == "FORWARD") {
byte arrow[8][12] = {
{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
{0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
{0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
matrix.loadFrame(arrow);
}
// เพิ่ม patterns อื่น ๆ ตามต้องการ
}

// สร้าง HTML Dashboard + Remote Control
String createDashboardHTML() {
String html = "<!DOCTYPE html><html><head>";
html += "<meta charset='UTF-8'>";
html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
html += "<title>🤖 Robot Car Dashboard</title>";
html += "<style>";
html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 20px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; }";
html += ".container { max-width: 800px; margin: 0 auto; }";
html += ".header { text-align: center; margin-bottom: 30px; }";
html += ".header h1 { font-size: 2.5em; margin: 0; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); }";
html += ".dashboard { display: grid; grid-template-columns: 1fr 1fr; gap: 20px; margin-bottom: 30px; }";
html += ".card { background: rgba(255,255,255,0.1); backdrop-filter: blur(10px); border-radius: 15px; padding: 20px; box-shadow: 0 8px 32px rgba(0,0,0,0.1); }";
html += ".card h3 { margin-top: 0; font-size: 1.2em; }";
html += ".status-value { font-size: 2em; font-weight: bold; margin: 10px 0; }";
html += ".controls { text-align: center; }";
html += ".control-grid { display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 10px; max-width: 300px; margin: 20px auto; }";
html += ".btn { background: rgba(255,255,255,0.2); border: 2px solid rgba(255,255,255,0.3); color: white; padding: 15px; border-radius: 10px; font-size: 1.1em; cursor: pointer; transition: all 0.3s; text-decoration: none; display: block; }";
html += ".btn:hover { background: rgba(255,255,255,0.3); transform: translateY(-2px); }";
html += ".btn-large { grid-column: span 3; background: rgba(255,215,0,0.3); border-color: rgba(255,215,0,0.5); }";
html += ".empty { grid-column: 1; } .forward { grid-column: 2; } .turn-left { grid-column: 1; } .stop { grid-column: 2; } .turn-right { grid-column: 3; } .backward { grid-column: 2; }";
html += "@media (max-width: 600px) { .dashboard { grid-template-columns: 1fr; } }";
html += "auto-refresh { font-size: 0.9em; opacity: 0.7; }";
html += "</style>";
html += "<script>";
html += "function sendCommand(cmd) { fetch('/command?action=' + cmd).then(() => { setTimeout(() => { location.reload(); }, 500); }); }";
html += "setTimeout(() => { location.reload(); }, 5000);";  // Auto refresh ทุก 5 วินาที
html += "</script>";
html += "</head><body>";

html += "<div class='container'>";
html += "<div class='header'>";
html += "<h1>🤖 Robot Car Control Center</h1>";
html += "<p>Arduino UNO R4 WiFi Dashboard</p>";
html += "</div>";

// Dashboard Status
html += "<div class='dashboard'>";
html += "<div class='card'>";
html += "<h3>📊 Status</h3>";
html += "<div>Last Command: <span class='status-value'>" + lastCommand + "</span></div>";
html += "<div>Commands Sent: <span class='status-value'>" + String(totalCommands) + "</span></div>";
html += "</div>";

html += "<div class='card'>";
html += "<h3>⚡ System Info</h3>";
html += "<div>Uptime: <span class='status-value'>" + String(millis()/1000) + "s</span></div>";
html += "<div>Signal: <span class='status-value'>WiFi ✅</span></div>";
html += "</div>";
html += "</div>";

// Remote Control
html += "<div class='card controls'>";
html += "<h3>🎮 Remote Control</h3>";
html += "<div class='control-grid'>";
html += "<div class='empty'></div>";
html += "<a href='#' class='btn forward' onclick='sendCommand(\"FORWARD\")'>⬆️<br>FORWARD</a>";
html += "<div class='empty'></div>";

html += "<a href='#' class='btn turn-left' onclick='sendCommand(\"LEFT\")'>⬅️<br>LEFT</a>";
html += "<a href='#' class='btn stop' onclick='sendCommand(\"STOP\")'>⏹️<br>STOP</a>";
html += "<a href='#' class='btn turn-right' onclick='sendCommand(\"RIGHT\")'>➡️<br>RIGHT</a>";

html += "<div class='empty'></div>";
html += "<a href='#' class='btn backward' onclick='sendCommand(\"BACKWARD\")'>⬇️<br>BACKWARD</a>";
html += "<div class='empty'></div>";
html += "</div>";

html += "<a href='#' class='btn btn-large' onclick='sendCommand(\"SQUARE\")'>🔄 Execute Square Spiral</a>";
html += "</div>";

html += "<div class='auto-refresh'>🔄 Auto-refresh every 5 seconds</div>";
html += "</div>";
html += "</body></html>";

return html;
}

void handleWebServer() {
WiFiClient client = server.available();

if (client) {
String request = "";
while (client.connected() && client.available()) {
String line = client.readStringUntil('\n');
if (line.length() == 1 && line[0] == '\n') break;
request += line;
}

```
// ตรวจสอบคำสั่งที่ส่งมา
if (request.indexOf("GET /command?action=") >= 0) {
  String command = "";
  int actionStart = request.indexOf("action=") + 7;
  int actionEnd = request.indexOf(" ", actionStart);
  if (actionEnd == -1) actionEnd = request.indexOf("&", actionStart);
  if (actionEnd == -1) actionEnd = request.length();

  command = request.substring(actionStart, actionEnd);
  command.toUpperCase();

  // ดำเนินการตามคำสั่ง
  executeCommand(command);

  // ส่งคำตอบกลับ
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println("Connection: close");
  client.println();
  client.println("Command executed: " + command);
}
else {
  // แสดง Dashboard
  String html = createDashboardHTML();

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=UTF-8");
  client.println("Connection: close");
  client.println();
  client.println(html);
}

client.stop();

```

}
}

void loop() {
handleWebServer();
delay(50);  // ลด CPU usage
}

```

### 🎮 วิธีใช้งาน WiFi Dashboard:

1. **แก้ไข WiFi Settings:**
   ```cpp
   const char* ssid = "ชื่อWiFiบ้าน";
   const char* password = "รหัสผ่าน";

```

1. **อัปโหลดโค้ด** และรอจนเชื่อมต่อ WiFi สำเร็จ
2. **เปิด Serial Monitor** เพื่อดู IP Address
    
    ```
    WiFi connected!
    Dashboard URL: http://192.168.1.100
    
    ```
    
3. **เปิดมือถือหรือคอมพิวเตอร์** พิมพ์ IP Address ในเบราว์เซอร์
4. **ใช้งาน Remote Control:**
    - ⬆️ **FORWARD** - เดินหน้า 1 วินาที
    - ⬇️ **BACKWARD** - ถอยหลัง 1 วินาที
    - ⬅️ **LEFT** - เลี้ยวซ้าย 90°
    - ➡️ **RIGHT** - เลี้ยวขวา 90°
    - ⏹️ **STOP** - หยุด
    - 🔄 **Square Spiral** - ทำ mission อัตโนมัติ

### 📱 ฟีเจอร์ Dashboard:

- **📊 Real-time Status:** แสดงคำสั่งล่าสุดและจำนวนคำสั่งที่ส่ง
- **⚡ System Info:** เวลาการทำงานและสถานะการเชื่อมต่อ
- **🔄 Auto Refresh:** อัปเดตข้อมูลอัตโนมัติทุก 5 วินาที
- **📱 Responsive Design:** ใช้งานได้ทั้งมือถือและคอมพิวเตอร์
- **🎨 Modern UI:** ดีไซน์สวยงามด้วย Glass Effect

### 🔧 การปรับแต่งเพิ่มเติม:

```cpp
// เพิ่มการควบคุมความเร็ว
if (command.startsWith("SPEED")) {
  int newSpeed = command.substring(5).toInt();
  moveSpeed = constrain(newSpeed, 50, 255);
}

// เพิ่มการบันทึกข้อมูลมากขึ้น
void logCommand(String cmd) {
  Serial.print("[");
  Serial.print(millis()/1000.0, 1);
  Serial.print("s] Web Command: ");
  Serial.println(cmd);
}

```

---

*"การทดสอบซ้ำ ๆ คือกุญแจสำคัญของการเรียนรู้วิศวกรรม - และ WiFi Dashboard ทำให้การทดสอบสนุกขึ้นมาก!"*
