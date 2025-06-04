// โค้ด Robot Car WiFi Dashboard - Version 2.1 (4-Pin Motor Driver) - WiFi Optimized
// สำหรับ Motor Driver 4 pins ที่มี PWM built-in
#include <WiFiS3.h>

// WiFi credentials - ปรับสำหรับหลายเครือข่าย
struct WiFiCredentials {
  const char* ssid;
  const char* password;
};

// รายการ WiFi ที่ต้องการเชื่อมต่อ (เรียงตามลำดับความสำคัญ)
WiFiCredentials wifiNetworks[] = {
  {"Your_iPhone_Hotspot", "your_password"},      // iPhone Hotspot
  {"Your_Router_2.4GHz", "your_password"},       // Router 2.4GHz
  {"Your_Router_5GHz", "your_password"},         // Router 5GHz (ถ้ามี)
  {"Guest_Network", "guest_password"}            // Guest Network
};

const int numNetworks = sizeof(wifiNetworks) / sizeof(wifiNetworks[0]);

WiFiServer server(80);

// Motor pins สำหรับ 4-Pin Motor Driver (Built-in PWM)
#define MOTOR_LEFT_FORWARD    9   // PWM pin สำหรับมอเตอร์ซ้าย - เดินหน้า
#define MOTOR_LEFT_BACKWARD   10  // PWM pin สำหรับมอเตอร์ซ้าย - ถอยหลัง
#define MOTOR_RIGHT_FORWARD   5   // PWM pin สำหรับมอเตอร์ขวา - เดินหน้า
#define MOTOR_RIGHT_BACKWARD  6   // PWM pin สำหรับมอเตอร์ขวา - ถอยหลัง

// Ultrasonic sensor pins
#define ULTRASONIC_TRIG   7
#define ULTRASONIC_ECHO   8

// ตัวแปรการตั้งค่า
int moveSpeed = 200;
int turnSpeed = 150;
int moveDuration = 1000;
int turnDuration = 700;

// ตัวแปรปรับเทียบความเร็วมอเตอร์
int leftMotorOffset = 0;   // ค่าปรับสำหรับมอเตอร์ซ้าย (-50 ถึง +50)
int rightMotorOffset = 0;  // ค่าปรับสำหรับมอเตอร์ขวา (-50 ถึง +50)
bool calibrationMode = false;  // โหมดปรับเทียบ

// ตัวแปร Ultrasonic
int obstacleDistance = 20;  // ระยะหยุด (เซนติเมตร)
bool obstacleDetectionEnabled = true;  // เปิด/ปิดการตรวจจับสิ่งกีดขวาง

// ตัวแปร Sequence
struct Command {
  String action;
  int speed;
  int duration;
};

Command commandSequence[20];
int sequenceLength = 0;
bool isExecutingSequence = false;
int currentSequenceStep = 0;
unsigned long sequenceStepStartTime = 0;

// ตัวแปรสถานะ WiFi และระบบ
String systemLog = "";
unsigned long startTime = 0;
bool continuousForwardActive = false;
String lastActivity = "";
String connectedSSID = "";
bool isAPMode = false;
int connectionAttempts = 0;

// ฟังก์ชันสแกนหา WiFi networks ที่มีอยู่
void scanAndConnectWiFi() {
  Serial.println("🔍 Scanning for available WiFi networks...");
  
  // สแกนเครือข่าย WiFi
  int numSsid = WiFi.scanNetworks();
  
  if (numSsid == -1) {
    Serial.println("❌ WiFi scan failed");
    return;
  }
  
  Serial.println("📡 Found " + String(numSsid) + " networks:");
  
  // แสดงรายการเครือข่ายที่พบ
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    String ssid = WiFi.SSID(thisNet);
    int rssi = WiFi.RSSI(thisNet);
    String encryption = WiFi.encryptionType(thisNet) == WIFI_AUTH_OPEN ? "Open" : "Encrypted";
    
    Serial.println("  " + String(thisNet + 1) + ". " + ssid + 
                   " (Signal: " + String(rssi) + "dBm, " + encryption + ")");
  }
  
  Serial.println();
  
  // ลองเชื่อมต่อกับเครือข่ายที่กำหนดไว้
  for (int i = 0; i < numNetworks; i++) {
    String targetSSID = String(wifiNetworks[i].ssid);
    
    // ตรวจสอบว่าเครือข่ายนี้มีอยู่หรือไม่
    bool networkFound = false;
    int signalStrength = -100;
    
    for (int thisNet = 0; thisNet < numSsid; thisNet++) {
      if (WiFi.SSID(thisNet) == targetSSID) {
        networkFound = true;
        signalStrength = WiFi.RSSI(thisNet);
        break;
      }
    }
    
    if (networkFound) {
      Serial.println("🎯 Attempting to connect to: " + targetSSID + 
                     " (Signal: " + String(signalStrength) + "dBm)");
      
      if (connectToWiFi(wifiNetworks[i].ssid, wifiNetworks[i].password)) {
        connectedSSID = targetSSID;
        return; // เชื่อมต่อสำเร็จ
      }
    }
  }
  
  Serial.println("❌ Could not connect to any configured network");
}

// ฟังก์ชันเชื่อมต่อ WiFi แบบปรับปรุง
bool connectToWiFi(const char* ssid, const char* password) {
  Serial.println("🔌 Connecting to: " + String(ssid));
  
  // ตั้งค่า WiFi แบบละเอียด
  WiFi.disconnect();
  delay(1000);
  
  // กำหนดค่า WiFi config
  WiFi.config(IPAddress(0, 0, 0, 0)); // ใช้ DHCP
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  const int maxAttempts = 20; // ลดจาก 30 เป็น 20
  
  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
    delay(500);
    Serial.print(".");
    attempts++;
    
    // ตรวจสอบสถานะการเชื่อมต่อ
    if (attempts % 5 == 0) {
      wl_status_t status = WiFi.status();
      Serial.println();
      Serial.print("📊 Connection status: ");
      
      switch (status) {
        case WL_IDLE_STATUS:
          Serial.println("IDLE");
          break;
        case WL_NO_SSID_AVAIL:
          Serial.println("SSID NOT FOUND");
          return false; // หยุดทันทีถ้าไม่เจอ SSID
        case WL_CONNECT_FAILED:
          Serial.println("CONNECTION FAILED");
          return false; // หยุดทันทีถ้าเชื่อมต่อล้มเหลว
        case WL_CONNECTION_LOST:
          Serial.println("CONNECTION LOST");
          break;
        case WL_DISCONNECTED:
          Serial.println("DISCONNECTED");
          break;
        default:
          Serial.println("UNKNOWN (" + String(status) + ")");
      }
      
      Serial.print("🔄 Attempt " + String(attempts/5) + "/" + String(maxAttempts/5) + 
                   " for " + String(ssid) + "...");
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✅ WiFi connected successfully!");
    Serial.println("📱 SSID: " + String(ssid));
    Serial.println("🌐 IP address: " + WiFi.localIP().toString());
    Serial.println("📡 Signal strength: " + String(WiFi.RSSI()) + " dBm");
    Serial.println("🔒 Encryption: " + getEncryptionType());
    
    connectionAttempts = 0;
    return true;
  } else {
    Serial.println();
    Serial.println("❌ Failed to connect to " + String(ssid));
    connectionAttempts++;
    return false;
  }
}

// ฟังก์ชันตรวจสอบประเภทการเข้ารหัส
String getEncryptionType() {
  switch (WiFi.encryptionType()) {
    case WIFI_AUTH_OPEN: return "Open";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA PSK";
    case WIFI_AUTH_WPA2_PSK: return "WPA2 PSK";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2 PSK";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2 Enterprise";
    default: return "Unknown";
  }
}

