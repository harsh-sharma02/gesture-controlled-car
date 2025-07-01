#include <RF24.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int motorA1 = 3; 
const int motorA2 = 4;
const int motorB1 = 5; 
const int motorB2 = 6;

int motorSpeed = 180; 

const int trigPin = 8;
const int echoPin = 7;

RF24 radio(9, 10); 
const byte address[6] = "00001";
char receivedCommand;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);

  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Speed: ");
  lcd.setCursor(0, 1);
  lcd.print("Status: Idle");

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, address);
  radio.startListening();

  Serial.println("Receiver ready with PWM");
}

void loop() {
  if (radio.available()) {
    radio.read(&receivedCommand, sizeof(receivedCommand));
    Serial.print("Received: ");
    Serial.println(receivedCommand);

    handleCommand(receivedCommand);
  }
}

void handleCommand(char cmd) {
  int distance = readUltrasonic();
  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance < 20 && cmd == 'F') {
    stopMotors();
    lcd.setCursor(0, 1);
    lcd.print("Status: Obstacle");
    return;
  }

  switch (cmd) {
    case 'F': forward(); break;
    case 'B': backward(); break;
    case 'L': left(); break;
    case 'R': right(); break;
    case 'S': stopMotors(); break;
    default: stopMotors(); break;
  }

  lcd.setCursor(0, 0);
  lcd.print("Speed: ");
  lcd.print(motorSpeed);
  lcd.print("   "); 

  lcd.setCursor(0, 1);
  lcd.print("Status: ");
  lcd.print(cmd);
  lcd.print("     "); 
}

void forward() {
  analogWrite(motorA1, motorSpeed);
  digitalWrite(motorA2, LOW);
  analogWrite(motorB1, motorSpeed);
  digitalWrite(motorB2, LOW);
}

void backward() {
  digitalWrite(motorA1, LOW);
  analogWrite(motorA2, motorSpeed);
  digitalWrite(motorB1, LOW);
  analogWrite(motorB2, motorSpeed);
}

void left() {
  digitalWrite(motorA1, LOW);
  analogWrite(motorA2, motorSpeed);
  analogWrite(motorB1, motorSpeed);
  digitalWrite(motorB2, LOW);
}

void right() {
  analogWrite(motorA1, motorSpeed);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  analogWrite(motorB2, motorSpeed);
}

void stopMotors() {
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, LOW);
}

int readUltrasonic() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  return distance;
}
