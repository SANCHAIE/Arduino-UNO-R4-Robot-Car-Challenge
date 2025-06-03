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

// ตัวแปรการตั้งค่า
int moveSpeed = 200;
int turnSpeed = 150;
int moveDuration = 1000;
int turnDuration = 700;

// ตัวแปร Sequence
struct Command {
  String action;
  int speed;
  int duration;
};

Command commandSequence[20];
int sequenceLength = 0;
bool isExecutingSequence = false;

// ตัวแปรสถานะ
String systemLog = "";
int totalCommands = 0;
unsigned long startTime = 0;

// Function declarations
String createMainHTML();
String createControlHTML();
String createSequenceHTML();
String createLogHTML();
void handleWebServer();
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
  delay(10);
}

// ========== MOTOR FUNCTIONS - NO DUPLICATES ==========

// ฟังก์ชันหยุดมอเตอร์
void stopMotors() {
  analogWrite(MOTOR_LEFT_PWM, 0);
  analogWrite(MOTOR_RIGHT_PWM, 0);
  digitalWrite(MOTOR_LEFT_DIR1, LOW);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);
  digitalWrite(MOTOR_RIGHT_DIR1, LOW);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);
}

// ฟังก์ชันเคลื่อนที่แบบต่อเนื่อง (สำหรับกดค้าง)
void startForward(int speed) {
  digitalWrite(MOTOR_LEFT_DIR1, HIGH);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);
  digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);
  analogWrite(MOTOR_LEFT_PWM, speed);
  analogWrite(MOTOR_RIGHT_PWM, speed);
}

void startBackward(int speed) {
  digitalWrite(MOTOR_LEFT_DIR1, LOW);
  digitalWrite(MOTOR_LEFT_DIR2, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR1, LOW);
  digitalWrite(MOTOR_RIGHT_DIR2, HIGH);
  analogWrite(MOTOR_LEFT_PWM, speed);
  analogWrite(MOTOR_RIGHT_PWM, speed);
}

void startTurnLeft(int speed) {
  digitalWrite(MOTOR_LEFT_DIR1, LOW);
  digitalWrite(MOTOR_LEFT_DIR2, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR2, LOW);
  analogWrite(MOTOR_LEFT_PWM, speed);
  analogWrite(MOTOR_RIGHT_PWM, speed);
}

void startTurnRight(int speed) {
  digitalWrite(MOTOR_LEFT_DIR1, HIGH);
  digitalWrite(MOTOR_LEFT_DIR2, LOW);
  digitalWrite(MOTOR_RIGHT_DIR1, LOW);
  digitalWrite(MOTOR_RIGHT_DIR2, HIGH);
  analogWrite(MOTOR_LEFT_PWM, speed);
  analogWrite(MOTOR_RIGHT_PWM, speed);
}

// ฟังก์ชันเคลื่อนที่แบบมีระยะเวลา (สำหรับ sequence และ test)
void timedForward(int speed, int duration) {
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
    addLog("⬆️ Start moving forward (continuous)");
    startForward(moveSpeed);
  }
  else if (command == "START_BACKWARD") {
    addLog("⬇️ Start moving backward (continuous)");
    startBackward(moveSpeed);
  }
  else if (command == "START_LEFT") {
    addLog("⬅️ Start turning left (continuous)");
    startTurnLeft(turnSpeed);
  }
  else if (command == "START_RIGHT") {
    addLog("➡️ Start turning right (continuous)");
    startTurnRight(turnSpeed);
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
  
  addLog("🚀 Starting sequence execution (" + String(sequenceLength) + " commands)");
  addLog("⏳ Sequence in progress...");
  isExecutingSequence = true;
  
  for (int i = 0; i < sequenceLength; i++) {
    addLog("📍 Step " + String(i+1) + "/" + String(sequenceLength) + ": " + commandSequence[i].action);
    
    // รันคำสั่งโดยไม่ผ่าน executeCommand เพื่อไม่ให้ log ซ้ำ
    if (commandSequence[i].action == "FORWARD") {
      timedForward(commandSequence[i].speed, commandSequence[i].duration);
    }
    else if (commandSequence[i].action == "BACKWARD") {
      timedBackward(commandSequence[i].speed, commandSequence[i].duration);
    }
    else if (commandSequence[i].action == "LEFT") {
      timedTurnLeft(commandSequence[i].speed, commandSequence[i].duration);
    }
    else if (commandSequence[i].action == "RIGHT") {
      timedTurnRight(commandSequence[i].speed, commandSequence[i].duration);
    }
    else if (commandSequence[i].action == "STOP") {
      stopMotors();
      delay(commandSequence[i].duration);
    }
    
    addLog("✅ Step " + String(i+1) + " completed");
    delay(200);
  }
  
  isExecutingSequence = false;
  addLog("🎯 Sequence execution completed successfully!");
  addLog("📊 Total " + String(sequenceLength) + " commands executed");
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
  html += "<button onclick='location.href=\"/log\"'>📋 Log</button>";
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
  
  // Status
  html += "<div class='section'>";
  html += "<h3>📊 System Status</h3>";
  html += "<div class='status'>";
  html += "<div><strong>Commands:</strong> " + String(totalCommands) + "</div>";
  html += "<div><strong>Uptime:</strong> " + String((millis()-startTime)/1000) + "s</div>";
  html += "<div><strong>Sequence:</strong> " + String(sequenceLength) + " commands</div>";
  html += "<div><strong>IP:</strong> " + WiFi.localIP().toString() + "</div>";
  html += "</div>";
  html += "</div>";
  
  html += "<script>";
  html += "function saveSettings(){";
  html += "fetch('/set?ms='+document.getElementById('ms').value+'&ts='+document.getElementById('ts').value+'&mt='+document.getElementById('mt').value+'&tt='+document.getElementById('tt').value);";
  html += "setTimeout(()=>location.reload(),300);";
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
  html += "</select>";
  html += "<input type='number' id='dur' placeholder='Duration (ms)' min='100' max='5000' value='1000'>";
  html += "<button onclick='addCmd()'>➕ Add</button>";
  html += "</div>";
  html += "</div>";
  
  // Sequence List
  html += "<div class='section'>";
  html += "<h3>📋 Command List (" + String(sequenceLength) + " commands)</h3>";
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
      
      html += "<div class='seq-item'>";
      html += "<span>" + String(i+1) + ". " + emoji + " " + commandSequence[i].action + "</span>";
      html += "<span>" + String(commandSequence[i].duration) + "ms</span>";
      html += "</div>";
    }
  }
  html += "</div>";
  
  // Control Buttons
  html += "<div class='btn-group'>";
  html += "<button class='run-btn' onclick='runSeq()'>🚀 Run Sequence</button>";
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