// ฟังก์ชันสร้าง Access Point mode
void createAccessPoint() {
  Serial.println("🏠 Creating Access Point mode...");
  WiFi.disconnect();
  delay(1000);
  
  // สร้างชื่อ AP ที่ไม่ซ้ำ
  String apName = "RobotCar_4PWM_" + String(random(1000, 9999));
  String apPassword = "RobotCar123";
  
  if (WiFi.beginAP(apName.c_str(), apPassword.c_str())) {
    isAPMode = true;
    Serial.println("✅ Access Point created successfully!");
    Serial.println("📱 AP Name: " + apName);
    Serial.println("🔐 Password: " + apPassword);
    Serial.println("🌐 AP IP: " + WiFi.localIP().toString());
    Serial.println();
    Serial.println("📋 How to connect:");
    Serial.println("1. Connect your phone/computer to WiFi: " + apName);
    Serial.println("2. Use password: " + apPassword);
    Serial.println("3. Open browser and go to: " + WiFi.localIP().toString());
    
    connectedSSID = apName + " (AP Mode)";
  } else {
    Serial.println("❌ Failed to create Access Point!");
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000); // รอให้ Serial Monitor พร้อม
  
  Serial.println("🤖 Arduino UNO R4 WiFi Robot Car Starting...");
  Serial.println("🔧 4-Pin Motor Driver with Built-in PWM");
  Serial.println();
  
  // ตั้งค่า pins สำหรับ 4-Pin Motor Driver
  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACKWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACKWARD, OUTPUT);
  
  // ตั้งค่า Ultrasonic pins
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  
  // หยุดมอเตอร์
  stopMotors();
  
  startTime = millis();
  
  // เพิ่ม randomness สำหรับ AP name
  randomSeed(analogRead(0));
  
  // ลองเชื่อมต่อ WiFi แบบอัจฉริยะ
  Serial.println("🔍 Starting intelligent WiFi connection...");
  scanAndConnectWiFi();
  
  // ถ้าเชื่อมต่อไม่ได้ ให้สร้าง Access Point
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("🔄 WiFi connection failed, switching to Access Point mode...");
    createAccessPoint();
  }
  
  // เริ่มต้น web server
  server.begin();
  
  // Log การเริ่มต้นระบบ
  addLog("🚀 System started - " + (isAPMode ? "AP Mode" : "WiFi Mode"));
  addLog("🌐 Network: " + connectedSSID);
  addLog("📍 IP: " + WiFi.localIP().toString());
  addLog("🔧 Motor Driver: 4-Pin with Built-in PWM");
  addLog("📡 Web Dashboard ready");
  
  Serial.println();
  Serial.println("=== Robot Car WiFi Dashboard Ready ===");
  Serial.println("🌐 Access URL: http://" + WiFi.localIP().toString());
  Serial.println("==========================================");
}

// ฟังก์ชันตรวจสอบและเชื่อมต่อ WiFi ใหม่
void checkAndReconnectWiFi() {
  static unsigned long lastWiFiCheck = 0;
  static int reconnectAttempts = 0;
  
  if (millis() - lastWiFiCheck > 10000) { // ตรวจสอบทุก 10 วินาที
    lastWiFiCheck = millis();
    
    if (!isAPMode && WiFi.status() != WL_CONNECTED) {
      reconnectAttempts++;
      addLog("⚠️ WiFi disconnected! Attempt " + String(reconnectAttempts) + " to reconnect...");
      
      // ถ้าเชื่อมต่อไม่ได้หลายครั้ง ให้เปลี่ยนเป็น AP mode
      if (reconnectAttempts >= 3) {
        addLog("🔄 Multiple reconnection failures, switching to AP mode...");
        createAccessPoint();
        reconnectAttempts = 0;
        return;
      }
      
      // ลองเชื่อมต่อ WiFi ใหม่
      scanAndConnectWiFi();
      
      if (WiFi.status() == WL_CONNECTED) {
        addLog("✅ WiFi reconnected! IP: " + WiFi.localIP().toString());
        reconnectAttempts = 0;
      }
    } else if (WiFi.status() == WL_CONNECTED) {
      reconnectAttempts = 0; // รีเซ็ตการนับถ้าเชื่อมต่อปกติ
    }
  }
}

// ฟังก์ชันคำนวณความเร็วที่ปรับเทียบแล้ว
int getCalibratedLeftSpeed(int baseSpeed) {
  return constrain(baseSpeed + leftMotorOffset, 0, 255);
}

int getCalibratedRightSpeed(int baseSpeed) {
  return constrain(baseSpeed + rightMotorOffset, 0, 255);
}

// ฟังก์ชันเคลื่อนที่แบบต่อเนื่อง (สำหรับกดค้าง)
void startForward(int speed) {
  int leftSpeed = getCalibratedLeftSpeed(speed);
  int rightSpeed = getCalibratedRightSpeed(speed);
  
  analogWrite(MOTOR_LEFT_FORWARD, leftSpeed);
  analogWrite(MOTOR_LEFT_BACKWARD, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, rightSpeed);
  analogWrite(MOTOR_RIGHT_BACKWARD, 0);
}

void startBackward(int speed) {
  int leftSpeed = getCalibratedLeftSpeed(speed);
  int rightSpeed = getCalibratedRightSpeed(speed);
  
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACKWARD, leftSpeed);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACKWARD, rightSpeed);
}

void startTurnLeft(int speed) {
  int leftSpeed = getCalibratedLeftSpeed(speed);
  int rightSpeed = getCalibratedRightSpeed(speed);
  
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACKWARD, leftSpeed);
  analogWrite(MOTOR_RIGHT_FORWARD, rightSpeed);
  analogWrite(MOTOR_RIGHT_BACKWARD, 0);
}

void startTurnRight(int speed) {
  int leftSpeed = getCalibratedLeftSpeed(speed);
  int rightSpeed = getCalibratedRightSpeed(speed);
  
  analogWrite(MOTOR_LEFT_FORWARD, leftSpeed);
  analogWrite(MOTOR_LEFT_BACKWARD, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACKWARD, rightSpeed);
}

// ฟังก์ชันหยุดมอเตอร์
void stopMotors() {
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACKWARD, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACKWARD, 0);
}

// ฟังก์ชันเคลื่อนที่แบบกำหนดเวลา (สำหรับ Sequence)
void timedForward(int speed, int duration) {
  if (obstacleDetectionEnabled && getDistance() <= obstacleDistance) {
    addLog("⚠️ Obstacle detected! Stopping forward movement");
    return;
  }
  
  startForward(speed);
  delay(duration);
  stopMotors();
}

void timedBackward(int speed, int duration) {
  startBackward(speed);
  delay(duration);
  stopMotors();
}

void timedTurnLeft(int speed, int duration) {
  startTurnLeft(speed);
  delay(duration);
  stopMotors();
}

void timedTurnRight(int speed, int duration) {
  startTurnRight(speed);
  delay(duration);
  stopMotors();
}

// ฟังก์ชันอ่านระยะทางจาก Ultrasonic Sensor
int getDistance() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  
  long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 30000);
  
  if (duration == 0) {
    return 999;  // ไม่มีการตอบกลับ
  }
  
  int distance = duration * 0.034 / 2;
  return distance;
}

// ฟังก์ชันเพิ่ม Log
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
  
  // ส่งไป Serial Monitor ด้วย (สำหรับ debugging)
  Serial.println(logEntry);
}

