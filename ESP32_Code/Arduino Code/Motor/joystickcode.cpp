#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Motor A pins
#define IN1 25
#define IN2 26
#define ENA 13  // PWM channel 0

// Motor B pins
#define IN3 27
#define IN4 14
#define ENB 12  // PWM channel 1

// NRF24L01 configuration
RF24 radio(5, 17);  // CE, CSN pins for NRF24L01 on ESP32
const byte address[6] = "Robo1";  // NRF24L01 address

// PWM configuration
#define PWM_FREQ 1000
#define PWM_RES 8
#define MAX_DUTY_CYCLE 255

void setup() {
    Serial.begin(115200);
    
    // Motor pins setup
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    // Setup PWM for motors
    ledcSetup(0, PWM_FREQ, PWM_RES);
    ledcAttachPin(ENA, 0);  // Motor A speed control
    ledcSetup(1, PWM_FREQ, PWM_RES);
    ledcAttachPin(ENB, 1);  // Motor B speed control

    // Setup NRF24L01 radio
    radio.begin();
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();
}

void loop() {
    uint8_t joystickData[2];  // Array to store X and Y data
    
    if (radio.available()) {
        radio.read(&joystickData, sizeof(joystickData));

        uint8_t joystickX = joystickData[0];  // X-axis value (0-255)
        uint8_t joystickY = joystickData[1];  // Y-axis value (0-255)

        // Process the joystick data to control motors
        controlMotors(joystickX, joystickY);
    }
}

void controlMotors(uint8_t x, uint8_t y) {
    // Convert joystick input (0-255) to motor control signals
    if (y > 135) {  // Forward motion
        setMotorA(1, map(y, 135, 255, 0, MAX_DUTY_CYCLE));
        setMotorB(1, map(y, 135, 255, 0, MAX_DUTY_CYCLE));
    } else if (y < 120) {  // Backward motion
        setMotorA(0, map(y, 0, 120, MAX_DUTY_CYCLE, 0));
        setMotorB(0, map(y, 0, 120, MAX_DUTY_CYCLE, 0));
    } else {  // Stop if joystick is in the middle
        setMotorA(0, 0);
        setMotorB(0, 0);
    }

    // Left-right control
    if (x > 135) {  // Turn right
        setMotorA(1, map(x, 135, 255, MAX_DUTY_CYCLE / 2, MAX_DUTY_CYCLE));
        setMotorB(1, 0);  // Slow down Motor B for turning
    } else if (x < 120) {  // Turn left
        setMotorA(1, 0);  // Slow down Motor A for turning
        setMotorB(1, map(x, 0, 120, MAX_DUTY_CYCLE / 2, MAX_DUTY_CYCLE));
    }
}

void setMotorA(bool forward, int speed) {
    if (forward) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
    } else {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
    }
    ledcWrite(0, speed);
}

void setMotorB(bool forward, int speed) {
    if (forward) {
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
    } else {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
    }
    ledcWrite(1, speed);
}