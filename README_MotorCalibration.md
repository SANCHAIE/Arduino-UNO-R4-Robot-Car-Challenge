# ระบบปรับเทียบความเร็วมอเตอร์ (Motor Speed Calibration)

## 🎯 **วัตถุประสงค์**
ระบบปรับเทียบความเร็วมอเตอร์สำหรับ Arduino UNO R4 Robot Car เพื่อให้รถเคลื่อนที่ตรงเมื่อมอเตอร์ทั้งสองข้างได้รับค่าความเร็วเท่ากัน

## ⚠️ **ปัญหาที่พบบ่อย**
- รถเคลื่อนที่เบี้ยวไปทางซ้ายหรือขวาแม้ตั้งค่าความเร็วเท่ากัน
- มอเตอร์แต่ละตัวมีประสิทธิภาพไม่เท่ากัน
- การสึกหรอของล้อและมอเตอร์ไม่เท่ากัน
- แรงเสียดทานของล้อแต่ละข้างต่างกัน

## 🛠️ **วิธีแก้ปัญหา**
ระบบนี้ใช้ **Motor Offset** เพื่อปรับค่าความเร็วของมอเตอร์แต่ละข้างให้เหมาะสม

---

## 📋 **รายการไฟล์**

### 1. **Robot Car WiFi Dashboard - Version 2-1.ino** (ไฟล์หลัก)
- รถหุ่นยนต์ควบคุมผ่าน WiFi พร้อมระบบปรับเทียบ
- เว็บอินเตอร์เฟซสำหรับปรับเทียบแบบเรียลไทม์
- บันทึกผลการปรับเทียบอัตโนมัติ

### 2. **MotorCalibration.ino** (เครื่องมือแยกต่างหาก)
- เครื่องมือปรับเทียบผ่าน Serial Monitor
- ทดสอบค่าต่างๆ และหาค่าที่เหมาะสม
- แสดงโค้ดที่ใช้ในโปรแกรมหลัก

---

## 🚀 **วิธีใช้งาน**

### **วิธีที่ 1: ผ่านเว็บอินเตอร์เฟซ (แนะนำ)**

#### 1. **เตรียมระบบ**
```arduino
// อัปโหลด Robot Car WiFi Dashboard - Version 2-1.ino
// ตั้งค่า WiFi credentials:
const char* ssid = "ชื่อ_WiFi_ของคุณ";
const char* password = "รหัสผ่าน_WiFi";
```

#### 2. **เชื่อมต่อและเข้าสู่ระบบ**
- เปิด Serial Monitor เพื่อดู IP Address
- ใช้เบราว์เซอร์เข้า `http://[IP_ADDRESS]`
- คลิก **"🔧 Calibration"** ในเมนู

#### 3. **ขั้นตอนการปรับเทียบ**

##### **3.1 เตรียมพื้นที่ทดสอบ**
- วางรถบนพื้นเรียบและแข็ง
- ให้มีพื้นที่เคลื่อนที่อย่างน้อย 2 เมตร
- ทำเครื่องหมายจุดเริ่มต้น

##### **3.2 ทดสอบเบื้องต้น**
```
คลิก "🧪 Test Current Settings"
→ รถจะเคลื่อนที่ไปข้างหน้า 3 วินาที
→ สังเกตทิศทางการเคลื่อนที่
```

##### **3.3 การปรับค่า**

**หากรถเบี้ยวไปทางซ้าย:**
- คลิก **"+5"** ที่ Left Motor Offset หรือ
- คลิก **"-5"** ที่ Right Motor Offset

**หากรถเบี้ยวไปทางขวา:**
- คลิก **"+5"** ที่ Right Motor Offset หรือ
- คลิก **"-5"** ที่ Left Motor Offset

##### **3.4 ทดสอบซ้ำ**
- คลิก "🧪 Test Current Settings" หลังการปรับค่า
- ปรับค่าต่อไปจนกว่ารถจะเคลื่อนที่ตรง

##### **3.5 บันทึกค่า**
- ค่าจะถูกบันทึกอัตโนมัติ
- ดูผลลัพธ์ใน **"📋 Log"**

---

### **วิธีที่ 2: ผ่าน Serial Monitor**

#### 1. **อัปโหลด MotorCalibration.ino**
```arduino
// อัปโหลดไฟล์ MotorCalibration.ino
// เปิด Serial Monitor (9600 baud)
```