// ฟังก์ชันแสดงสถานะ Sequence สำหรับ debugging
void debugSequenceStatus() {
  addLog("🔍 DEBUG: Sequence Status");
  addLog("  - isExecutingSequence: " + String(isExecutingSequence ? "true" : "false"));
  addLog("  - sequenceLength: " + String(sequenceLength));
  addLog("  - currentSequenceStep: " + String(currentSequenceStep));
  addLog("  - stepStartTime: " + String(sequenceStepStartTime));
  addLog("  - currentTime: " + String(millis()));
  if (sequenceLength > 0 && currentSequenceStep < sequenceLength) {
    addLog("  - currentCommand: " + commandSequence[currentSequenceStep].action);
    addLog("  - currentDuration: " + String(commandSequence[currentSequenceStep].duration));
    addLog("  - timeElapsed: " + String(millis() - sequenceStepStartTime));
  }
}

// ฟังก์ชันรันคำสั่ง
void executeCommand(String command) {
  if (command == "FORWARD") {
    addLog("⬆️ Moving forward");
    if (obstacleDetectionEnabled && getDistance() <= obstacleDistance) {
      addLog("⚠️ Obstacle detected! Cannot move forward");
      return;
    }
    startForward(moveSpeed);
    continuousForwardActive = true;
  }
  else if (command == "BACKWARD") {
    addLog("⬇️ Moving backward");
    startBackward(moveSpeed);
    continuousForwardActive = false;
  }
  else if (command == "LEFT") {
    addLog("⬅️ Turning left");
    startTurnLeft(turnSpeed);
    continuousForwardActive = false;
  }
  else if (command == "RIGHT") {
    addLog("➡️ Turning right");
    startTurnRight(turnSpeed);
    continuousForwardActive = false;
  }
  else if (command == "STOP") {
    addLog("⏹️ Motors stopped");
    stopMotors();
    continuousForwardActive = false;
  }
  else if (command == "TEST_TURN") {
    addLog("🧪 Starting 90° Test Turn");
    addLog("🔄 Test turn - Right 90° (" + String(turnSpeed) + " speed, " + String(turnDuration) + "ms)");
    timedTurnRight(turnSpeed, turnDuration);
    addLog("✅ 90° Test Turn completed successfully");
  }
  else if (command == "SQUARE") {
    addLog("🔄 Starting Square Pattern (4 sides)");
    for (int i = 0; i < 4; i++) {
      addLog("📐 Side " + String(i+1) + "/4 - Moving forward");
      timedForward(moveSpeed, moveDuration);
      delay(100); // Reduced delay for faster execution
      addLog("🔄 Turn " + String(i+1) + "/4 - Turning right");
      timedTurnRight(turnSpeed, turnDuration);
      delay(100); // Reduced delay for faster execution
    }
    addLog("✅ Square pattern completed");
  }
  else if (command == "CONTINUOUS_FORWARD") {
    addLog("🚀 Continuous forward mode activated");
    if (obstacleDetectionEnabled && getDistance() <= obstacleDistance) {
      addLog("⚠️ Obstacle detected! Cannot activate continuous forward");
      return;
    }
    startForward(moveSpeed);
    continuousForwardActive = true;
  }
  else if (command == "EXECUTE_SEQUENCE") {
    executeSequence();
  }
  else if (command == "STOP_SEQUENCE") {
    stopSequence();
  }
  else if (command == "CLEAR_SEQUENCE") {
    clearSequence();
  }
  else if (command == "DEBUG_SEQUENCE") {
    debugSequenceStatus();
  }
  else if (command == "CHECK_DISTANCE") {
    float distance = getDistance();
    addLog("📏 Distance measurement: " + String(distance, 1) + "cm");
  }
  else if (command == "TOGGLE_OBSTACLE") {
    obstacleDetectionEnabled = !obstacleDetectionEnabled;
    addLog("📡 Obstacle detection: " + String(obstacleDetectionEnabled ? "ENABLED" : "DISABLED"));
  }
  else if (command == "TOGGLE_OBSTACLE") {
    obstacleDetectionEnabled = !obstacleDetectionEnabled;
    String status = obstacleDetectionEnabled ? "ENABLED" : "DISABLED";
    addLog("🔄 Obstacle detection " + status);
  }
  // คำสั่งปรับเทียบมอเตอร์
  else if (command == "CAL_LEFT_PLUS") {
    leftMotorOffset = constrain(leftMotorOffset + 5, -50, 50);
    addLog("🔧 Left motor offset: " + String(leftMotorOffset) + " (increased)");
  }
  else if (command == "CAL_LEFT_MINUS") {
    leftMotorOffset = constrain(leftMotorOffset - 5, -50, 50);
    addLog("🔧 Left motor offset: " + String(leftMotorOffset) + " (decreased)");
  }
  else if (command == "CAL_RIGHT_PLUS") {
    rightMotorOffset = constrain(rightMotorOffset + 5, -50, 50);
    addLog("🔧 Right motor offset: " + String(rightMotorOffset) + " (increased)");
  }
  else if (command == "CAL_RIGHT_MINUS") {
    rightMotorOffset = constrain(rightMotorOffset - 5, -50, 50);
    addLog("🔧 Right motor offset: " + String(rightMotorOffset) + " (decreased)");
  }
  else if (command == "CAL_TEST") {
    addLog("🧪 Testing motor calibration - moving forward for 3 seconds");
    addLog("📊 Left offset: " + String(leftMotorOffset) + ", Right offset: " + String(rightMotorOffset));
    timedForward(moveSpeed, 3000);
    addLog("✅ Calibration test completed");
  }
  else if (command == "CAL_RESET") {
    leftMotorOffset = 0;
    rightMotorOffset = 0;
    addLog("🔄 Motor calibration reset to defaults");
  }
  else if (command == "CAL_MODE") {
    calibrationMode = !calibrationMode;
    String status = calibrationMode ? "ENABLED" : "DISABLED";
    addLog("🔧 Calibration mode " + status);
  }
}

// ฟังก์ชันเริ่มต้นคำสั่งใน Sequence
void startSequenceCommand(int stepIndex) {
  if (stepIndex >= sequenceLength) {
    isExecutingSequence = false;
    currentSequenceStep = 0;
    addLog("⚠️ Invalid sequence step - stopping execution");
    return;
  }
  
  String action = commandSequence[stepIndex].action;
  int speed = commandSequence[stepIndex].speed;
  int duration = commandSequence[stepIndex].duration;
  
  addLog("📍 Step " + String(stepIndex + 1) + "/" + String(sequenceLength) + ": " + action + 
         " (Speed: " + String(speed) + ", Duration: " + String(duration) + "ms)");
  
  // เริ่มคำสั่งโดยไม่ใช้ delay
  if (action == "FORWARD") {
    if (obstacleDetectionEnabled && getDistance() <= obstacleDistance) {
      addLog("🚨 Obstacle detected! Skipping FORWARD command");
      stopMotors();
    } else {
      startForward(speed);
    }
  }
  else if (action == "BACKWARD") {
    startBackward(speed);
  }
  else if (action == "LEFT") {
    startTurnLeft(speed);
  }
  else if (action == "RIGHT") {
    startTurnRight(speed);
  }
  else if (action == "STOP") {
    stopMotors();
  }
  else {
    addLog("❌ Unknown command: " + action);
    stopMotors();
  }
  
  sequenceStepStartTime = millis();
  lastActivity = action;
}

