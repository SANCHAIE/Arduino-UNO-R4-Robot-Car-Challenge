// โค้ด Robot Car WiFi Dashboard - Version 2.1 (Clean Motor Functions)
#include <WiFiS3.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_PASSWORD";

WiFiServer server(80);

// Motor pins
#define MOTOR_LEFT_PWM    9
#define MOTOR_RIGHT_PWM   10
#define MOTOR_LEFT_DIR1   2
#define MOTOR_LEFT_DIR2   3
#define MOTOR_RIGHT_DIR1  4
#define MOTOR_RIGHT_DIR2  5

// Ultrasonic sensor pins
#define ULTRASONIC_TRIG   6
#define ULTRASONIC_ECHO   7

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

// ตัวแปรสถานะ
String systemLog = "";
int totalCommands = 0;
unsigned long startTime = 0;
bool continuousForwardActive = false;  // ตรวจสอบว่าเคลื่อนที่ไปข้างหน้าแบบต่อเนื่องหรือไม่

// Function declarations
String createMainHTML();
String createControlHTML();
String createSequenceHTML();
String createCalibrationHTML();
String createLogHTML();
void handleWebServer();
void handleSequenceExecution();
void handleContinuousMovement();
void startSequenceCommand(int stepIndex);
String extractParameter(String request, String paramName);
void sendResponse(WiFiClient client, String message);

void setup() {
  Serial.begin(115200);
  
  // ตั้งค่า pins
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  pinMode(MOTOR_LEFT_DIR1, OUTPUT);
  pinMode(MOTOR_LEFT_DIR2, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR1, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR2, OUTPUT);
  
  // ตั้งค่า Ultrasonic pins
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  
  // หยุดมอเตอร์
  analogWrite(MOTOR_LEFT_PWM, 0);
  analogWrite(MOTOR_RIGHT_PWM, 0);
  
  startTime = millis();
  
  // เชื่อมต่อ WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.begin();
  addLog("🚀 Robot Car Started - IP: " + WiFi.localIP().toString());
}

void loop() {
  handleWebServer();
  handleSequenceExecution();  // เพิ่มการจัดการ sequence แบบ non-blocking
  handleContinuousMovement();  // ตรวจสอบการเคลื่อนที่แบบต่อเนื่อง
  delay(10);
}

// ========== MOTOR FUNCTIONS - NO DUPLICATES ==========

// ฟังก์ชัน Ultrasonic Sensor
float getDistance() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  
  long duration = pulseIn(ULTRASONIC_ECHO, HIGH);
  float distance = duration * 0.034 / 2;  // แปลงเป็นเซนติเมตร
  
  return distance;
}

bool isObstacleDetected() {
  if (!obstacleDetectionEnabled) return false;
  
  float distance = getDistance();
  return distance > 0 && distance < obstacleDistance;
}

// ฟังก์ชันหยุดมอเตอร์
void stopMotors() {
  analogWrite(MOTOR_LEFT_PWM, 0);
  analogWrite(MOTOR_RIGHT_PWM, 0);
  digitalWrite(MOTOR_LEFT_DIR1, LOW);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);
  digitalWrite(MOTOR_RIGHT_DIR1, LOW);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);
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
  
  digitalWrite(MOTOR_LEFT_DIR1, HIGH);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);
  digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);
  analogWrite(MOTOR_LEFT_PWM, leftSpeed);
  analogWrite(MOTOR_RIGHT_PWM, rightSpeed);
}

void startBackward(int speed) {
  int leftSpeed = getCalibratedLeftSpeed(speed);
  int rightSpeed = getCalibratedRightSpeed(speed);
  
  digitalWrite(MOTOR_LEFT_DIR1, LOW);
  digitalWrite(MOTOR_LEFT_DIR2, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR1, LOW);
  digitalWrite(MOTOR_RIGHT_DIR2, HIGH);
  analogWrite(MOTOR_LEFT_PWM, leftSpeed);
  analogWrite(MOTOR_RIGHT_PWM, rightSpeed);
}

