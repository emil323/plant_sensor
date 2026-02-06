/*
 * Plant Weight Sensor with HX711 - Low Power Version
 * Arduino Pro Mini
 * 
 * Features:
 * - Deep sleep mode (wakes once per day)
 * - Interrupt-based button controls
 * - LED indicator for watering needed
 * - Auto plant growth compensation
 * - Ultra-low power consumption
 * 
 * Connections:
 * - HX711 DOUT    -> Pin 4
 * - HX711 SCK     -> Pin 5
 * - HX711 VCC     -> Pin 6 (power control)
 * - LED           -> Pin 9 (PWM capable)
 * - Dry Button    -> Pin 2 (INT0)
 * - Wet Button    -> Pin 3 (INT1)
 * 
 * LED Behavior:
 * - Slow breathing (3x): Not calibrated (missing config)
 * - 3 quick pulses: Button press feedback
 * - 3 slow pulses: Config cleared (both buttons held 2s)
 * - Single pulse: Needs water
 * - Fast pulsing (8x): Error (dry > wet)
 * - OFF: Plant is OK
 */

#include <HX711.h>
#include <EEPROM.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

// Pin definitions
const int HX711_DOUT = 4;
const int HX711_SCK = 5;
const int HX711_VCC = 6;
const int LED_PIN = 9;       // PWM capable pin
const int DRY_BUTTON = 2;    // INT0
const int WET_BUTTON = 3;    // INT1

// EEPROM addresses
const int ADDR_DRY_WEIGHT = 0;
const int ADDR_WET_WEIGHT = 4;
const int ADDR_PLANT_BASE = 8;
const int ADDR_CALIBRATED = 12;
const int ADDR_SLEEP_COUNTER = 16;
const int ADDR_NEEDS_WATER = 20;

// Settings
const float WATER_THRESHOLD = 0.25;
const int STABLE_READINGS = 10;
const int SLEEP_CYCLES_PER_DAY = 10800;  // 8s * 10800 = 24h
const int SLEEP_CYCLES_5_SEC = 1;        // For LED flash when needs water

// Variables
HX711 scale;
float dryWeight = 0;
float wetWeight = 0;
float plantBaseWeight = 0;
bool calibrated = false;
bool needsWater = false;
bool hasError = false;
volatile bool dryButtonPressed = false;
volatile bool wetButtonPressed = false;
volatile uint16_t sleepCounter = 0;

// Watchdog interrupt
ISR(WDT_vect) {
}

void setup() {
  // Load state
  EEPROM.get(ADDR_SLEEP_COUNTER, sleepCounter);
  EEPROM.get(ADDR_NEEDS_WATER, needsWater);
  
  // Configure pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(HX711_VCC, OUTPUT);
  pinMode(DRY_BUTTON, INPUT_PULLUP);
  pinMode(WET_BUTTON, INPUT_PULLUP);
  
  // Check if woken by button
  bool buttonWake = checkButtonWake();
  
  // Handle LED states first
  handleLEDState();
  
  // Do measurement if needed
  if (buttonWake || sleepCounter >= SLEEP_CYCLES_PER_DAY) {
    performMeasurement(buttonWake);
    if (!buttonWake) {
      sleepCounter = 0;
    }
  } else {
    sleepCounter++;
  }
  
  // Save state
  EEPROM.put(ADDR_SLEEP_COUNTER, sleepCounter);
  EEPROM.put(ADDR_NEEDS_WATER, needsWater);
  
  // Go to deep sleep
  goToSleep();
}

void loop() {
}

void handleLEDState() {
  loadCalibration();
  
  // Check for error condition
  if (calibrated && dryWeight >= wetWeight) {
    hasError = true;
  }
  
  // LED behavior based on state
  if (hasError) {
    // Fast pulsing for error
    for (int i = 0; i < 8; i++) {
      fadeLED(100);
    }
  } else if (!calibrated) {
    // Breathing pattern when not calibrated
    for (int i = 0; i < 3; i++) {
      fadeLED(600);
    }
  } else if (needsWater) {
    // Single gentle pulse
    fadeLED(400);
  } else {
    // OFF when everything is OK
    analogWrite(LED_PIN, 0);
  }
}

bool checkButtonWake() {
  delay(50);
  
  // Check if both buttons pressed - config reset
  if (digitalRead(DRY_BUTTON) == LOW && digitalRead(WET_BUTTON) == LOW) {
    // Wait 2 seconds, check if still held
    unsigned long startTime = millis();
    bool stillHeld = true;
    
    while (millis() - startTime < 2000) {
      if (digitalRead(DRY_BUTTON) == HIGH || digitalRead(WET_BUTTON) == HIGH) {
        stillHeld = false;
        break;
      }
      delay(100);
    }
    
    if (stillHeld) {
      clearConfiguration();
      // Slow fade pattern for reset confirmation
      for (int i = 0; i < 3; i++) {
        fadeLED(800);
      }
      return false; // Don't process as regular button press
    }
  }
  
  if (digitalRead(DRY_BUTTON) == LOW) {
    dryButtonPressed = true;
    // 3 quick pulses for feedback
    for (int i = 0; i < 3; i++) {
      fadeLED(150);
    }
    return true;
  }
  
  if (digitalRead(WET_BUTTON) == LOW) {
    wetButtonPressed = true;
    // 3 quick pulses for feedback
    for (int i = 0; i < 3; i++) {
      fadeLED(150);
    }
    return true;
  }
  
  return false;
}

