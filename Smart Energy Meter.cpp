#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Sensor pins
const int voltagePin = A0;
const int currentPin = A1;

// Calibration values
// Adjust these according to your sensors and multimeter readings.
float voltageCalibration = 1.0;
float currentCalibration = 1.0;

// Energy calculation
unsigned long previousMillis = 0;
float energyWh = 0.0;

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Smart Energy");
  lcd.setCursor(0, 1);
  lcd.print("Meter Starting");
  delay(2000);

  lcd.clear();
}

float readVoltage() {
  long total = 0;

  for (int i = 0; i < 100; i++) {
    total += analogRead(voltagePin);
    delayMicroseconds(500);
  }

  float average = total / 100.0;

  // Basic sensor conversion.
  // Calibrate this value for your ZMPT101B module.
  float voltage = (average * 5.0 / 1023.0) * 100.0;

  voltage *= voltageCalibration;

  return voltage;
}

float readCurrent() {
  long total = 0;

  for (int i = 0; i < 100; i++) {
    total += analogRead(currentPin);
    delayMicroseconds(500);
  }

  float average = total / 100.0;

  float sensorVoltage = average * 5.0 / 1023.0;

  // ACS712 5A version:
  // Zero-current output is approximately 2.5V.
  float current = (sensorVoltage - 2.5) / 0.185;

  current *= currentCalibration;

  if (current < 0) {
    current = -current;
  }

  // Remove very small sensor noise.
  if (current < 0.05) {
    current = 0;
  }

  return current;
}

void loop() {
  float voltage = readVoltage();
  float current = readCurrent();

  float power = voltage * current;

  // Calculate energy in Wh.
  unsigned long currentMillis = millis();
  float elapsedHours =
      (currentMillis - previousMillis) / 3600000.0;

  energyWh += power * elapsedHours;
  previousMillis = currentMillis;

  // LCD display
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltage, 1);
  lcd.print(" I:");
  lcd.print(current, 2);

  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.print(power, 1);
  lcd.print("W");

  delay(1500);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Energy:");

  lcd.setCursor(0, 1);
  lcd.print(energyWh / 1000.0, 3);
  lcd.print(" kWh");

  // Serial monitor
  Serial.println("--------------------");
  Serial.print("Voltage: ");
  Serial.print(voltage, 2);
  Serial.println(" V");

  Serial.print("Current: ");
  Serial.print(current, 3);
  Serial.println(" A");

  Serial.print("Power: ");
  Serial.print(power, 2);
  Serial.println(" W");

  Serial.print("Energy: ");
  Serial.print(energyWh / 1000.0, 4);
  Serial.println(" kWh");

  delay(1500);
}
