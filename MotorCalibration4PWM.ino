/*
  Motor Speed Calibration for Arduino Robot Car - 4-Pin PWM Motor Driver
  
  This code helps calibrate the speed of both motors to ensure the robot
  moves straight when both motors are set to the same speed value.
  
  Designed for 4-pin motor drivers with built-in PWM capability.
  Each motor has separate PWM pins for forward and backward direction.
  
  Instructions:
  1. Place the robot on a flat surface with enough space to move
  2. Upload this code to your Arduino
  3. Open Serial Monitor (9600 baud)
  4. Follow the on-screen instructions
  5. Record the calibration values for use in your main robot code
  
  Hardware Connections for 4-Pin Motor Driver:
  - Left Motor Forward: Pin 9 (PWM)
  - Left Motor Backward: Pin 10 (PWM)
  - Right Motor Forward: Pin 5 (PWM)
  - Right Motor Backward: Pin 6 (PWM)
*/

// Motor pins for 4-Pin PWM Motor Driver
const int leftMotorForward = 9;
const int leftMotorBackward = 10;
const int rightMotorForward = 5;
const int rightMotorBackward = 6;

// Calibration variables
int baseSpeed = 150;  // Base speed for testing (0-255)
int leftSpeedOffset = 0;   // Offset for left motor (-50 to +50)
int rightSpeedOffset = 0;  // Offset for right motor (-50 to +50)

// Test duration
const unsigned long testDuration = 3000; // 3 seconds per test

void setup() {
  Serial.begin(9600);
  
  // Initialize motor pins
  pinMode(leftMotorForward, OUTPUT);
  pinMode(leftMotorBackward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT);
  pinMode(rightMotorBackward, OUTPUT);
  
  // Stop motors initially
  stopMotors();
  
  Serial.println("=== Motor Speed Calibration (4-Pin PWM Driver) ===");
  Serial.println("Place robot on flat surface with space to move");
  Serial.println();
  Serial.println("Commands:");
  Serial.println("'t' - Test current settings (forward)");
  Serial.println("'tb' - Test backward movement");
  Serial.println("'tl' - Test turn left");
  Serial.println("'tr' - Test turn right");
  Serial.println("'l+' - Increase left motor speed (+5)");
  Serial.println("'l-' - Decrease left motor speed (-5)");
  Serial.println("'r+' - Increase right motor speed (+5)");
  Serial.println("'r-' - Decrease right motor speed (-5)");
  Serial.println("'b+' - Increase base speed (+10)");
  Serial.println("'b-' - Decrease base speed (-10)");
  Serial.println("'s' - Show current settings");
  Serial.println("'c' - Run complete calibration sequence");
  Serial.println("'h' - Show this help");
  Serial.println();
  showCurrentSettings();
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readString();
    command.trim();
    command.toLowerCase();
    
    if (command == "t") {
      testForward();
    }
    else if (command == "tb") {
      testBackward();
    }
    else if (command == "tl") {
      testTurnLeft();
    }
    else if (command == "tr") {
      testTurnRight();
    }
    else if (command == "l+") {
      leftSpeedOffset = constrain(leftSpeedOffset + 5, -50, 50);
      Serial.println("Left motor speed increased");
      showCurrentSettings();
    }
    else if (command == "l-") {
      leftSpeedOffset = constrain(leftSpeedOffset - 5, -50, 50);
      Serial.println("Left motor speed decreased");
      showCurrentSettings();
    }
    else if (command == "r+") {
      rightSpeedOffset = constrain(rightSpeedOffset + 5, -50, 50);
      Serial.println("Right motor speed increased");
      showCurrentSettings();
    }
    else if (command == "r-") {
      rightSpeedOffset = constrain(rightSpeedOffset - 5, -50, 50);
      Serial.println("Right motor speed decreased");
      showCurrentSettings();
    }
    else if (command == "b+") {
      baseSpeed = constrain(baseSpeed + 10, 50, 255);
      Serial.println("Base speed increased");
      showCurrentSettings();
    }
    else if (command == "b-") {
      baseSpeed = constrain(baseSpeed - 10, 50, 255);
      Serial.println("Base speed decreased");
      showCurrentSettings();
    }
    else if (command == "s") {
      showCurrentSettings();
    }
    else if (command == "c") {
      runCompleteCalibration();
    }
    else if (command == "h") {
      showHelp();
    }
    else {
      Serial.println("Unknown command. Type 'h' for help.");
    }
  }
}