void startTurnLeft(int speed) {
  int leftSpeed = getCalibratedLeftSpeed(speed);
  int rightSpeed = getCalibratedRightSpeed(speed);
  
  digitalWrite(MOTOR_LEFT_DIR1, LOW);
  digitalWrite(MOTOR_LEFT_DIR2, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);
  analogWrite(MOTOR_LEFT_PWM, leftSpeed);
  analogWrite(MOTOR_RIGHT_PWM, rightSpeed);
}

void startTurnRight(int speed) {
  int leftSpeed = getCalibratedLeftSpeed(speed);
  int rightSpeed = getCalibratedRightSpeed(speed);
  
  digitalWrite(MOTOR_LEFT_DIR1, HIGH);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);
  digitalWrite(MOTOR_RIGHT_DIR1, LOW);
  digitalWrite(MOTOR_RIGHT_DIR2, HIGH);
  analogWrite(MOTOR_LEFT_PWM, leftSpeed);
  analogWrite(MOTOR_RIGHT_PWM, rightSpeed);
}

// ฟังก์ชันเคลื่อนที่แบบมีระยะเวลา (สำหรับ sequence และ test)
void timedForward(int speed, int duration) {
  unsigned long startTime = millis();
  startForward(speed);
  
  while (millis() - startTime < duration) {
    if (obstacleDetectionEnabled && isObstacleDetected()) {
      stopMotors();
      float distance = getDistance();
      addLog("🚨 Obstacle detected! Distance: " + String(distance, 1) + "cm - Stopped");
      return;
    }
    delay(50);  // ตรวจสอบทุก 50ms
  }
  
  stopMotors();
}