#### 2. **คำสั่งที่ใช้**
```
't'  - ทดสอบการตั้งค่าปัจจุบัน
'l+' - เพิ่มความเร็วมอเตอร์ซ้าย (+5)
'l-' - ลดความเร็วมอเตอร์ซ้าย (-5)
'r+' - เพิ่มความเร็วมอเตอร์ขวา (+5)
'r-' - ลดความเร็วมอเตอร์ขวา (-5)
'b+' - เพิ่ม Base Speed (+10)
'b-' - ลด Base Speed (-10)
's'  - แสดงการตั้งค่าปัจจุบัน
'c'  - รันการปรับเทียบแบบครบวงจร
'h'  - แสดงความช่วยเหลือ
```

#### 3. **ตัวอย่างการใช้งาน**
```
=== Motor Speed Calibration ===
Commands: 't', 'l+', 'l-', 'r+', 'r-', 's', 'h'

> t
Testing Current Settings...
Left motor: 150, Right motor: 150
Test complete. Robot turned left.

> r+
Right motor speed increased

> t
Testing Current Settings...
Left motor: 150, Right motor: 155
Test complete. Robot moved straighter.

> s
--- Current Settings ---
Base Speed: 150
Left Offset: 0
Right Offset: 5
Actual Left Speed: 150
Actual Right Speed: 155
```

---

## ⚙️ **ข้อมูลทางเทคนิค**

### **ตัวแปรสำคัญ**
```cpp
int leftMotorOffset = 0;   // ค่าปรับมอเตอร์ซ้าย (-50 ถึง +50)
int rightMotorOffset = 0;  // ค่าปรับมอเตอร์ขวา (-50 ถึง +50)
bool calibrationMode = false;  // โหมดปรับเทียบ
```

### **ฟังก์ชันคำนวณความเร็ว**
```cpp
int getCalibratedLeftSpeed(int baseSpeed) {
  return constrain(baseSpeed + leftMotorOffset, 0, 255);
}

int getCalibratedRightSpeed(int baseSpeed) {
  return constrain(baseSpeed + rightMotorOffset, 0, 255);
}
```

### **การใช้งานในโค้ด**
```cpp
// แทนที่การใช้ baseSpeed โดยตรง
analogWrite(MOTOR_LEFT_PWM, getCalibratedLeftSpeed(moveSpeed));
analogWrite(MOTOR_RIGHT_PWM, getCalibratedRightSpeed(moveSpeed));
```

---

## 🎮 **คำสั่งใหม่ในเว็บอินเตอร์เฟซ**

| คำสั่ง | ฟังก์ชัน |
|--------|---------|
| `CAL_LEFT_PLUS` | เพิ่มค่า offset มอเตอร์ซ้าย (+5) |
| `CAL_LEFT_MINUS` | ลดค่า offset มอเตอร์ซ้าย (-5) |
| `CAL_RIGHT_PLUS` | เพิ่มค่า offset มอเตอร์ขวา (+5) |
| `CAL_RIGHT_MINUS` | ลดค่า offset มอเตอร์ขวา (-5) |
| `CAL_TEST` | ทดสอบการตั้งค่าปัจจุบัน (3 วินาที) |
| `CAL_RESET` | รีเซ็ตค่าเป็นค่าเริ่มต้น |
| `CAL_MODE` | เปิด/ปิดโหมดปรับเทียบ |

---

## 📱 **หน้าเว็บ Calibration**

### **ส่วนประกอบ**
1. **📊 Current Calibration** - แสดงค่า offset ปัจจุบัน
2. **🧪 Testing** - ปุ่มทดสอบและรีเซ็ต
3. **ℹ️ How to Calibrate** - คำแนะนำการใช้งาน
4. **⚙️ Actual Motor Speeds** - ความเร็วจริงที่คำนวณแล้ว

### **ฟีเจอร์พิเศษ**
- **Real-time Updates** - อัปเดตค่าทันทีหลังการปรับเทียบ
- **Mobile Responsive** - ใช้งานได้บนมือถือ
- **Visual Feedback** - แสดงผลการปรับเทียบแบบกราฟิก
- **Safety Limits** - จำกัดค่า offset ไม่ให้เกินขอบเขต

---

## 🔧 **การแก้ปัญหา**

