/*
  Motor Speed Calibration for Arduino Robot Car
  
  This code helps calibrate the speed of both motors to ensure the robot
  moves straight when both motors are set to the same speed value.
  
  Instructions:
  1. Place the robot on a flat surface with enough space to move
  2. Upload this code to your Arduino
  3. Open Serial Monitor (9600 baud)
  4. Follow the on-screen instructions
  5. Record the calibration values for use in your main robot code
  
  Hardware Connections:
  - Left Motor: Pin 9 (PWM), Pin 8 (Direction)
  - Right Motor: Pin 10 (PWM), Pin 7 (Direction)
  - Optional: Encoder pins for more accurate calibration
*/

// Motor pins
const int leftMotorPWM = 9;
const int leftMotorDir = 8;
const int rightMotorPWM = 10;
const int rightMotorDir = 7;

// Calibration variables
int baseSpeed = 150;  // Base speed for testing (0-255)
int leftSpeedOffset = 0;   // Offset for left motor (-50 to +50)
int rightSpeedOffset = 0;  // Offset for right motor (-50 to +50)

// Test duration
const unsigned long testDuration = 3000; // 3 seconds per test

void setup() {
  Serial.begin(9600);
  
  // Initialize motor pins
  pinMode(leftMotorPWM, OUTPUT);
  pinMode(leftMotorDir, OUTPUT);
  pinMode(rightMotorPWM, OUTPUT);
  pinMode(rightMotorDir, OUTPUT);
  
  // Set initial direction (forward)
  digitalWrite(leftMotorDir, HIGH);
  digitalWrite(rightMotorDir, HIGH);
  
  // Stop motors initially
  analogWrite(leftMotorPWM, 0);
  analogWrite(rightMotorPWM, 0);
  
  Serial.println("=== Motor Speed Calibration ===");
  Serial.println("Place robot on flat surface with space to move");
  Serial.println("Commands:");
  Serial.println("'t' - Test current settings");
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
      testCurrentSettings();
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

void testCurrentSettings() {
  Serial.println("\n=== Testing Current Settings ===");
  Serial.print("Running for ");
  Serial.print(testDuration / 1000);
  Serial.println(" seconds...");
  
  int leftSpeed = constrain(baseSpeed + leftSpeedOffset, 0, 255);
  int rightSpeed = constrain(baseSpeed + rightSpeedOffset, 0, 255);
  
  Serial.print("Left motor: ");
  Serial.print(leftSpeed);
  Serial.print(", Right motor: ");
  Serial.println(rightSpeed);
  
  // Start motors
  analogWrite(leftMotorPWM, leftSpeed);
  analogWrite(rightMotorPWM, rightSpeed);
  
  delay(testDuration);
  
  // Stop motors
  analogWrite(leftMotorPWM, 0);
  analogWrite(rightMotorPWM, 0);
  
  Serial.println("Test complete. Observe if robot moved straight.");
  Serial.println("If robot turned left: increase left motor or decrease right motor");
  Serial.println("If robot turned right: increase right motor or decrease left motor");
  Serial.println();
}

void runCompleteCalibration() {
  Serial.println("\n=== Complete Calibration Sequence ===");
  Serial.println("This will test different speed combinations");
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
      
      // Start motors
      analogWrite(leftMotorPWM, leftSpeed);
      analogWrite(rightMotorPWM, rightSpeed);
      
      delay(2000); // 2 seconds per test
      
      // Stop motors
      analogWrite(leftMotorPWM, 0);
      analogWrite(rightMotorPWM, 0);
      
      delay(1000); // 1 second pause between tests
    }
  }
  
  Serial.println("\nCalibration sequence complete!");
  Serial.println("Choose the best combination and set it manually using l+/l-/r+/r- commands");
  Serial.println();
}

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
  Serial.println("--- Code for Main Program ---");
  Serial.println("// Add these calibration values to your main robot code:");
  Serial.print("const int LEFT_MOTOR_OFFSET = ");
  Serial.print(leftSpeedOffset);
  Serial.println(";");
  Serial.print("const int RIGHT_MOTOR_OFFSET = ");
  Serial.print(rightSpeedOffset);
  Serial.println(";");
  Serial.println();
}

void showHelp() {
  Serial.println("\n=== Motor Calibration Help ===");
  Serial.println("Commands:");
  Serial.println("'t' - Test current settings (motors run for 3 seconds)");
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
  Serial.println("Calibration Tips:");
  Serial.println("1. Use a flat, smooth surface");
  Serial.println("2. Ensure wheels are clean and have good grip");
  Serial.println("3. Test at different speeds (use b+/b- to change base speed)");
  Serial.println("4. Small adjustments work better than large ones");
  Serial.println("5. Mark starting position to measure deviation");
  Serial.println();
}

// Additional functions for advanced calibration

void runDistanceCalibration() {
  // This function can be used to calibrate based on distance traveled
  // Requires measurement of actual distance vs expected distance
  Serial.println("Place ruler or measure tape alongside robot path");
  Serial.println("Robot will move forward for exactly 10 seconds");
  Serial.println("Measure the distance traveled by each wheel");
  
  // Run motors for precise timing
  unsigned long startTime = millis();
  analogWrite(leftMotorPWM, baseSpeed + leftSpeedOffset);
  analogWrite(rightMotorPWM, baseSpeed + rightSpeedOffset);
  
  while (millis() - startTime < 10000) {
    // Run for exactly 10 seconds
  }
  
  analogWrite(leftMotorPWM, 0);
  analogWrite(rightMotorPWM, 0);
  
  Serial.println("Distance test complete. Measure the distance each wheel traveled.");
}

void runTurnCalibration() {
  // Test turning to calibrate for precise rotation
  Serial.println("Turn calibration - robot will attempt 90-degree turn");
  
  // Right turn test
  digitalWrite(leftMotorDir, HIGH);   // Left forward
  digitalWrite(rightMotorDir, LOW);   // Right backward
  
  analogWrite(leftMotorPWM, baseSpeed);
  analogWrite(rightMotorPWM, baseSpeed);
  
  delay(1000); // Adjust this timing for 90-degree turn
  
  analogWrite(leftMotorPWM, 0);
  analogWrite(rightMotorPWM, 0);
  
  // Reset direction
  digitalWrite(leftMotorDir, HIGH);
  digitalWrite(rightMotorDir, HIGH);
  
  Serial.println("Turn test complete. Measure actual turn angle.");
}
