#include <Wire.h>
//74HC595
int latchPin = 18; // Latch pin of 74HC595
int clockPin = 5; // Clock pin of 74HC595
int dataPin = 19; // Data pin of 74HC595
byte led_data = 0; // Variable to hold the pattern of which LEDs are currently turned on or off
//gyro
const int sda = 21;
const int scl = 22;
int address = 0x68;
//define int for Gyrometer
byte GYRO_XOUT_H = 0;
byte GYRO_XOUT_L = 0;
int16_t GYRO_X_RAW = 0;
int16_t gyroX_Per_S = 0;
float elapsedTime = 0.0;
float currentTime = 0.0;
float previousTime = 0.0;
float oldGyroAngleX = 0.0;
float newGyroAngleX = 0.0;
//calibration
float calibrationOffsetX = 0.0;

void setup() {
  // //gyroscope
  Wire.begin(sda,scl); //to call the SDA and SCL
    Serial.begin(115200);
    Serial.println("Calibration Start!");
    for (int i = 0; i < 100; i ++) { //calibrate
        readGyrometer();
        calibrationOffsetX += gyroX_Per_S;
        delay(50);
    }
  // Calculate the average calibration offsets
    calibrationOffsetX /= 100.0;

  // Calibration Starts
  Serial.println("Calibration End");
  Serial.println(calibrationOffsetX);

  oldGyroAngleX = 0.0;

  // LED
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

void loop() {
  readGyrometer();
  printAngle();
  mapAngleToBinary();
  updateShiftRegister();
}

void readGyrometer(){
  Wire.beginTransmission(address);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(address, 6);
  GYRO_XOUT_H = Wire.read();
  GYRO_XOUT_L = Wire.read();
  GYRO_X_RAW = GYRO_XOUT_H << 8 | GYRO_XOUT_L; 
  gyroX_Per_S = GYRO_X_RAW/131.0; 
  // time
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000;
  previousTime = currentTime;
  newGyroAngleX = oldGyroAngleX + (gyroX_Per_S - calibrationOffsetX) * elapsedTime;
  oldGyroAngleX = newGyroAngleX;

}

void printAngle(){
  Serial.print("Angle X-axis:");
  Serial.println(newGyroAngleX);
}

void mapAngleToBinary(){
  if (newGyroAngleX >= 80){
    led_data = 0b11111111;
  }
  else if (newGyroAngleX >=70){
    led_data = 0b01111111;
  }
  else if (newGyroAngleX >=60){
    led_data = 0b00111111;
  }
  else if (newGyroAngleX >=50){
    led_data = 0b00011111;
  }
  else if (newGyroAngleX >=40){
    led_data = 0b00001111;
  }
  else if (newGyroAngleX >=30){
    led_data = 0b00000111;
  }
  else if (newGyroAngleX >=20){
    led_data = 0b0000011;
  }
  else if (newGyroAngleX >=10){
    led_data = 0b00000001;
  }
  else if (newGyroAngleX >=-1){
    led_data = 0b00000000;
  }
  else if (newGyroAngleX >=-10){
    led_data = 0b00000001;
  }
  else if (newGyroAngleX >=-20){
    led_data = 0b00000011;
  }
  else if (newGyroAngleX >=-30){
    led_data = 0b00000111;
  }
  else if (newGyroAngleX >=-40){
    led_data = 0b00001111;
  }
  else if (newGyroAngleX >=-50){
    led_data = 0b00011111;
  }
  else if (newGyroAngleX >=-60){
    led_data = 0b00111111;
  }
  else if (newGyroAngleX >=-70){
    led_data = 0b01111111;
  }
  else if (newGyroAngleX >=-80){
    led_data = 0b11111111;
  }
}

void updateShiftRegister(){
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, led_data);
  digitalWrite(latchPin,HIGH);
}
