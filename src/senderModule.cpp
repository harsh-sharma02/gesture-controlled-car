#include <Wire.h>
#include <RF24.h>
#include <MPU6050.h>

RF24 radio(9, 10); 

const byte address[6] = "00001";

MPU6050 accelgyro;

const int threshold_forward = 15000;
const int threshold_backward = -15000;
const int threshold_left = -15000;
const int threshold_right = 15000;

int16_t ax, ay, az;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  accelgyro.initialize();
  if (!accelgyro.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }
  Serial.println("MPU6050 connected");

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS); 
  radio.openWritingPipe(address);
  radio.stopListening(); 

  Serial.println("Gesture sender ready.");
}

void loop() {
  accelgyro.getAcceleration(&ax, &ay, &az);

  Serial.print("X = "); Serial.print(ax);
  Serial.print(" | Y = "); Serial.print(ay);
  Serial.print(" | Z = "); Serial.println(az);

  char command;

  if (ay > threshold_forward) {
    command = 'F'; 
  } else if (ay < threshold_backward) {
    command = 'B';
  } else if (ax < threshold_left) {
    command = 'L';
  } else if (ax > threshold_right) {
    command = 'R'; 
  } else {
    command = 'S'; 
  }

  radio.write(&command, sizeof(command));

  Serial.print("Sent: ");
  Serial.println(command);

  delay(200);
}