// ฟังก์ชันตรวจสอบและดำเนินการ Sequence
void processSequence() {
  if (!isExecutingSequence) return;
  
  // ตรวจสอบสิ่งกีดขวางสำหรับการเคลื่อนที่ไปข้างหน้า
  if (obstacleDetectionEnabled && 
      currentSequenceStep < sequenceLength && 
      commandSequence[currentSequenceStep].action == "FORWARD") {
    
    if (getDistance() <= obstacleDistance) {
      stopMotors();
      float distance = getDistance();
      addLog("🚨 Sequence stopped! Obstacle detected at " + String(distance, 1) + "cm");
      isExecutingSequence = false;
      currentSequenceStep = 0;
      return;
    }
  }
  
  // ตรวจสอบว่าขั้นตอนปัจจุบันเสร็จสิ้นหรือยัง
  if (millis() - sequenceStepStartTime >= commandSequence[currentSequenceStep].duration) {
    stopMotors();
    addLog("✅ Step " + String(currentSequenceStep + 1) + "/" + String(sequenceLength) + " completed");
    
    currentSequenceStep++;
    
    if (currentSequenceStep >= sequenceLength) {
      // จบ sequence
      isExecutingSequence = false;
      currentSequenceStep = 0;
      addLog("🎯 Sequence execution completed successfully!");
      addLog("📊 Total " + String(sequenceLength) + " commands executed");
    } else {
      // เริ่มขั้นตอนถัดไป (ไม่ใช้ delay เพื่อความเร็ว)
      startSequenceCommand(currentSequenceStep);
    }
  }
}

// ฟังก์ชันตรวจสอบสิ่งกีดขวางสำหรับ Continuous Forward
void checkObstacleForContinuousForward() {
  if (continuousForwardActive && obstacleDetectionEnabled) {
    int distance = getDistance();
    if (distance <= obstacleDistance) {
      stopMotors();
      continuousForwardActive = false;
      addLog("🛑 Emergency stop - Obstacle at " + String(distance) + "cm");
    }
  }
}

// สร้างหน้าหลัก - Settings + Status
String createMainHTML() {
  String html = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'><title>Robot Car Dashboard</title>";
  html += "<style>";
  html += "body{font-family:Arial;background:#1a1a2e;color:white;margin:5px;font-size:16px}";
  html += "h2{color:#4CAF50;text-align:center;margin:8px 0;font-size:20px}";
  html += ".nav{display:grid;grid-template-columns:1fr 1fr 1fr 1fr;gap:3px;margin:8px 0}";
  html += ".nav button{padding:8px 3px;background:#16213e;border:1px solid #4CAF50;color:white;border-radius:5px;font-size:12px}";
  html += ".nav button:hover{background:#4CAF50}";
  html += ".section{background:#16213e;padding:12px;margin:8px 0;border-radius:8px;border:1px solid #333}";
  html += "h3{margin:8px 0;font-size:16px;color:#4CAF50}";
  html += ".setting-row{display:flex;justify-content:space-between;align-items:center;margin:8px 0;flex-wrap:wrap}";
  html += ".setting-label{font-size:14px;margin-right:8px;min-width:80px}";
  html += "input{padding:6px;border:1px solid #555;background:#2a2a3e;color:white;border-radius:4px;width:60px;font-size:14px}";
  html += "button{padding:8px 12px;background:#4CAF50;border:none;color:white;border-radius:5px;cursor:pointer;margin:2px;font-size:14px}";
  html += "button:hover{background:#45a049}";
  html += ".status-grid{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin:8px 0}";
  html += ".status-item{background:#0f1419;padding:8px;border-radius:5px;font-size:13px}";
  html += ".status-label{color:#888;font-size:11px}";
  html += ".status-value{color:#4CAF50;font-weight:bold;font-size:14px}";
  html += "@media(max-width:600px){";
  html += ".setting-row{flex-direction:column;align-items:flex-start}";
  html += ".nav{grid-template-columns:1fr 1fr}";
  html += ".nav button{font-size:11px;padding:6px 2px}";
  html += ".status-grid{grid-template-columns:1fr}";
  html += "}";
  html += "</style></head><body>";
  
  html += "<h2>🚗 Robot Car Dashboard (4-Pin Motor)</h2>";
  
  // Navigation
  html += "<div class='nav'>";
  html += "<button onclick='location.href=\"/\"'>🏠 Main</button>";
  html += "<button onclick='location.href=\"/control\"'>🎮 Control</button>";
  html += "<button onclick='location.href=\"/seq\"'>📝 Sequence</button>";
  html += "<button onclick='location.href=\"/calibration\"'>🔧 Calibration</button>";
  html += "</div>";
  
  // Motor Settings
  html += "<div class='section'>";
  html += "<h3>⚙️ Motor Settings</h3>";
  html += "<div class='setting-row'>";
  html += "<span class='setting-label'>Move Speed:</span>";
  html += "<input type='number' id='ms' value='" + String(moveSpeed) + "' min='50' max='255'>";
  html += "<span class='setting-label'>Turn Speed:</span>";
  html += "<input type='number' id='ts' value='" + String(turnSpeed) + "' min='50' max='255'>";
  html += "</div>";
  html += "<div class='setting-row'>";
  html += "<span class='setting-label'>Move Time:</span>";
  html += "<input type='number' id='mt' value='" + String(moveDuration) + "' min='100' max='5000'>";
  html += "<span class='setting-label'>Turn Time:</span>";
  html += "<input type='number' id='tt' value='" + String(turnDuration) + "' min='100' max='3000'>";
  html += "</div>";
  html += "<button onclick='saveSettings()'>💾 Save Settings</button>";
  html += "</div>";
  
  // Ultrasonic Settings
  html += "<div class='section'>";
  html += "<h3>📡 Ultrasonic Settings</h3>";
  html += "<div class='setting-row'>";
  html += "<span class='setting-label'>Obstacle Distance:</span>";
  html += "<input type='number' id='od' value='" + String(obstacleDistance) + "' min='5' max='100'>";
  html += "<span class='setting-label'>cm</span>";
  html += "</div>";
  html += "<button onclick='saveSettings()'>💾 Save Settings</button>";
  html += "</div>";
  
  // Status
  html += "<div class='section'>";
  html += "<h3>📊 System Status</h3>";
  html += "<div class='status-grid'>";
  
  html += "<div class='status-item'>";
  html += "<div class='status-label'>WiFi Status</div>";
  html += "<div class='status-value'>Connected</div>";
  html += "</div>";
  
  html += "<div class='status-item'>";
  html += "<div class='status-label'>IP Address</div>";
  html += "<div class='status-value'>" + WiFi.localIP().toString() + "</div>";
  html += "</div>";
  
  html += "<div class='status-item'>";
  html += "<div class='status-label'>Motor Driver</div>";
  html += "<div class='status-value'>4-Pin PWM</div>";
  html += "</div>";
  
  html += "<div class='status-item'>";
  html += "<div class='status-label'>Last Activity</div>";
  html += "<div class='status-value'>" + lastActivity + "</div>";
  html += "</div>";
  
  html += "<div class='status-item'>";
  html += "<div class='status-label'>Distance Sensor</div>";
  html += "<div class='status-value'>" + String(getDistance()) + " cm</div>";
  html += "</div>";
  
  html += "<div class='status-item'>";
  html += "<div class='status-label'>Obstacle Detection</div>";
  html += "<div class='status-value'>" + String(obstacleDetectionEnabled ? "ON" : "OFF") + "</div>";
  html += "</div>";
  
  html += "</div>";
  html += "</div>";
  
  // Quick Actions
  html += "<div class='section'>";
  html += "<h3>🚀 Quick Actions</h3>";
  html += "<button onclick='cmd(\"TEST_TURN\")'>🧪 Test 90° Turn</button>";
  html += "<button onclick='cmd(\"SQUARE\")'>🔲 Square Pattern</button>";
  html += "<button onclick='cmd(\"TOGGLE_OBSTACLE\")'>🔄 Toggle Sensor</button>";
  html += "</div>";
  
  html += "<script>";
  html += "function cmd(c){fetch('/cmd?a='+c);}";
  html += "function saveSettings(){";
  html += "let ms=document.getElementById('ms').value;";
  html += "let ts=document.getElementById('ts').value;";
  html += "let mt=document.getElementById('mt').value;";
  html += "let tt=document.getElementById('tt').value;";
  html += "let od=document.getElementById('od').value;";
  html += "fetch('/set?ms='+ms+'&ts='+ts+'&mt='+mt+'&tt='+tt+'&od='+od).then(()=>alert('Settings saved!'));";
  html += "}";
  html += "</script>";
  
  html += "</body></html>";
  return html;
}