void timedBackward(int speed, int duration) {
  unsigned long startTime = millis();
  startBackward(speed);
  
  while (millis() - startTime < duration) {
    delay(50);  // Backward ไม่ต้องตรวจสอบสิ่งกีดขวางด้านหน้า
  }
  
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

// ========== LOG FUNCTIONS ==========

// ฟังก์ชันเพิ่ม log
void addLog(String message) {
  String timestamp = String((millis() - startTime) / 1000);
  systemLog += "[" + timestamp + "s] " + message + "\n";
  
  // เก็บ 25 บรรทัดล่าสุด
  int lineCount = 0;
  for (int i = systemLog.length() - 1; i >= 0; i--) {
    if (systemLog[i] == '\n') {
      lineCount++;
      if (lineCount >= 25) {
        systemLog = systemLog.substring(i + 1);
        break;
      }
    }
  }
  
  Serial.println(message);
}

// ========== COMMAND EXECUTION ==========

// ฟังก์ชันรันคำสั่ง
void executeCommand(String command) {
  totalCommands++;
  addLog("🎮 Executing: " + command);
  
  // คำสั่งแบบกดค้าง (เริ่มเคลื่อนที่)
  if (command == "START_FORWARD") {
    if (obstacleDetectionEnabled && isObstacleDetected()) {
      float distance = getDistance();
      addLog("🚨 Cannot move forward! Obstacle at " + String(distance, 1) + "cm");
    } else {
      addLog("⬆️ Start moving forward (continuous)");
      startForward(moveSpeed);
      continuousForwardActive = true;
    }
  }
  else if (command == "START_BACKWARD") {
    addLog("⬇️ Start moving backward (continuous)");
    startBackward(moveSpeed);
    continuousForwardActive = false;
  }
  else if (command == "START_LEFT") {
    addLog("⬅️ Start turning left (continuous)");
    startTurnLeft(turnSpeed);
    continuousForwardActive = false;
  }
  else if (command == "START_RIGHT") {
    addLog("➡️ Start turning right (continuous)");
    startTurnRight(turnSpeed);
    continuousForwardActive = false;
  }
  // คำสั่งแบบเดิม (มีระยะเวลา)
  else if (command == "FORWARD") {
    addLog("⬆️ Moving Forward (" + String(moveSpeed) + " speed, " + String(moveDuration) + "ms)");
    timedForward(moveSpeed, moveDuration);
    addLog("✅ Forward completed");
  }
  else if (command == "BACKWARD") {
    addLog("⬇️ Moving Backward (" + String(moveSpeed) + " speed, " + String(moveDuration) + "ms)");
    timedBackward(moveSpeed, moveDuration);
    addLog("✅ Backward completed");
  }
  else if (command == "LEFT") {
    addLog("⬅️ Turning Left (" + String(turnSpeed) + " speed, " + String(turnDuration) + "ms)");
    timedTurnLeft(turnSpeed, turnDuration);
    addLog("✅ Left turn completed");
  }
  else if (command == "RIGHT") {
    addLog("➡️ Turning Right (" + String(turnSpeed) + " speed, " + String(turnDuration) + "ms)");
    timedTurnRight(turnSpeed, turnDuration);
    addLog("✅ Right turn completed");
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
      addLog("🔄 Side " + String(i+1) + "/4 - Turning right 90°");
      delay(500);
      timedTurnRight(turnSpeed, turnDuration);
      delay(500);
      addLog("✅ Side " + String(i+1) + "/4 completed");
    }
    addLog("🎯 Square Pattern completed successfully!");
  }
  else if (command == "CHECK_DISTANCE") {
    float distance = getDistance();
    addLog("📏 Distance reading: " + String(distance, 1) + "cm");
    if (distance > 0 && distance < obstacleDistance) {
      addLog("⚠️ Obstacle detected within " + String(obstacleDistance) + "cm!");
    } else {
      addLog("✅ Path clear");
    }
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

// ========== SEQUENCE FUNCTIONS ==========

// ฟังก์ชัน Sequence
void addToSequence(String action, int speed, int duration) {
  if (sequenceLength < 20) {
    commandSequence[sequenceLength].action = action;
    commandSequence[sequenceLength].speed = speed;
    commandSequence[sequenceLength].duration = duration;
    sequenceLength++;
    addLog("📝 Added to sequence [" + String(sequenceLength) + "]: " + action + " (" + String(duration) + "ms)");
  } else {
    addLog("❌ Sequence full! Maximum 20 commands");
  }
}

void executeSequence() {
  if (sequenceLength == 0) {
    addLog("❌ Cannot execute: Sequence is empty");
    return;
  }
  
  if (isExecutingSequence) {
    addLog("⚠️ Sequence is already running!");
    return;
  }
  
  addLog("🚀 Starting sequence execution (" + String(sequenceLength) + " commands)");
  isExecutingSequence = true;
  currentSequenceStep = 0;
  sequenceStepStartTime = millis();
  
  // เริ่มคำสั่งแรก
  startSequenceCommand(currentSequenceStep);
}

void startSequenceCommand(int stepIndex) {
  if (stepIndex >= sequenceLength) return;
  
  addLog("📍 Step " + String(stepIndex + 1) + "/" + String(sequenceLength) + ": " + commandSequence[stepIndex].action);
  
  // เริ่มคำสั่งโดยไม่ใช้ delay
  if (commandSequence[stepIndex].action == "FORWARD") {
    startForward(commandSequence[stepIndex].speed);
  }
  else if (commandSequence[stepIndex].action == "BACKWARD") {
    startBackward(commandSequence[stepIndex].speed);
  }
  else if (commandSequence[stepIndex].action == "LEFT") {
    startTurnLeft(commandSequence[stepIndex].speed);
  }
  else if (commandSequence[stepIndex].action == "RIGHT") {
    startTurnRight(commandSequence[stepIndex].speed);
  }
  else if (commandSequence[stepIndex].action == "STOP") {
    stopMotors();
  }
  else if (commandSequence[stepIndex].action == "CHECK_DISTANCE") {
    float distance = getDistance();
    addLog("📏 Distance: " + String(distance, 1) + "cm");
    stopMotors();  // ไม่ต้องเคลื่อนที่
  }
}

void handleSequenceExecution() {
  if (!isExecutingSequence) return;
  
  // ตรวจสอบสิ่งกีดขวางสำหรับการเคลื่อนที่ไปข้างหน้า
  if (obstacleDetectionEnabled && 
      currentSequenceStep < sequenceLength && 
      commandSequence[currentSequenceStep].action == "FORWARD") {
    
    if (isObstacleDetected()) {
      stopMotors();
      float distance = getDistance();
      addLog("🚨 Sequence stopped! Obstacle detected at " + String(distance, 1) + "cm");
      isExecutingSequence = false;
      return;
    }
  }
  
  // ตรวจสอบว่าคำสั่งปัจจุบันเสร็จสิ้นแล้วหรือไม่
  if (millis() - sequenceStepStartTime >= commandSequence[currentSequenceStep].duration) {
    // หยุดมอเตอร์และเพิ่ม log
    stopMotors();
    addLog("✅ Step " + String(currentSequenceStep + 1) + " completed");
    
    // ไปคำสั่งถัดไป
    currentSequenceStep++;
    
    if (currentSequenceStep < sequenceLength) {
      // รอสักครู่ก่อนคำสั่งถัดไป
      delay(200);
      sequenceStepStartTime = millis();
      startSequenceCommand(currentSequenceStep);
    } else {
      // sequence เสร็จสิ้น
      isExecutingSequence = false;
      addLog("🎯 Sequence execution completed successfully!");
      addLog("📊 Total " + String(sequenceLength) + " commands executed");
    }
  }
}

void handleContinuousMovement() {
  // ตรวจสอบสิ่งกีดขวางขณะเคลื่อนที่ไปข้างหน้าแบบต่อเนื่อง
  if (continuousForwardActive && obstacleDetectionEnabled) {
    if (isObstacleDetected()) {
      stopMotors();
      float distance = getDistance();
      addLog("🚨 Continuous movement stopped! Obstacle at " + String(distance, 1) + "cm");
      continuousForwardActive = false;
    }
  }
}

void clearSequence() {
  int oldLength = sequenceLength;
  sequenceLength = 0;
  addLog("🧹 Sequence cleared (" + String(oldLength) + " commands removed)");
}

// ========== HTML GENERATION ==========

// สร้างหน้าหลัก - Settings + Status
String createMainHTML() {
  String html = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'><title>Robot Main</title>";
  html += "<style>";
  html += "body{font-family:Arial;background:#1a1a2e;color:white;margin:5px;font-size:16px}";
  html += "h2{color:#4CAF50;text-align:center;margin:8px 0;font-size:20px}";
  html += ".nav{display:grid;grid-template-columns:1fr 1fr 1fr 1fr;gap:3px;margin:8px 0}";
  html += ".nav button{padding:10px 5px;background:#16213e;border:1px solid #4CAF50;color:white;border-radius:5px;font-size:14px}";
  html += ".nav button:hover{background:#4CAF50}";
  html += ".section{background:#16213e;padding:10px;margin:8px 0;border-radius:8px;border:1px solid #333}";
  html += "h3{margin:5px 0;font-size:16px;color:#4CAF50}";
  html += "input{width:50px;padding:6px;margin:3px;background:#0f3460;border:1px solid #4CAF50;color:white;border-radius:3px;font-size:14px}";
  html += "button{padding:8px 12px;background:#4CAF50;border:none;color:white;border-radius:5px;cursor:pointer;margin:5px;font-size:14px}";
  html += "button:hover{background:#45a049}";
  html += ".status{display:grid;grid-template-columns:1fr 1fr;gap:8px;font-size:14px}";
  html += ".setting-row{margin:8px 0;display:flex;flex-wrap:wrap;align-items:center;gap:5px}";
  html += ".setting-label{min-width:80px;font-size:14px}";
  html += "@media(max-width:600px){";
  html += ".status{grid-template-columns:1fr}";
  html += ".setting-row{flex-direction:column;align-items:flex-start}";
  html += "input{width:80px}";
  html += "}";
  html += "</style></head><body>";
  
  html += "<h2>🤖 Robot Control</h2>";
  
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
  html += "<input type='number' id='mt' value='" + String(moveDuration) + "' min='100' max='5000'><span>ms</span>";
  html += "<span class='setting-label'>Turn Time:</span>";
  html += "<input type='number' id='tt' value='" + String(turnDuration) + "' min='100' max='3000'><span>ms</span>";
  html += "</div>";
  html += "<button onclick='saveSettings()'>💾 Save Settings</button>";
  html += "</div>";
  
  // Ultrasonic Settings
  html += "<div class='section'>";
  html += "<h3>📡 Ultrasonic Settings</h3>";
  html += "<div class='setting-row'>";
  html += "<span class='setting-label'>Stop Distance:</span>";
  html += "<input type='number' id='od' value='" + String(obstacleDistance) + "' min='5' max='100'><span>cm</span>";
  html += "<span class='setting-label'>Detection:</span>";
  html += "<span style='color:" + String(obstacleDetectionEnabled ? "#4CAF50" : "#e74c3c") + "'>";
  html += obstacleDetectionEnabled ? "✅ ENABLED" : "❌ DISABLED";
  html += "</span>";
  html += "</div>";
  html += "<div class='setting-row'>";
  html += "<button onclick='checkDistance()'>📏 Check Distance</button>";
  html += "<button onclick='toggleObstacle()'>🔄 Toggle Detection</button>";
  html += "</div>";
  html += "<button onclick='saveUltrasonicSettings()'>💾 Save Distance</button>";
  html += "</div>";
  
  // Status
  html += "<div class='section'>";
  html += "<h3>📊 System Status</h3>";
  html += "<div class='status'>";
  html += "<div><strong>Commands:</strong> " + String(totalCommands) + "</div>";
  html += "<div><strong>Uptime:</strong> " + String((millis()-startTime)/1000) + "s</div>";
  html += "<div><strong>Sequence:</strong> " + String(sequenceLength) + " commands</div>";
  html += "<div><strong>IP:</strong> " + WiFi.localIP().toString() + "</div>";
  html += "<div><strong>Distance:</strong> " + String(getDistance(), 1) + "cm</div>";
  html += "<div><strong>Obstacle:</strong> ";
  if (obstacleDetectionEnabled) {
    if (isObstacleDetected()) {
      html += "<span style='color:#e74c3c'>🚨 DETECTED</span>";
    } else {
      html += "<span style='color:#4CAF50'>✅ CLEAR</span>";
    }
  } else {
    html += "<span style='color:#f39c12'>⚠️ DISABLED</span>";
  }
  html += "</div>";
  html += "</div>";
  html += "</div>";
  
  html += "<script>";
  html += "function saveSettings(){";
  html += "fetch('/set?ms='+document.getElementById('ms').value+'&ts='+document.getElementById('ts').value+'&mt='+document.getElementById('mt').value+'&tt='+document.getElementById('tt').value);";
  html += "setTimeout(()=>location.reload(),300);";
  html += "}";
  html += "function saveUltrasonicSettings(){";
  html += "fetch('/set?od='+document.getElementById('od').value);";
  html += "setTimeout(()=>location.reload(),300);";
  html += "}";
  html += "function checkDistance(){";
  html += "fetch('/cmd?a=CHECK_DISTANCE');";
  html += "setTimeout(()=>location.reload(),1000);";
  html += "}";
  html += "function toggleObstacle(){";
  html += "fetch('/cmd?a=TOGGLE_OBSTACLE');";
  html += "setTimeout(()=>location.reload(),500);";
  html += "}";
  html += "</script>";
  
  html += "</body></html>";
  return html;
}

// สร้างหน้า Control - ปุ่มบังคับแบบจอย (เพิ่มกดค้าง)
String createControlHTML() {
  String html = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'><title>Robot Control</title>";
  html += "<style>";
  html += "body{font-family:Arial;background:#1a1a2e;color:white;margin:5px;text-align:center;font-size:16px;user-select:none;-webkit-user-select:none}";
  html += "h2{color:#4CAF50;margin:8px 0;font-size:20px}";
  html += ".nav{display:grid;grid-template-columns:1fr 1fr 1fr 1fr;gap:3px;margin:8px 0}";
  html += ".nav button{padding:8px 3px;background:#16213e;border:1px solid #4CAF50;color:white;border-radius:5px;font-size:12px}";
  html += ".nav button:hover{background:#4CAF50}";
  html += ".joystick{display:grid;grid-template-columns:1fr 1fr 1fr;grid-template-rows:1fr 1fr 1fr;gap:8px;max-width:240px;margin:15px auto;padding:15px}";
  html += ".joy-btn{padding:18px;background:#16213e;border:2px solid #4CAF50;color:white;border-radius:12px;font-size:24px;cursor:pointer;min-height:50px;display:flex;align-items:center;justify-content:center;touch-action:manipulation}";
  html += ".joy-btn:hover{background:#4CAF50;transform:scale(1.05)}";
  html += ".joy-btn:active,.joy-btn.pressed{background:#45a049;transform:scale(0.95);border-color:#fff}";
  html += ".stop-btn{background:#e74c3c !important;border-color:#c0392b !important}";
  html += ".stop-btn:hover{background:#c0392b !important}";
  html += ".empty{background:none;border:none}";
  html += ".extra{margin:15px 0}";
  html += ".extra button{padding:12px 20px;margin:8px;background:#e74c3c;border:none;color:white;border-radius:8px;font-size:16px;min-width:120px}";
  html += ".extra button:hover{background:#c0392b}";
  html += ".instruction{font-size:14px;color:#aaa;margin:10px 0}";
  html += "@media(max-width:600px){";
  html += ".joystick{max-width:200px;gap:6px;padding:10px}";
  html += ".joy-btn{padding:12px;font-size:20px;min-height:40px}";
  html += ".extra button{padding:10px 15px;font-size:14px;min-width:100px;margin:5px}";
  html += ".nav button{font-size:11px;padding:6px 2px}";
  html += ".instruction{font-size:12px}";
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
  
  // Extra Commands
  html += "<div class='extra'>";
  html += "<button onclick='cmd(\"TEST_TURN\")'>🧪 Test Turn</button>";
  html += "<button onclick='cmd(\"SQUARE\")'>🔄 Square</button>";
  html += "<button onclick='cmd(\"CHECK_DISTANCE\")'>📏 Distance</button>";
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
  html += "btn.addEventListener('touchstart',function(e){e.preventDefault();start();});";
  html += "btn.addEventListener('touchend',function(e){e.preventDefault();stop();});";
  html += "btn.addEventListener('touchcancel',function(e){e.preventDefault();stop();});";
  html += "btn.addEventListener('contextmenu',function(e){e.preventDefault();});";
  html += "btn.addEventListener('dragstart',function(e){e.preventDefault();});";
  html += "}";
  
  // ตั้งค่าปุ่มทั้งหมด
  html += "document.addEventListener('DOMContentLoaded',function(){";
  html += "setupHoldButton(document.getElementById('btnUp'),'START_FORWARD','STOP');";
  html += "setupHoldButton(document.getElementById('btnDown'),'START_BACKWARD','STOP');";
  html += "setupHoldButton(document.getElementById('btnLeft'),'START_LEFT','STOP');";
  html += "setupHoldButton(document.getElementById('btnRight'),'START_RIGHT','STOP');";
  html += "});";
  html += "</script>";
  
  html += "</body></html>";
  return html;
}

// สร้างหน้า Sequence
String createSequenceHTML() {
  String html = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'><title>Sequence</title>";
  html += "<style>";
  html += "body{font-family:Arial;background:#1a1a2e;color:white;margin:5px;font-size:16px}";
  html += "h2{color:#4CAF50;text-align:center;margin:8px 0;font-size:20px}";
  html += ".nav{display:grid;grid-template-columns:1fr 1fr 1fr 1fr;gap:3px;margin:8px 0}";
  html += ".nav button{padding:8px 3px;background:#16213e;border:1px solid #4CAF50;color:white;border-radius:5px;font-size:12px}";
  html += ".nav button:hover{background:#4CAF50}";
  html += ".section{background:#16213e;padding:10px;margin:8px 0;border-radius:8px;border:1px solid #333}";
  html += "h3{margin:5px 0;font-size:16px;color:#4CAF50}";
  html += "select,input{padding:8px;margin:3px;background:#0f3460;border:1px solid #4CAF50;color:white;border-radius:3px;font-size:14px}";
  html += "select{width:120px} input{width:80px}";
  html += "button{padding:8px 12px;background:#4CAF50;border:none;color:white;border-radius:5px;cursor:pointer;margin:3px;font-size:14px}";
  html += "button:hover{background:#45a049}";
  html += ".seq-list{background:#0f3460;padding:8px;border-radius:5px;max-height:200px;overflow-y:auto;font-family:monospace;font-size:14px}";
  html += ".seq-item{padding:6px;margin:2px 0;background:#1a1a2e;border-radius:3px;display:flex;justify-content:space-between;align-items:center}";
  html += ".run-btn{background:#e74c3c} .run-btn:hover{background:#c0392b}";
  html += ".clear-btn{background:#f39c12} .clear-btn:hover{background:#e67e22}";
  html += ".add-section{display:flex;flex-wrap:wrap;gap:5px;align-items:center}";
  html += ".btn-group{display:flex;gap:5px;flex-wrap:wrap}";
  html += "@media(max-width:600px){";
  html += ".add-section{flex-direction:column;align-items:stretch}";
  html += "select,input{width:100%;margin:2px 0}";
  html += ".btn-group{justify-content:center}";
  html += ".nav button{font-size:11px;padding:6px 2px}";
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
  
  // Add Command
  html += "<div class='section'>";
  html += "<h3>➕ Add Command</h3>";
  html += "<div class='add-section'>";
  html += "<select id='cmd'>";
  html += "<option value='FORWARD'>⬆️ Forward</option>";
  html += "<option value='BACKWARD'>⬇️ Backward</option>";
  html += "<option value='LEFT'>⬅️ Left</option>";
  html += "<option value='RIGHT'>➡️ Right</option>";
  html += "<option value='STOP'>⏹️ Stop</option>";
  html += "<option value='CHECK_DISTANCE'>📏 Check Distance</option>";
  html += "</select>";
  html += "<input type='number' id='dur' placeholder='Duration (ms)' min='100' max='5000' value='1000'>";
  html += "<button onclick='addCmd()'>➕ Add</button>";
  html += "</div>";
  html += "</div>";
  
  // Sequence List
  html += "<div class='section'>";
  html += "<h3>📋 Command List (" + String(sequenceLength) + " commands)";
  if (isExecutingSequence) {
    html += " - <span style='color:#e74c3c'>🔄 RUNNING (Step " + String(currentSequenceStep + 1) + "/" + String(sequenceLength) + ")</span>";
  }
  html += "</h3>";
  html += "<div class='seq-list'>";
  if (sequenceLength == 0) {
    html += "<div style='text-align:center;opacity:0.7;padding:15px'>No commands added yet</div>";
  } else {
    for (int i = 0; i < sequenceLength; i++) {
      String emoji = "🎮";
      if (commandSequence[i].action == "FORWARD") emoji = "⬆️";
      else if (commandSequence[i].action == "BACKWARD") emoji = "⬇️";
      else if (commandSequence[i].action == "LEFT") emoji = "⬅️";
      else if (commandSequence[i].action == "RIGHT") emoji = "➡️";
      else if (commandSequence[i].action == "STOP") emoji = "⏹️";
      else if (commandSequence[i].action == "CHECK_DISTANCE") emoji = "📏";
      
      html += "<div class='seq-item'";
      if (isExecutingSequence && i == currentSequenceStep) {
        html += " style='background:#e74c3c;border:1px solid #fff'";
      }
      html += ">";
      html += "<span>" + String(i+1) + ". " + emoji + " " + commandSequence[i].action + "</span>";
      html += "<span>" + String(commandSequence[i].duration) + "ms</span>";
      html += "</div>";
    }
  }
  html += "</div>";
  
  // Control Buttons
  html += "<div class='btn-group'>";
  html += "<button class='run-btn' onclick='runSeq()'>🚀 Run Sequence</button>";
  if (isExecutingSequence) {
    html += "<button style='background:#e74c3c' onclick='stopSeq()'>⏹️ Stop Sequence</button>";
  }
  html += "<button class='clear-btn' onclick='clearSeq()'>🧹 Clear All</button>";
  html += "</div>";
  html += "</div>";
  
  html += "<script>";
  html += "function addCmd(){";
  html += "var cmd=document.getElementById('cmd').value;";
  html += "var dur=document.getElementById('dur').value;";
  html += "fetch('/seq?a=add&c='+cmd+'&d='+dur);";
  html += "setTimeout(()=>location.reload(),200);";
  html += "}";
  html += "function runSeq(){fetch('/seq?a=run');setTimeout(()=>location.reload(),500);}";
  html += "function stopSeq(){fetch('/seq?a=stop');setTimeout(()=>location.reload(),200);}";
  html += "function clearSeq(){fetch('/seq?a=clear');setTimeout(()=>location.reload(),200);}";
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
  html += ".log-container{background:#0f1419;padding:10px;border-radius:8px;border:1px solid #333;max-height:400px;overflow-y:auto;margin:8px 0}";
  html += ".log-text{font-family:'Courier New',monospace;font-size:13px;line-height:1.4;white-space:pre-line;color:#00ff00;word-wrap:break-word}";
  html += "button{padding:8px 15px;background:#4CAF50;border:none;color:white;border-radius:5px;cursor:pointer;margin:8px 0;font-size:14px}";
  html += "button:hover{background:#45a049}";
  html += "@media(max-width:600px){";
  html += ".log-container{max-height:300px;padding:8px}";
  html += ".log-text{font-size:12px;line-height:1.3}";
  html += ".nav button{font-size:11px;padding:6px 2px}";
  html += "button{width:100%;margin:5px 0}";
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
  
  html += "<button onclick='location.reload()'>🔄 Refresh Log</button>";
  
  // Log Content
  html += "<div class='log-container'>";
  html += "<div class='log-text'>";
  if (systemLog.length() == 0) {
    html += "No log entries yet...";
  } else {
    html += systemLog;
  }
  html += "</div>";
  html += "</div>";
  
  html += "<div style='text-align:center;font-size:12px;opacity:0.7;margin:10px 0'>";
  html += "Showing last 25 log entries • Auto-scroll to bottom";
  html += "</div>";
  
  html += "<script>";
  html += "window.onload=function(){";
  html += "var log=document.querySelector('.log-container');";
  html += "log.scrollTop=log.scrollHeight;";
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

// ========== WEB SERVER HANDLING ==========

void handleWebServer() {
  WiFiClient client = server.available();
  if (!client) return;
  
  String request = "";
  while (client.connected() && client.available()) {
    String line = client.readStringUntil('\r');
    request += line;
    if (line.length() == 1 && line[0] == '\n') break;
  }
  
  // Extract the path from the request
  String path = "";
  int pathStart = request.indexOf(' ') + 1;
  int pathEnd = request.indexOf(' ', pathStart);
  if (pathStart > 0 && pathEnd > pathStart) {
    path = request.substring(pathStart, pathEnd);
  }
  
  // Route handling
  if (path == "/" || path.startsWith("/?")) {
    // Main page or main page with parameters
    if (path.indexOf("command=") > 0) {
      // Handle calibration commands from main page
      String command = extractParameter(request, "command");
      executeCommand(command);
      sendResponse(client, "Command executed: " + command);
    } else {
      // Regular main page
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html; charset=UTF-8");
      client.println("Connection: close");
      client.println();
      client.println(createMainHTML());
    }
  }
  else if (path == "/control") {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html; charset=UTF-8");
    client.println("Connection: close");
    client.println();
    client.println(createControlHTML());
  }
  else if (path == "/seq" || path.startsWith("/seq?")) {
    if (path.indexOf("?") > 0) {
      // Handle sequence commands
      String action = extractParameter(request, "a");
      if (action == "add") {
        String command = extractParameter(request, "c");
        int duration = extractParameter(request, "d").toInt();
        if (duration == 0) duration = 1000;
        addToSequence(command, moveSpeed, duration);
      }
      else if (action == "run") {
        executeSequence();
      }
      else if (action == "stop") {
        if (isExecutingSequence) {
          isExecutingSequence = false;
          stopMotors();
          addLog("⏹️ Sequence execution stopped by user");
        }
      }
      else if (action == "clear") {
        clearSequence();
      }
      sendResponse(client, "Sequence action: " + action);
    } else {
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

String extractParameter(String request, String paramName) {
  String searchStr = paramName + "=";
  int startIndex = request.indexOf(searchStr);
  if (startIndex == -1) return "";
  
  startIndex += searchStr.length();
  int endIndex = request.indexOf('&', startIndex);
  if (endIndex == -1) endIndex = request.indexOf(' ', startIndex);
  if (endIndex == -1) endIndex = request.length();
  
  String value = request.substring(startIndex, endIndex);
  // URL decode basic characters
  value.replace("%20", " ");
  value.replace("+", " ");
  return value;
}

void sendResponse(WiFiClient client, String message) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain; charset=UTF-8");
  client.println("Connection: close");
  client.println();
  client.println(message);
}