// ========== URL PARAMETER EXTRACTION ==========

// Extract parameter จาก URL
String extractParameter(String request, String paramName) {
  int startPos = request.indexOf(paramName + "=");
  if (startPos == -1) return "";
  
  startPos += paramName.length() + 1;
  int endPos = request.indexOf("&", startPos);
  if (endPos == -1) endPos = request.indexOf(" ", startPos);
  
  return request.substring(startPos, endPos);
}

// ส่ง response แบบสั้น
void sendResponse(WiFiClient client, String message) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println("Connection: close");
  client.println();
  client.println(message);
}

// ========== WEB SERVER HANDLER ==========

// จัดการ Web Server
void handleWebServer() {
  WiFiClient client = server.available();
  
  if (client) {
    String request = "";
    while (client.connected() && client.available()) {
      String line = client.readStringUntil('\n');
      if (line.length() == 1 && line[0] == '\n') break;
      request += line;
    }
    
    String html = "";
    
    // ตรวจสอบคำสั่ง
    if (request.indexOf("GET /cmd?a=") >= 0) {
      String cmd = extractParameter(request, "a");
      if (cmd == "FORWARD") executeCommand("FORWARD");
      else if (cmd == "BACKWARD") executeCommand("BACKWARD");
      else if (cmd == "LEFT") executeCommand("LEFT");
      else if (cmd == "RIGHT") executeCommand("RIGHT");
      else if (cmd == "STOP") executeCommand("STOP");
      else if (cmd == "TEST_TURN") executeCommand("TEST_TURN");
      else if (cmd == "SQUARE") executeCommand("SQUARE");
      // คำสั่งใหม่สำหรับกดค้าง
      else if (cmd == "START_FORWARD") executeCommand("START_FORWARD");
      else if (cmd == "START_BACKWARD") executeCommand("START_BACKWARD");
      else if (cmd == "START_LEFT") executeCommand("START_LEFT");
      else if (cmd == "START_RIGHT") executeCommand("START_RIGHT");
      
      sendResponse(client, "OK");
      client.stop();
      return;
    }
    else if (request.indexOf("GET /set?") >= 0) {
      String ms = extractParameter(request, "ms");
      String ts = extractParameter(request, "ts");
      String mt = extractParameter(request, "mt");
      String tt = extractParameter(request, "tt");
      
      String changes = "";
      if (ms.length() > 0) {
        int oldSpeed = moveSpeed;
        moveSpeed = ms.toInt();
        changes += "Move Speed: " + String(oldSpeed) + "→" + String(moveSpeed) + " ";
      }
      if (ts.length() > 0) {
        int oldSpeed = turnSpeed;
        turnSpeed = ts.toInt();
        changes += "Turn Speed: " + String(oldSpeed) + "→" + String(turnSpeed) + " ";
      }
      if (mt.length() > 0) {
        int oldTime = moveDuration;
        moveDuration = mt.toInt();
        changes += "Move Time: " + String(oldTime) + "→" + String(moveDuration) + "ms ";
      }
      if (tt.length() > 0) {
        int oldTime = turnDuration;
        turnDuration = tt.toInt();
        changes += "Turn Time: " + String(oldTime) + "→" + String(turnDuration) + "ms ";
      }
      
      addLog("⚙️ Settings updated: " + changes);
      sendResponse(client, "OK");
      client.stop();
      return;
    }
    else if (request.indexOf("GET /seq?") >= 0) {
      String action = extractParameter(request, "a");
      
      if (action == "add") {
        String c = extractParameter(request, "c");
        String d = extractParameter(request, "d");
        int duration = (d.length() > 0) ? d.toInt() : 1000;
        int speed = (c == "FORWARD" || c == "BACKWARD") ? moveSpeed : turnSpeed;
        addToSequence(c, speed, duration);
      }
      else if (action == "run") {
        executeSequence();
      }
      else if (action == "clear") {
        clearSequence();
      }
      
      sendResponse(client, "OK");
      client.stop();
      return;
    }
    // Navigation
    else if (request.indexOf("GET /control") >= 0) {
      html = createControlHTML();
    }
    else if (request.indexOf("GET /seq") >= 0) {
      html = createSequenceHTML();
    }
    else if (request.indexOf("GET /log") >= 0) {
      html = createLogHTML();
    }
    else {
      // หน้าหลัก
      html = createMainHTML();
    }
    
    // ส่ง HTML
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html; charset=UTF-8");
    client.println("Connection: close");
    client.println();
    client.println(html);
    
    client.stop();
  }
}