// สร้างหน้า Control - ปุ่มบังคับแบบจอย (เพิ่มกดค้าง)
String createControlHTML() {
  String html = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'><title>Robot Control</title>";
  html += "<style>";
  html += "body{font-family:Arial;background:#1a1a2e;color:white;margin:5px;text-align:center;font-size:16px}";
  html += "h2{color:#4CAF50;margin:8px 0;font-size:20px}";
  html += ".nav{display:grid;grid-template-columns:1fr 1fr 1fr 1fr;gap:3px;margin:8px 0}";
  html += ".nav button{padding:8px 3px;background:#16213e;border:1px solid #4CAF50;color:white;border-radius:5px;font-size:12px}";
  html += ".nav button:hover{background:#4CAF50}";
  html += ".joystick{display:grid;grid-template-columns:1fr 1fr 1fr;gap:8px;max-width:250px;margin:20px auto;padding:15px;background:#16213e;border-radius:15px;border:2px solid #4CAF50}";
  html += ".joy-btn{padding:15px;background:#4CAF50;border:none;color:white;border-radius:10px;font-size:24px;cursor:pointer;user-select:none;min-height:50px;display:flex;align-items:center;justify-content:center}";
  html += ".joy-btn:hover{background:#45a049}";
  html += ".joy-btn:active,.joy-btn.pressed{background:#2e7d32;transform:scale(0.95)}";
  html += ".stop-btn{background:#e74c3c !important;grid-column:2}";
  html += ".stop-btn:hover{background:#c0392b !important}";
  html += ".empty{background:none;border:none}";
  html += ".extra{margin:15px 0}";
  html += ".extra button{padding:12px 20px;margin:8px;background:#e74c3c;border:none;color:white;border-radius:8px;font-size:16px;min-width:120px}";
  html += ".extra button:hover{background:#c0392b}";
  html += ".instruction{font-size:14px;color:#aaa;margin:10px 0}";
  html += "@media(max-width:600px){";
  html += ".joystick{max-width:200px;gap:6px;padding:10px}";
  html += ".joy-btn{padding:12px;font-size:20px;min-height:40px}";
  html += ".nav{grid-template-columns:1fr 1fr}";
  html += ".nav button{font-size:11px;padding:6px 2px}";
  html += ".extra button{padding:10px 15px;font-size:14px;margin:5px;min-width:100px}";
  html += "}";
  html += "</style></head><body>";
  
  html += "<h2>🎮 Robot Control</h2>";
  
  // Navigation
  html += "<div class='nav'>";
  html += "<button onclick='location.href=\"/\"'>🏠 Main</button>";
  html += "<button onclick='location.href=\"/control\"'>🎮 Control</button>";
  html += "<button onclick='location.href=\"/seq\"'>📝 Sequence</button>";
  html += "<button onclick='location.href=\"/log\"'>📋 Log</button>";
  html += "</div>";
  
  html += "<div class='instruction'>💡 กดค้างปุ่มทิศทาง = เคลื่อนที่ต่อเนื่อง | คลิก STOP = หยุด</div>";
  
  // Joystick Layout
  html += "<div class='joystick'>";
  html += "<div class='empty'></div>";
  html += "<button class='joy-btn' id='btnUp'>⬆️</button>";
  html += "<div class='empty'></div>";
  html += "<button class='joy-btn' id='btnLeft'>⬅️</button>";
  html += "<button class='joy-btn stop-btn' onclick='cmdStop()'>⏹️</button>";
  html += "<button class='joy-btn' id='btnRight'>➡️</button>";
  html += "<div class='empty'></div>";
  html += "<button class='joy-btn' id='btnDown'>⬇️</button>";
  html += "<div class='empty'></div>";
  html += "</div>";
  
  // Extra Controls
  html += "<div class='extra'>";
  html += "<button onclick='cmd(\"CONTINUOUS_FORWARD\")'>🚀 Continuous Forward</button>";
  html += "<button onclick='cmd(\"TOGGLE_OBSTACLE\")'>🔄 Toggle Sensor</button>";
  html += "</div>";
  
  html += "<script>";
  html += "function cmd(c){fetch('/cmd?a='+c);}";
  html += "function cmdStop(){fetch('/cmd?a=STOP');}";
  
  // ฟังก์ชันกดค้าง
  html += "function setupHoldButton(btn,startCmd,stopCmd){";
  html += "let isPressed=false;";
  html += "function start(){if(!isPressed){isPressed=true;btn.classList.add('pressed');fetch('/cmd?a='+startCmd);}}";
  html += "function stop(){if(isPressed){isPressed=false;btn.classList.remove('pressed');fetch('/cmd?a='+stopCmd);}}";
  html += "btn.addEventListener('mousedown',start);";
  html += "btn.addEventListener('mouseup',stop);";
  html += "btn.addEventListener('mouseleave',stop);";
  html += "btn.addEventListener('touchstart',start);";
  html += "btn.addEventListener('touchend',stop);";
  html += "}";
  
  // Setup ปุ่มทั้งหมด
  html += "setupHoldButton(document.getElementById('btnUp'),'FORWARD','STOP');";
  html += "setupHoldButton(document.getElementById('btnDown'),'BACKWARD','STOP');";
  html += "setupHoldButton(document.getElementById('btnLeft'),'LEFT','STOP');";
  html += "setupHoldButton(document.getElementById('btnRight'),'RIGHT','STOP');";
  
  html += "</script>";
  html += "</body></html>";
  return html;
}

