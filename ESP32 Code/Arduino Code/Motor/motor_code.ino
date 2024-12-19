// Pin definitions for Motor 1
#define ENA 9   // PWM pin for Motor 1 speed control
#define IN1 10   // Direction control pin 1 for Motor 1
#define IN2 8   // Direction control pin 2 for Motor 1

// Pin definitions for Motor 2
#define ENB 6   // PWM pin for Motor 2 speed control
#define IN3 7   // Direction control pin 1 for Motor 2
#define IN4 5   // Direction control pin 2 for Motor 2

// Motor speed variables (0 to 255)
int motor1Speed = 200; // Speed for Motor 1
int motor2Speed = 150; // Speed for Motor 2

void setup() {
  // Set pins as outputs
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Initialize motors stopped
  stopMotor1();
  stopMotor2();
}

void loop() {
  // Motor 1: Forward
  forwardMotor1(motor1Speed);
  delay(2000); // Run for 2 seconds

  // Motor 1: Backward
  backwardMotor1(motor1Speed);
  delay(2000); // Run for 2 seconds

  // Motor 1: Stop
  stopMotor1();
  delay(1000); // Pause for 1 second

  // Motor 2: Forward
  forwardMotor2(motor2Speed);
  delay(2000); // Run for 2 seconds

  // Motor 2: Backward
  backwardMotor2(motor2Speed);
  delay(2000); // Run for 2 seconds

  // Motor 2: Stop
  stopMotor2();
  delay(1000); // Pause for 1 second
}

// Functions to control Motor 1
void forwardMotor1(int speed) {
  analogWrite(ENA, speed); // Set speed
  digitalWrite(IN1, HIGH); // Set IN1 HIGH
  digitalWrite(IN2, LOW);  // Set IN2 LOW
}

void backwardMotor1(int speed) {
  analogWrite(ENA, speed); // Set speed
  digitalWrite(IN1, LOW);  // Set IN1 LOW
  digitalWrite(IN2, HIGH); // Set IN2 HIGH
}

void stopMotor1() {
  analogWrite(ENA, 0);     // Stop motor
  digitalWrite(IN1, LOW);  // IN1 LOW
  digitalWrite(IN2, LOW);  // IN2 LOW
}

// Functions to control Motor 2
void forwardMotor2(int speed) {
  analogWrite(ENB, speed); // Set speed
  digitalWrite(IN3, HIGH); // Set IN3 HIGH
  digitalWrite(IN4, LOW);  // Set IN4 LOW
}

void backwardMotor2(int speed) {
  analogWrite(ENB, speed); // Set speed
  digitalWrite(IN3, LOW);  // Set IN3 LOW
  digitalWrite(IN4, HIGH); // Set IN4 HIGH
}

void stopMotor2() {
  analogWrite(ENB, 0);     // Stop motor
  digitalWrite(IN3, LOW);  // IN3 LOW
  digitalWrite(IN4, LOW);
}