// ========== MOTOR CONTROL FUNCTIONS FOR 4-PIN PWM DRIVER ==========

void stopMotors() {
  analogWrite(leftMotorForward, 0);
  analogWrite(leftMotorBackward, 0);
  analogWrite(rightMotorForward, 0);
  analogWrite(rightMotorBackward, 0);
}

void moveForward(int leftSpeed, int rightSpeed) {
  stopMotors(); // หยุดก่อนเปลี่ยนทิศทาง
  delay(10);
  
  analogWrite(leftMotorForward, leftSpeed);
  analogWrite(rightMotorForward, rightSpeed);
}

void moveBackward(int leftSpeed, int rightSpeed) {
  stopMotors(); // หยุดก่อนเปลี่ยนทิศทาง
  delay(10);
  
  analogWrite(leftMotorBackward, leftSpeed);
  analogWrite(rightMotorBackward, rightSpeed);
}

void turnLeft(int leftSpeed, int rightSpeed) {
  stopMotors(); // หยุดก่อนเปลี่ยนทิศทาง
  delay(10);
  
  // เลี้ยวซ้าย: มอเตอร์ซ้ายถอย, มอเตอร์ขวาเดิน
  analogWrite(leftMotorBackward, leftSpeed);
  analogWrite(rightMotorForward, rightSpeed);
}

void turnRight(int leftSpeed, int rightSpeed) {
  stopMotors(); // หยุดก่อนเปลี่ยนทิศทาง
  delay(10);
  
  // เลี้ยวขวา: มอเตอร์ซ้ายเดิน, มอเตอร์ขวาถอย
  analogWrite(leftMotorForward, leftSpeed);
  analogWrite(rightMotorBackward, rightSpeed);
}

// ========== TEST FUNCTIONS ==========

void testForward() {
  Serial.println("\n=== Testing Forward Movement ===");
  Serial.print("Running for ");
  Serial.print(testDuration / 1000);
  Serial.println(" seconds...");
  
  int leftSpeed = constrain(baseSpeed + leftSpeedOffset, 0, 255);
  int rightSpeed = constrain(baseSpeed + rightSpeedOffset, 0, 255);
  
  Serial.print("Left motor: ");
  Serial.print(leftSpeed);
  Serial.print(", Right motor: ");
  Serial.println(rightSpeed);
  
  moveForward(leftSpeed, rightSpeed);
  delay(testDuration);
  stopMotors();
  
  Serial.println("Forward test complete. Observe if robot moved straight.");
  Serial.println("If robot turned left: increase left motor or decrease right motor");
  Serial.println("If robot turned right: increase right motor or decrease left motor");
  Serial.println();
}

void testBackward() {
  Serial.println("\n=== Testing Backward Movement ===");
  Serial.print("Running for ");
  Serial.print(testDuration / 1000);
  Serial.println(" seconds...");
  
  int leftSpeed = constrain(baseSpeed + leftSpeedOffset, 0, 255);
  int rightSpeed = constrain(baseSpeed + rightSpeedOffset, 0, 255);
  
  Serial.print("Left motor: ");
  Serial.print(leftSpeed);
  Serial.print(", Right motor: ");
  Serial.println(rightSpeed);
  
  moveBackward(leftSpeed, rightSpeed);
  delay(testDuration);
  stopMotors();
  
  Serial.println("Backward test complete.");
  Serial.println();
}

void testTurnLeft() {
  Serial.println("\n=== Testing Turn Left ===");
  Serial.println("Running for 1 second...");
  
  int leftSpeed = constrain(baseSpeed + leftSpeedOffset, 0, 255);
  int rightSpeed = constrain(baseSpeed + rightSpeedOffset, 0, 255);
  
  turnLeft(leftSpeed, rightSpeed);
  delay(1000);
  stopMotors();
  
  Serial.println("Turn left test complete.");
  Serial.println();
}