// สร้างหน้า Sequence
String createSequenceHTML() {
  String html = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'><title>Command Sequence</title>";
  html += "<style>";
  html += "body{font-family:Arial;background:#1a1a2e;color:white;margin:5px;font-size:16px}";
  html += "h2{color:#4CAF50;text-align:center;margin:8px 0;font-size:20px}";
  html += ".nav{display:grid;grid-template-columns:1fr 1fr 1fr 1fr;gap:3px;margin:8px 0}";
  html += ".nav button{padding:8px 3px;background:#16213e;border:1px solid #4CAF50;color:white;border-radius:5px;font-size:12px}";
  html += ".nav button:hover{background:#4CAF50}";
  html += ".section{background:#16213e;padding:12px;margin:8px 0;border-radius:8px;border:1px solid #333}";
  html += "h3{margin:8px 0;font-size:16px;color:#4CAF50}";
  html += ".add-command{display:grid;grid-template-columns:1fr 1fr 1fr 1fr;gap:8px;margin:10px 0;align-items:center}";
  html += "select,input{padding:6px;border:1px solid #555;background:#2a2a3e;color:white;border-radius:4px;font-size:14px}";
  html += "button{padding:8px 12px;background:#4CAF50;border:none;color:white;border-radius:5px;cursor:pointer;font-size:14px}";
  html += "button:hover{background:#45a049}";
  html += ".btn-danger{background:#e74c3c}";
  html += ".btn-danger:hover{background:#c0392b}";
  html += ".btn-warning{background:#f39c12}";
  html += ".btn-warning:hover{background:#e67e22}";
  html += ".sequence-list{max-height:200px;overflow-y:auto;background:#0f1419;padding:8px;border-radius:5px;margin:8px 0}";
  html += ".sequence-item{background:#1a1a2e;padding:6px;margin:4px 0;border-radius:4px;font-size:13px;display:flex;justify-content:space-between;align-items:center}";
  html += ".item-info{flex:1}";
  html += ".item-actions{display:flex;gap:4px}";
  html += ".item-actions button{padding:4px 8px;font-size:12px;min-width:auto}";
  html += "@media(max-width:600px){";
  html += ".add-command{grid-template-columns:1fr 1fr;gap:5px}";
  html += ".nav{grid-template-columns:1fr 1fr}";
  html += ".nav button{font-size:11px;padding:6px 2px}";
  html += ".sequence-item{flex-direction:column;align-items:flex-start}";
  html += ".item-actions{margin-top:5px;width:100%}";
  html += "}";
  html += "</style></head><body>";
  
  html += "<h2>📝 Command Sequence</h2>";
  
  // Navigation
  html += "<div class='nav'>";
  html += "<button onclick='location.href=\"/\"'>🏠 Main</button>";
  html += "<button onclick='location.href=\"/control\"'>🎮 Control</button>";
  html += "<button onclick='location.href=\"/seq\"'>📝 Sequence</button>";
  html += "<button onclick='location.href=\"/log\"'>📋 Log</button>";
  html += "</div>";
  
  // Add Command Section
  html += "<div class='section'>";
  html += "<h3>➕ Add Command</h3>";
  html += "<div class='add-command'>";
  html += "<select id='action'>";
  html += "<option value='FORWARD'>⬆️ Forward</option>";
  html += "<option value='BACKWARD'>⬇️ Backward</option>";
  html += "<option value='LEFT'>⬅️ Turn Left</option>";
  html += "<option value='RIGHT'>➡️ Turn Right</option>";
  html += "<option value='STOP'>⏹️ Stop</option>";
  html += "</select>";
  html += "<input type='number' id='speed' placeholder='Speed' value='150' min='50' max='255'>";
  html += "<input type='number' id='duration' placeholder='Duration (ms)' value='1000' min='100' max='5000'>";
  html += "<button onclick='addCommand()'>➕ Add</button>";
  html += "</div>";
  html += "</div>";
  
  // Current Sequence
  html += "<div class='section'>";
  html += "<h3>📋 Current Sequence (" + String(sequenceLength) + " commands)</h3>";
  html += "<div class='sequence-list' id='sequenceList'>";
  
  if (sequenceLength == 0) {
    html += "<div style='text-align:center;color:#888;font-style:italic'>No commands added yet</div>";
  } else {
    for (int i = 0; i < sequenceLength; i++) {
      html += "<div class='sequence-item'>";
      html += "<div class='item-info'>";
      html += String(i+1) + ". " + commandSequence[i].action + " (Speed: " + String(commandSequence[i].speed) + ", Duration: " + String(commandSequence[i].duration) + "ms)";
      html += "</div>";
      html += "<div class='item-actions'>";
      html += "<button class='btn-danger' onclick='removeCommand(" + String(i) + ")'>❌</button>";
      html += "</div>";
      html += "</div>";
    }
  }
  
  html += "</div>";
  
  // Control Buttons
  html += "<div style='margin:10px 0'>";
  html += "<button onclick='executeSequence()' " + String(sequenceLength == 0 ? "disabled" : "") + ">▶️ Execute Sequence</button>";
  html += "<button class='btn-warning' onclick='clearSequence()'>🗑️ Clear All</button>";
  html += "</div>";
  
  // Execution Status
  if (isExecutingSequence) {
    html += "<div style='background:#0f3460;padding:8px;border-radius:5px;margin:8px 0'>";
    html += "🏃‍♂️ Executing step " + String(currentSequenceStep + 1) + "/" + String(sequenceLength);
    html += "</div>";
  }
  
  html += "</div>";
  
  html += "<script>";
  html += "function addCommand(){";
  html += "let action=document.getElementById('action').value;";
  html += "let speed=document.getElementById('speed').value;";
  html += "let duration=document.getElementById('duration').value;";
  html += "fetch('/seq?add=1&action='+action+'&speed='+speed+'&duration='+duration).then(()=>location.reload());";
  html += "}";
  html += "function removeCommand(index){";
  html += "fetch('/seq?remove='+index).then(()=>location.reload());";
  html += "}";
  html += "function clearSequence(){";
  html += "if(confirm('Clear all commands?')){";
  html += "fetch('/seq?clear=1').then(()=>location.reload());";
  html += "}";
  html += "}";
  html += "function executeSequence(){";
  html += "fetch('/seq?execute=1').then(()=>location.reload());";
  html += "}";
  html += "</script>";
  
  html += "</body></html>";
  return html;
}

// สร้างหน้า Motor Calibration
String createCalibrationHTML() {
  String html = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'><title>Motor Calibration</title>";
  html += "<style>";
  html += "body{font-family:Arial;background:#1a1a2e;color:white;margin:5px;font-size:16px}";
  html += "h2{color:#4CAF50;text-align:center;margin:8px 0;font-size:20px}";
  html += ".nav{display:grid;grid-template-columns:1fr 1fr 1fr 1fr;gap:3px;margin:8px 0}";
  html += ".nav button{padding:8px 3px;background:#16213e;border:1px solid #4CAF50;color:white;border-radius:5px;font-size:12px}";
  html += ".nav button:hover{background:#4CAF50}";
  html += ".section{background:#16213e;padding:12px;margin:8px 0;border-radius:8px;border:1px solid #333}";
  html += "h3{margin:8px 0;font-size:16px;color:#4CAF50}";
  html += ".cal-row{display:flex;align-items:center;justify-content:space-between;margin:8px 0;padding:8px;background:#0f3460;border-radius:5px}";
  html += ".cal-label{font-size:14px;min-width:100px}";
  html += ".cal-value{font-size:18px;font-weight:bold;color:#4CAF50;min-width:40px;text-align:center}";
  html += ".cal-buttons{display:flex;gap:5px}";
  html += "button{padding:8px 12px;background:#4CAF50;border:none;color:white;border-radius:5px;cursor:pointer;font-size:14px}";
  html += "button:hover{background:#45a049}";
  html += ".btn-small{padding:6px 10px;font-size:12px}";
  html += ".btn-test{background:#ff9800;width:100%;margin:10px 0}";
  html += ".btn-test:hover{background:#f57c00}";
  html += ".btn-reset{background:#f44336}";
  html += ".btn-reset:hover{background:#d32f2f}";
  html += ".info{background:#0f1419;padding:10px;border-radius:5px;font-size:13px;margin:8px 0;border-left:3px solid #4CAF50}";
  html += "@media(max-width:600px){";
  html += ".cal-row{flex-direction:column;gap:8px;text-align:center}";
  html += ".cal-buttons{justify-content:center}";
  html += ".nav{grid-template-columns:1fr 1fr}";
  html += ".nav button{font-size:11px;padding:6px 2px}";
  html += "}";
  html += "</style></head><body>";
  
  html += "<h2>🔧 Motor Calibration</h2>";
  
  // Navigation
  html += "<div class='nav'>";
  html += "<button onclick='location.href=\"/\"'>🏠 Main</button>";
  html += "<button onclick='location.href=\"/control\"'>🎮 Control</button>";
  html += "<button onclick='location.href=\"/seq\"'>📝 Sequence</button>";
  html += "<button onclick='location.href=\"/calibration\"'>🔧 Calibration</button>";
  html += "</div>";
  
  // Current Settings
  html += "<div class='section'>";
  html += "<h3>📊 Current Calibration</h3>";
  html += "<div class='cal-row'>";
  html += "<span class='cal-label'>Left Motor Offset:</span>";
  html += "<span class='cal-value'>" + String(leftMotorOffset) + "</span>";
  html += "<div class='cal-buttons'>";
  html += "<button class='btn-small' onclick='sendCommand(\"CAL_LEFT_MINUS\")'>-5</button>";
  html += "<button class='btn-small' onclick='sendCommand(\"CAL_LEFT_PLUS\")'>+5</button>";
  html += "</div>";
  html += "</div>";
  
  html += "<div class='cal-row'>";
  html += "<span class='cal-label'>Right Motor Offset:</span>";
  html += "<span class='cal-value'>" + String(rightMotorOffset) + "</span>";
  html += "<div class='cal-buttons'>";
  html += "<button class='btn-small' onclick='sendCommand(\"CAL_RIGHT_MINUS\")'>-5</button>";
  html += "<button class='btn-small' onclick='sendCommand(\"CAL_RIGHT_PLUS\")'>+5</button>";
  html += "</div>";
  html += "</div>";
  html += "</div>";
  
  // Test & Reset
  html += "<div class='section'>";
  html += "<h3>🧪 Testing</h3>";
  html += "<button class='btn-test' onclick='sendCommand(\"CAL_TEST\")'>🧪 Test Current Settings (3 sec forward)</button>";
  html += "<button class='btn-reset' onclick='confirmReset()'>🔄 Reset to Default</button>";
  html += "</div>";
  
  // Information
  html += "<div class='section'>";
  html += "<h3>ℹ️ How to Calibrate</h3>";
  html += "<div class='info'>";
  html += "1. Place robot on a flat surface with space to move<br>";
  html += "2. Click 'Test Current Settings' to see robot movement<br>";
  html += "3. If robot turns left: increase left motor or decrease right motor<br>";
  html += "4. If robot turns right: increase right motor or decrease left motor<br>";
  html += "5. Repeat until robot moves straight<br>";
  html += "6. Valid offset range: -50 to +50";
  html += "</div>";
  html += "</div>";
  
  // Calculated Values
  html += "<div class='section'>";
  html += "<h3>⚙️ Actual Motor Speeds</h3>";
  html += "<div class='info'>";
  html += "At Move Speed " + String(moveSpeed) + ":<br>";
  html += "Left Motor: " + String(getCalibratedLeftSpeed(moveSpeed)) + "/255<br>";
  html += "Right Motor: " + String(getCalibratedRightSpeed(moveSpeed)) + "/255";
  html += "</div>";
  html += "</div>";
  
  html += "<script>";
  html += "function sendCommand(cmd){";
  html += "fetch('/?command='+cmd).then(()=>setTimeout(()=>location.reload(),500));";
  html += "}";
  html += "function confirmReset(){";
  html += "if(confirm('Reset calibration to default values?')){";
  html += "sendCommand('CAL_RESET');";
  html += "}";
  html += "}";
  html += "</script>";
  
  html += "</body></html>";
  return html;
}

