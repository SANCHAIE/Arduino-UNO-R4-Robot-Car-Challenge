# Arduino UNO R4 Robot Car with Ultrasonic Sensor

## 🚗 การเพิ่มฟีเจอร์ Ultrasonic Sensor

### 📋 รายการอุปกรณ์
- Arduino UNO R4 WiFi
- Motor Shield หรือ L298N Motor Driver
- DC Motors x2
- Ultrasonic Sensor HC-SR04
- Jumper Wires

### 🔌 การเชื่อมต่อ Ultrasonic Sensor

| HC-SR04 Pin | Arduino Pin |
|-------------|-------------|
| VCC         | 5V          |
| GND         | GND         |
| Trig        | Pin 6       |
| Echo        | Pin 7       |

### 🔧 ฟีเจอร์ใหม่ที่เพิ่ม

#### 1. การตรวจจับสิ่งกีดขวาง
- **อัตโนมัติหยุด**: เมื่อเจอสิ่งกีดขวางใน sequence หรือการเคลื่อนที่ต่อเนื่อง
- **ระยะกำหนดได้**: ตั้งค่าระยะหยุดได้ (default: 20cm)
- **เปิด/ปิดได้**: สามารถ toggle การตรวจจับได้

#### 2. คำสั่งใหม่
- `CHECK_DISTANCE`: ตรวจสอบระยะทางปัจจุบัน
- `TOGGLE_OBSTACLE`: เปิด/ปิดการตรวจจับสิ่งกีดขวาง

#### 3. การแสดงผลใน Web Interface
- **สถานะ Distance**: แสดงระยะทางปัจจุบัน
- **สถานะ Obstacle**: แสดงว่ามีสิ่งกีดขวางหรือไม่
- **การตั้งค่า**: ปรับระยะหยุด และ toggle detection

#### 4. ระบบป้องกัน
- **Sequence Safety**: หยุด sequence เมื่อเจอสิ่งกีดขวาง
- **Continuous Mode**: หยุดการเคลื่อนที่ต่อเนื่องอัตโนมัติ
- **Manual Override**: สามารถปิดการตรวจจับเพื่อควบคุมด้วยตนเอง

### 🎮 การใช้งาน

#### หน้า Main (/)
- ดูสถานะระยะทางและสิ่งกีดขวาง
- ตั้งค่าระยะหยุด (5-100 cm)
- เปิด/ปิดการตรวจจับ
- ตรวจสอบระยะทางด้วยปุ่ม "Check Distance"

#### หน้า Control (/control)
- ปุ่ม "📏 Distance": ตรวจสอบระยะทางทันที
- ปุ่ม "🔄 Toggle Sensor": เปิด/ปิดการตรวจจับ
- การเคลื่อนที่ปกติ (หยุดอัตโนมัติเมื่อเจอสิ่งกีดขวาง)

#### หน้า Sequence (/seq)
- เพิ่มคำสั่ง "📏 Check Distance" ใน sequence
- Sequence จะหยุดอัตโนมัติเมื่อเจอสิ่งกีดขวาง
- แสดงสถานะการทำงาน

### ⚠️ ข้อควรระวัง

1. **การติดตั้ง Sensor**: วาง HC-SR04 หันหน้าไปข้างหน้ารถ
2. **ระยะการทำงาน**: HC-SR04 ทำงานได้ดีที่ 2-400 cm
3. **ความแม่นยำ**: อาจมีความคลาดเคลื่อนที่พื้นผิวเรียบหรือมุมเอียง
4. **การปิดระบบ**: หากต้องการควบคุมด้วยตนเองเต็มที่ ให้ปิด Detection

### 🧪 การทดสอบ

1. อัปโหลด `UltrasonicTest.ino` เพื่อทดสอบ sensor ก่อน
2. ตรวจสอบว่าได้ค่าระยะทางที่ถูกต้อง
3. อัปโหลด `Robot Car WiFi Dashboard - Version 2-1.ino`
4. ทดสอบการทำงานผ่าน web interface

### 📝 Log Messages

- `🚨 Obstacle detected! Distance: X.Xcm - Stopped`
- `🚨 Cannot move forward! Obstacle at X.Xcm`
- `🚨 Sequence stopped! Obstacle detected at X.Xcm`
- `📏 Distance reading: X.Xcm`
- `🔄 Obstacle detection ENABLED/DISABLED`

---

**Happy Coding! 🤖✨**