void testTurnRight() {
  Serial.println("\n=== Testing Turn Right ===");
  Serial.println("Running for 1 second...");
  
  int leftSpeed = constrain(baseSpeed + leftSpeedOffset, 0, 255);
  int rightSpeed = constrain(baseSpeed + rightSpeedOffset, 0, 255);
  
  turnRight(leftSpeed, rightSpeed);
  delay(1000);
  stopMotors();
  
  Serial.println("Turn right test complete.");
  Serial.println();
}

void runCompleteCalibration() {
  Serial.println("\n=== Complete Calibration Sequence ===");
  Serial.println("This will test different speed combinations for forward movement");
  Serial.println("Observe which combination makes the robot go straightest");
  Serial.println();
  
  // Test different offset combinations
  int testOffsets[] = {-10, -5, 0, 5, 10};
  int numTests = sizeof(testOffsets) / sizeof(testOffsets[0]);
  
  for (int l = 0; l < numTests; l++) {
    for (int r = 0; r < numTests; r++) {
      int leftOffset = testOffsets[l];
      int rightOffset = testOffsets[r];
      
      Serial.print("Test ");
      Serial.print((l * numTests) + r + 1);
      Serial.print("/");
      Serial.print(numTests * numTests);
      Serial.print(" - Left offset: ");
      Serial.print(leftOffset);
      Serial.print(", Right offset: ");
      Serial.println(rightOffset);
      
      int leftSpeed = constrain(baseSpeed + leftOffset, 0, 255);
      int rightSpeed = constrain(baseSpeed + rightOffset, 0, 255);
      
      moveForward(leftSpeed, rightSpeed);
      delay(2000); // 2 seconds per test
      stopMotors();
      delay(1000); // 1 second pause between tests
    }
  }
  
  Serial.println("\nCalibration sequence complete!");
  Serial.println("Choose the best combination and set it manually using l+/l-/r+/r- commands");
  Serial.println();
}

// ========== DISPLAY FUNCTIONS ==========

void showCurrentSettings() {
  int leftSpeed = constrain(baseSpeed + leftSpeedOffset, 0, 255);
  int rightSpeed = constrain(baseSpeed + rightSpeedOffset, 0, 255);
  
  Serial.println("\n--- Current Settings ---");
  Serial.print("Base Speed: ");
  Serial.println(baseSpeed);
  Serial.print("Left Offset: ");
  Serial.println(leftSpeedOffset);
  Serial.print("Right Offset: ");
  Serial.println(rightSpeedOffset);
  Serial.print("Actual Left Speed: ");
  Serial.println(leftSpeed);
  Serial.print("Actual Right Speed: ");
  Serial.println(rightSpeed);
  Serial.println();
  
  // Show code for main program
  Serial.println("--- Code for Main Program (4PWM) ---");
  Serial.println("// Add these calibration values to your 4PWM robot code:");
  Serial.print("const int LEFT_MOTOR_OFFSET = ");
  Serial.print(leftSpeedOffset);
  Serial.println(";");
  Serial.print("const int RIGHT_MOTOR_OFFSET = ");
  Serial.print(rightSpeedOffset);
  Serial.println(";");
  Serial.println();
  Serial.println("// Use in your motor functions:");
  Serial.println("int calibratedLeftSpeed = constrain(speed + LEFT_MOTOR_OFFSET, 0, 255);");
  Serial.println("int calibratedRightSpeed = constrain(speed + RIGHT_MOTOR_OFFSET, 0, 255);");
  Serial.println();
}