// สร้างหน้า Log
String createLogHTML() {
  String html = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'><title>System Log</title>";
  html += "<style>";
  html += "body{font-family:Arial;background:#1a1a2e;color:white;margin:5px;font-size:16px}";
  html += "h2{color:#4CAF50;text-align:center;margin:8px 0;font-size:20px}";
  html += ".nav{display:grid;grid-template-columns:1fr 1fr 1fr 1fr;gap:3px;margin:8px 0}";
  html += ".nav button{padding:8px 3px;background:#16213e;border:1px solid #4CAF50;color:white;border-radius:5px;font-size:12px}";
  html += ".nav button:hover{background:#4CAF50}";
  html += ".log-container{background:#0f1419;padding:10px;border-radius:8px;margin:10px 0;max-height:400px;overflow-y:auto;font-family:monospace;font-size:13px;line-height:1.4;border:1px solid #333}";
  html += "button{padding:8px 12px;background:#4CAF50;border:none;color:white;border-radius:5px;cursor:pointer;margin:5px;font-size:14px}";
  html += "button:hover{background:#45a049}";
  html += ".auto-refresh{background:#ff9800}";
  html += ".auto-refresh:hover{background:#f57c00}";
  html += "@media(max-width:600px){";
  html += ".nav{grid-template-columns:1fr 1fr}";
  html += ".nav button{font-size:11px;padding:6px 2px}";
  html += ".log-container{font-size:12px;max-height:300px}";
  html += "}";
  html += "</style></head><body>";
  
  html += "<h2>📋 System Log</h2>";
  
  // Navigation
  html += "<div class='nav'>";
  html += "<button onclick='location.href=\"/\"'>🏠 Main</button>";
  html += "<button onclick='location.href=\"/control\"'>🎮 Control</button>";
  html += "<button onclick='location.href=\"/seq\"'>📝 Sequence</button>";
  html += "<button onclick='location.href=\"/log\"'>📋 Log</button>";
  html += "</div>";
  
  html += "<button onclick='location.reload()'>🔄 Refresh</button>";
  html += "<button class='auto-refresh' onclick='toggleAutoRefresh()' id='autoBtn'>⏱️ Auto Refresh: OFF</button>";
  
  html += "<div class='log-container'>";
  if (systemLog.length() == 0) {
    html += "<div style='text-align:center;color:#888;font-style:italic'>No log entries yet</div>";
  } else {
    String formattedLog = systemLog;
    formattedLog.replace("\n", "<br>");
    html += formattedLog;
  }
  html += "</div>";
  
  html += "<script>";
  html += "let autoRefresh = false;";
  html += "let refreshInterval;";
  html += "function toggleAutoRefresh() {";
  html += "autoRefresh = !autoRefresh;";
  html += "const btn = document.getElementById('autoBtn');";
  html += "if (autoRefresh) {";
  html += "btn.textContent = '⏱️ Auto Refresh: ON';";
  html += "refreshInterval = setInterval(() => location.reload(), 2000);";
  html += "} else {";
  html += "btn.textContent = '⏱️ Auto Refresh: OFF';";
  html += "clearInterval(refreshInterval);";
  html += "}";
  html += "}";
  html += "</script>";
  
  html += "</body></html>";
  return html;
}

// ฟังก์ชันช่วยสำหรับ Web Server
String extractParameter(String request, String paramName) {
  String param = paramName + "=";
  int startIndex = request.indexOf(param);
  if (startIndex == -1) return "";
  
  startIndex += param.length();
  int endIndex = request.indexOf("&", startIndex);
  if (endIndex == -1) endIndex = request.indexOf(" ", startIndex);
  if (endIndex == -1) return "";
  
  return request.substring(startIndex, endIndex);
}

void sendResponse(WiFiClient client, String message) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain; charset=UTF-8");
  client.println("Connection: close");
  client.println("Cache-Control: no-cache");
  client.println();
  client.println(message);
  client.flush(); // Force send immediately
}