void performMeasurement(bool isButtonWake) {
  // Power on HX711
  digitalWrite(HX711_VCC, HIGH);
  delay(100);
  
  // Initialize HX711
  scale.begin(HX711_DOUT, HX711_SCK);
  scale.set_scale();
  delay(500);
  scale.tare();
  delay(200);
  
  // Get current weight
  float currentWeight = getStableWeight();
  
  // Handle button press
  if (dryButtonPressed) {
    calibrateDryWeight(currentWeight);
    dryButtonPressed = false;
  } 
  else if (wetButtonPressed) {
    calibrateWetWeight(currentWeight);
    wetButtonPressed = false;
  }
  else if (calibrated && !hasError) {
    needsWater = monitorPlant(currentWeight);
  }
  
  // Power off HX711
  digitalWrite(HX711_VCC, LOW);
}

float getStableWeight() {
  float sum = 0;
  int validReadings = 0;
  
  for (int i = 0; i < STABLE_READINGS * 2; i++) {
    if (scale.is_ready()) {
      sum += scale.get_units(1);
      validReadings++;
      if (validReadings >= STABLE_READINGS) break;
    }
    delay(100);
  }
  
  return validReadings > 0 ? sum / validReadings : 0;
}

void calibrateDryWeight(float currentWeight) {
  dryWeight = currentWeight;
  
  if (!calibrated) {
    plantBaseWeight = dryWeight;
  }
  
  saveCalibration();
}

void calibrateWetWeight(float currentWeight) {
  if (dryWeight == 0 || currentWeight <= dryWeight) {
    hasError = true;
    return;
  }
  
  wetWeight = currentWeight;
  calibrated = true;
  hasError = false;
  saveCalibration();
}

bool monitorPlant(float currentWeight) {
  // Check for plant growth
  if (currentWeight > wetWeight) {
    float estimatedGrowth = currentWeight - wetWeight;
    if (estimatedGrowth < 200) {
      plantBaseWeight += estimatedGrowth;
      dryWeight += estimatedGrowth;
      wetWeight = currentWeight;
      saveCalibration();
    }
  }
  
  // Calculate water level
  float waterRange = wetWeight - dryWeight;
  float currentWater = currentWeight - dryWeight;
  float waterPercent = (currentWater / waterRange);
  
  if (waterPercent < 0) waterPercent = 0;
  if (waterPercent > 1.0) waterPercent = 1.0;
  
  return (waterPercent < WATER_THRESHOLD);
}

void saveCalibration() {
  EEPROM.put(ADDR_DRY_WEIGHT, dryWeight);
  EEPROM.put(ADDR_WET_WEIGHT, wetWeight);
  EEPROM.put(ADDR_PLANT_BASE, plantBaseWeight);
  EEPROM.put(ADDR_CALIBRATED, calibrated);
}

void loadCalibration() {
  EEPROM.get(ADDR_CALIBRATED, calibrated);
  
  if (calibrated) {
    EEPROM.get(ADDR_DRY_WEIGHT, dryWeight);
    EEPROM.get(ADDR_WET_WEIGHT, wetWeight);
    EEPROM.get(ADDR_PLANT_BASE, plantBaseWeight);
  }
}

void clearConfiguration() {
  dryWeight = 0;
  wetWeight = 0;
  plantBaseWeight = 0;
  calibrated = false;
  needsWater = false;
  hasError = false;
  sleepCounter = 0;
  
  EEPROM.put(ADDR_DRY_WEIGHT, dryWeight);
  EEPROM.put(ADDR_WET_WEIGHT, wetWeight);
  EEPROM.put(ADDR_PLANT_BASE, plantBaseWeight);
  EEPROM.put(ADDR_CALIBRATED, calibrated);
  EEPROM.put(ADDR_NEEDS_WATER, needsWater);
  EEPROM.put(ADDR_SLEEP_COUNTER, sleepCounter);
}

void fadeLED(int durationMs) {
  int halfDuration = durationMs / 2;
  int steps = 50;
  int stepDelay = halfDuration / steps;
  
  // Fade in
  for (int i = 0; i <= steps; i++) {
    int brightness = (i * 255) / steps;
    analogWrite(LED_PIN, brightness);
    delay(stepDelay);
  }
  
  // Fade out
  for (int i = steps; i >= 0; i--) {
    int brightness = (i * 255) / steps;
    analogWrite(LED_PIN, brightness);
    delay(stepDelay);
  }
  
  analogWrite(LED_PIN, 0);
}

void goToSleep() {
  ADCSRA = 0;
  
  for (int i = 0; i < 14; i++) {
    if (i != DRY_BUTTON && i != WET_BUTTON) {
      pinMode(i, INPUT_PULLUP);
    }
  }
  
  attachInterrupt(digitalPinToInterrupt(DRY_BUTTON), wakeUp, LOW);
  attachInterrupt(digitalPinToInterrupt(WET_BUTTON), wakeUp, LOW);
  
  setupWatchdog(9);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_bod_disable();
  sleep_cpu();
  
  sleep_disable();
  
  detachInterrupt(digitalPinToInterrupt(DRY_BUTTON));
  detachInterrupt(digitalPinToInterrupt(WET_BUTTON));
}

void wakeUp() {
}

void setupWatchdog(int interval) {
  byte wdtcsr = interval & 7;
  if (interval > 7) wdtcsr |= (1 << 5);
  wdtcsr |= (1 << WDCE);
  
  MCUSR &= ~(1 << WDRF);
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = wdtcsr;
  WDTCSR |= (1 << WDIE);
}