void showHelp() {
  Serial.println("\n=== Motor Calibration Help (4PWM) ===");
  Serial.println("Commands:");
  Serial.println("'t' - Test forward movement (motors run for 3 seconds)");
  Serial.println("'tb' - Test backward movement");
  Serial.println("'tl' - Test turn left");
  Serial.println("'tr' - Test turn right");
  Serial.println("'l+' - Increase left motor speed (+5)");
  Serial.println("'l-' - Decrease left motor speed (-5)");
  Serial.println("'r+' - Increase right motor speed (+5)");
  Serial.println("'r-' - Decrease right motor speed (-5)");
  Serial.println("'b+' - Increase base speed (+10)");
  Serial.println("'b-' - Decrease base speed (-10)");
  Serial.println("'s' - Show current settings");
  Serial.println("'c' - Run complete calibration sequence");
  Serial.println("'h' - Show this help");
  Serial.println();
  Serial.println("4PWM Motor Driver Info:");
  Serial.println("- Pin 9: Left Motor Forward (PWM)");
  Serial.println("- Pin 10: Left Motor Backward (PWM)");
  Serial.println("- Pin 5: Right Motor Forward (PWM)");
  Serial.println("- Pin 6: Right Motor Backward (PWM)");
  Serial.println();
  Serial.println("Calibration Tips:");
  Serial.println("1. Use a flat, smooth surface");
  Serial.println("2. Ensure wheels are clean and have good grip");
  Serial.println("3. Test at different speeds (use b+/b- to change base speed)");
  Serial.println("4. Small adjustments work better than large ones");
  Serial.println("5. Mark starting position to measure deviation");
  Serial.println("6. Test all directions (forward, backward, turns)");
  Serial.println();
}

// ========== ADVANCED CALIBRATION FUNCTIONS ==========

void runDistanceCalibration() {
  Serial.println("\n=== Distance Calibration Test ===");
  Serial.println("Place ruler or measuring tape alongside robot path");
  Serial.println("Robot will move forward for exactly 10 seconds");
  Serial.println("Measure the distance traveled by each wheel");
  Serial.println();
  
  Serial.println("Starting distance test in 3 seconds...");
  delay(3000);
  
  int leftSpeed = constrain(baseSpeed + leftSpeedOffset, 0, 255);
  int rightSpeed = constrain(baseSpeed + rightSpeedOffset, 0, 255);
  
  // Run motors for precise timing
  unsigned long startTime = millis();
  moveForward(leftSpeed, rightSpeed);
  
  while (millis() - startTime < 10000) {
    // Run for exactly 10 seconds
    delay(100);
  }
  
  stopMotors();
  
  Serial.println("Distance test complete!");
  Serial.println("Measure the distance each wheel traveled.");
  Serial.println("If distances are different, adjust motor offsets accordingly.");
  Serial.println();
}

void runTurnCalibration() {
  Serial.println("\n=== Turn Calibration Test ===");
  Serial.println("Robot will attempt a 90-degree right turn");
  Serial.println("Observe the actual turn angle and adjust timing if needed");
  Serial.println();
  
  Serial.println("Starting turn test in 3 seconds...");
  delay(3000);
  
  int leftSpeed = constrain(baseSpeed + leftSpeedOffset, 0, 255);
  int rightSpeed = constrain(baseSpeed + rightSpeedOffset, 0, 255);
  
  turnRight(leftSpeed, rightSpeed);
  delay(1000); // Adjust this timing for 90-degree turn
  stopMotors();
  
  Serial.println("Turn test complete!");
  Serial.println("Measure actual turn angle.");
  Serial.println("Adjust timing in your main code for precise turns.");
  Serial.println();
}

void runFullSystemTest() {
  Serial.println("\n=== Full System Test ===");
  Serial.println("Running complete movement test sequence...");
  Serial.println();
  
  int leftSpeed = constrain(baseSpeed + leftSpeedOffset, 0, 255);
  int rightSpeed = constrain(baseSpeed + rightSpeedOffset, 0, 255);
  
  // Forward
  Serial.println("1. Moving forward...");
  moveForward(leftSpeed, rightSpeed);
  delay(2000);
  stopMotors();
  delay(500);
  
  // Backward
  Serial.println("2. Moving backward...");
  moveBackward(leftSpeed, rightSpeed);
  delay(2000);
  stopMotors();
  delay(500);
  
  // Turn left
  Serial.println("3. Turning left...");
  turnLeft(leftSpeed, rightSpeed);
  delay(1000);
  stopMotors();
  delay(500);
  
  // Turn right
  Serial.println("4. Turning right...");
  turnRight(leftSpeed, rightSpeed);
  delay(1000);
  stopMotors();
  
  Serial.println("\nFull system test complete!");
  Serial.println("All motor functions tested successfully.");
  Serial.println();
}