// ฟังก์ชัน Web Server หลัก
void handleWebServer() {
  WiFiClient client = server.available();
  if (!client) return;
  
  // Set shorter timeout for faster response
  client.setTimeout(1000);
  
  String request = "";
  unsigned long startTime = millis();
  
  // Read request with timeout protection
  while (client.connected() && client.available() && (millis() - startTime < 2000)) {
    String line = client.readStringUntil('\n');
    if (line.length() == 1 && line[0] == '\r') break;
    request += line + "\n";
    
    // Break early if we have enough data
    if (request.length() > 1500) break;
  }
  
  // ดึง path จาก request
  String path = "";
  int firstSpace = request.indexOf(' ');
  int secondSpace = request.indexOf(' ', firstSpace + 1);
  if (firstSpace != -1 && secondSpace != -1) {
    path = request.substring(firstSpace + 1, secondSpace);
  }
  
  // ตัดส่วน query string ออก
  int queryIndex = path.indexOf('?');
  String basePath = (queryIndex != -1) ? path.substring(0, queryIndex) : path;
  
  // Route handling
  if (basePath == "/" && path.indexOf("?command=") != -1) {
    // Handle calibration commands from main page
    String command = extractParameter(request, "command");
    executeCommand(command);
    sendResponse(client, "Command executed: " + command);
  }
  else if (basePath == "/") {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html; charset=UTF-8");
    client.println("Connection: close");
    client.println();
    client.println(createMainHTML());
  }
  else if (path == "/control") {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html; charset=UTF-8");
    client.println("Connection: close");
    client.println();
    client.println(createControlHTML());
  }
  else if (path.startsWith("/seq")) {
    if (path.indexOf("?add=1") != -1) {
      // Add command to sequence
      String action = extractParameter(request, "action");
      String speedStr = extractParameter(request, "speed");
      String durationStr = extractParameter(request, "duration");
      
      if (sequenceLength < 20 && action.length() > 0) {
        commandSequence[sequenceLength].action = action;
        commandSequence[sequenceLength].speed = speedStr.toInt();
        commandSequence[sequenceLength].duration = durationStr.toInt();
        sequenceLength++;
        addLog("➕ Added to sequence: " + action + " (Speed: " + speedStr + ", Duration: " + durationStr + "ms)");
      }
      
      sendResponse(client, "Command added");
    }
    else if (path.indexOf("?remove=") != -1) {
      // Remove command from sequence
      String indexStr = extractParameter(request, "remove");
      int index = indexStr.toInt();
      
      if (index >= 0 && index < sequenceLength) {
        addLog("➖ Removed from sequence: " + commandSequence[index].action);
        for (int i = index; i < sequenceLength - 1; i++) {
          commandSequence[i] = commandSequence[i + 1];
        }
        sequenceLength--;
      }
      
      sendResponse(client, "Command removed");
    }
    else if (path.indexOf("?clear=1") != -1) {
      // Clear all commands
      sequenceLength = 0;
      addLog("🗑️ Sequence cleared");
      sendResponse(client, "Sequence cleared");
    }
    else if (path.indexOf("?execute=1") != -1) {
      // Execute sequence
      if (sequenceLength > 0) {
        if (isExecutingSequence) {
          addLog("⚠️ Sequence already running - stopping current execution first");
          stopSequence();
          delay(200);
        }
        executeSequence(); // Use the improved function
      } else {
        addLog("❌ Cannot execute: No commands in sequence");
      }
      sendResponse(client, "Sequence execution initiated");
    }
    else if (path.indexOf("?stop=1") != -1) {
      // Stop sequence execution
      stopSequence();
      sendResponse(client, "Sequence stopped");
    }
    else {
      // Show sequence page
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html; charset=UTF-8");
      client.println("Connection: close");
      client.println();
      client.println(createSequenceHTML());
    }
  }
  else if (path == "/calibration") {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html; charset=UTF-8");
    client.println("Connection: close");
    client.println();
    client.println(createCalibrationHTML());
  }
  else if (path == "/log") {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html; charset=UTF-8");
    client.println("Connection: close");
    client.println();
    client.println(createLogHTML());
  }
  else if (path.startsWith("/cmd?")) {
    // Handle commands
    String action = extractParameter(request, "a");
    executeCommand(action);
    sendResponse(client, "Command executed: " + action);
  }
  else if (path.startsWith("/set?")) {
    // Handle settings
    String msParam = extractParameter(request, "ms");
    String tsParam = extractParameter(request, "ts");
    String mtParam = extractParameter(request, "mt");
    String ttParam = extractParameter(request, "tt");
    String odParam = extractParameter(request, "od");
    
    if (msParam.length() > 0) {
      moveSpeed = constrain(msParam.toInt(), 50, 255);
      addLog("⚙️ Move speed set to " + String(moveSpeed));
    }
    if (tsParam.length() > 0) {
      turnSpeed = constrain(tsParam.toInt(), 50, 255);
      addLog("⚙️ Turn speed set to " + String(turnSpeed));
    }
    if (mtParam.length() > 0) {
      moveDuration = constrain(mtParam.toInt(), 100, 5000);
      addLog("⚙️ Move duration set to " + String(moveDuration) + "ms");
    }
    if (ttParam.length() > 0) {
      turnDuration = constrain(ttParam.toInt(), 100, 3000);
      addLog("⚙️ Turn duration set to " + String(turnDuration) + "ms");
    }
    if (odParam.length() > 0) {
      obstacleDistance = constrain(odParam.toInt(), 5, 100);
      addLog("📡 Obstacle distance set to " + String(obstacleDistance) + "cm");
    }
    
    sendResponse(client, "Settings updated");
  }
  else {
    // 404 Not Found
    client.println("HTTP/1.1 404 Not Found");
    client.println("Content-Type: text/html; charset=UTF-8");
    client.println("Connection: close");
    client.println();
    client.println("<html><body><h1>404 - Page Not Found</h1></body></html>");
  }
  
  client.stop();
}

void loop() {
  // ตรวจสอบการเชื่อมต่อ WiFi และเชื่อมต่อใหม่ถ้าจำเป็น
  checkAndReconnectWiFi();
  
  // Handle web server with improved performance
  handleWebServer();
  
  // Process sequence execution (non-blocking)
  processSequence();
  
  // Check obstacle detection for continuous movement
  checkObstacleForContinuousForward();
  
  // Send heartbeat log every 30 seconds
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 30000) {
    lastHeartbeat = millis();
    
    String connectionInfo = "";
    if (isAPMode) {
      connectionInfo = "AP Mode - " + connectedSSID;
    } else if (WiFi.status() == WL_CONNECTED) {
      int rssi = WiFi.RSSI();
      connectionInfo = "WiFi: " + connectedSSID + " (" + String(rssi) + "dBm)";
    } else {
      connectionInfo = "Disconnected";
    }
    
    addLog("💗 Heartbeat - " + connectionInfo + ", Uptime: " + String((millis()-startTime)/1000) + "s");
  }
  
  // Reduced delay for better responsiveness
  delay(5);
}

// ฟังก์ชันเริ่มต้น Sequence Execution
void executeSequence() {
  if (sequenceLength == 0) {
    addLog("❌ Cannot execute: Sequence is empty");
    return;
  }
  
  if (isExecutingSequence) {
    addLog("⚠️ Sequence is already running! Stopping current sequence first.");
    isExecutingSequence = false;
    stopMotors();
    delay(500);
  }
  
  addLog("🚀 Starting sequence execution (" + String(sequenceLength) + " commands)");
  addLog("📊 Sequence commands overview:");
  
  // แสดงรายการคำสั่งทั้งหมด
  for (int i = 0; i < sequenceLength; i++) {
    String emoji = "🎮";
    if (commandSequence[i].action == "FORWARD") emoji = "⬆️";
    else if (commandSequence[i].action == "BACKWARD") emoji = "⬇️";
    else if (commandSequence[i].action == "LEFT") emoji = "⬅️";
    else if (commandSequence[i].action == "RIGHT") emoji = "➡️";
    else if (commandSequence[i].action == "STOP") emoji = "⏹️";
    
    addLog("  " + String(i+1) + ". " + emoji + " " + commandSequence[i].action + 
           " (" + String(commandSequence[i].duration) + "ms)");
  }
  
  isExecutingSequence = true;
  currentSequenceStep = 0;
  sequenceStepStartTime = millis();
  
  // เริ่มคำสั่งแรก
  startSequenceCommand(currentSequenceStep);
}

// ฟังก์ชันหยุด Sequence
void stopSequence() {
  if (isExecutingSequence) {
    isExecutingSequence = false;
    stopMotors();
    addLog("⏹️ Sequence execution stopped by user at step " + String(currentSequenceStep + 1) + "/" + String(sequenceLength));
    currentSequenceStep = 0;
  }
}

// ฟังก์ชันล้างคำสั่งทั้งหมด
void clearSequence() {
  if (isExecutingSequence) {
    stopSequence();
  }
  sequenceLength = 0;
  currentSequenceStep = 0;
  addLog("🗑️ All sequence commands cleared");
}