### **ปัญหา: รถยังเบี้ยวหลังปรับเทียบ**
**แก้ไข:**
- ตรวจสอบการติดตั้งล้อให้แน่น
- ล้างสิ่งสกปรกออกจากล้อ
- ปรับค่า offset เป็นขั้นเล็กลง (±1-2)
- ทดสอบบนพื้นผิวที่เรียบกว่า

### **ปัญหา: มอเตอร์ทำงานไม่เสถียร**
**แก้ไข:**
- ตรวจสอบแรงดันไฟ
- ตรวจสอบการเชื่อมต่อสายไฟ
- ลดค่า Base Speed ลง

### **ปัญหา: ไม่สามารถเข้าหน้า Calibration ได้**
**แก้ไข:**
- ตรวจสอบการเชื่อมต่อ WiFi
- รีสตาร์ท Arduino และลองใหม่
- ตรวจสอบ Serial Monitor เพื่อดู IP Address

---

## 📈 **ตัวอย่างผลลัพธ์**

### **ก่อนปรับเทียบ**
```
Left Motor: 200, Right Motor: 200
ผลลัพธ์: รถเบี้ยวไปทางซ้าย 15°
```

### **หลังปรับเทียบ**
```
Left Motor: 200, Right Motor: 210 (offset +10)
ผลลัพธ์: รถเคลื่อนที่ตรง เบี้ยว < 2°
```

---

## 💡 **เทคนิคขั้นสูง**

### **การปรับเทียบแบบละเอียด**
```cpp
// ใช้ค่า offset เป็นทศนิยม (ต้องแก้โค้ด)
float leftMotorOffset = 2.5;
int getCalibratedLeftSpeed(int baseSpeed) {
  return constrain(round(baseSpeed + leftMotorOffset), 0, 255);
}
```

### **การบันทึกค่าลง EEPROM**
```cpp
#include <EEPROM.h>

void saveCalibration() {
  EEPROM.write(0, leftMotorOffset + 50);  // +50 เพื่อให้เป็นค่าบวก
  EEPROM.write(1, rightMotorOffset + 50);
}

void loadCalibration() {
  leftMotorOffset = EEPROM.read(0) - 50;
  rightMotorOffset = EEPROM.read(1) - 50;
}
```

### **การใช้ Encoder เพื่อความแม่นยำ**
```cpp
// นับรอบล้อเพื่อเปรียบเทียบระยะทาง
volatile int leftEncoder = 0, rightEncoder = 0;

void setup() {
  attachInterrupt(digitalPinToInterrupt(2), leftEncoderISR, RISING);
  attachInterrupt(digitalPinToInterrupt(3), rightEncoderISR, RISING);
}
```

---

## 📚 **อ้างอิง**

### **เอกสารที่เกี่ยวข้อง**
- `README.md` - คู่มือการใช้งานทั่วไป
- `README_Ultrasonic.md` - ระบบตรวจจับสิ่งกีดขวาง

### **ไฟล์โค้ดที่เกี่ยวข้อง**
- `Robot Car WiFi Dashboard - Version 2-1.ino` - โปรแกรมหลัก
- `MotorCalibration.ino` - เครื่องมือปรับเทียบ
- `RUN1st.ino` - โปรแกรมทดสอบพื้นฐาน

---

## ✅ **Checklist การปรับเทียบ**

- [ ] ตรวจสอบการเชื่อมต่อมอเตอร์
- [ ] ทดสอบการทำงานพื้นฐานด้วย `RUN1st.ino`
- [ ] อัปโหลดโปรแกรมหลัก
- [ ] เชื่อมต่อ WiFi สำเร็จ
- [ ] เข้าหน้าเว็บได้
- [ ] เข้าหน้า Calibration ได้
- [ ] ทดสอบการเคลื่อนที่เบื้องต้น
- [ ] ปรับค่า offset ทีละน้อย
- [ ] ทดสอบการเคลื่อนที่หลังปรับ
- [ ] บันทึกค่าสุดท้าย
- [ ] ทดสอบ sequence และ control

---

**🎯 เป้าหมาย:** รถเคลื่อนที่ตรงภายในระยะเบี้ยว < 5° ใน 3 วินาที  
**⏱️ เวลาโดยเฉลี่ย:** 5-15 นาทีสำหรับการปรับเทียบ  
**🔄 ความถี่:** ปรับเทียบใหม่เมื่อเปลี่ยนแบตเตอรี่หรือล้อ

---

**Happy Calibrating! 🤖✨**
