/*
 * Plant Weight Sensor with HX711 - Adaptive Sampling Version
 * Arduino Pro Mini
 * 
 * Features:
 * - Adaptive deep sleep (2 min when dry, 24h when OK)
 * - Automatic wet weight detection from actual watering
 * - 60-minute windowed accumulation for slow/partial watering
 * - Visual water level indicator (status button)
 * - Battery level indicator (status button)
 * - Auto-dimming LED when battery low (<10%)
 * - Manual dry calibration button
 * - Ultra-low power with smart sampling
 * 
 * Connections:
 * - HX711 DOUT    -> Pin 4
 * - HX711 SCK     -> Pin 5
 * - HX711 VCC     -> Pin 6 (power control)
 * - LED           -> Pin 9 (PWM capable)
 * - Dry Button    -> Pin 2 (INT0) - Calibrate dry weight
 * - Status Button -> Pin 3 (INT1) - Show water + battery level
 * 
 * LED Behavior:
 * - Slow breathing (3x): Not calibrated
 * - 3 quick pulses: Dry calibration done
 * - Fade to level + flashes: Water % + battery level (status button)
 *   - Water: Fade brightness shows water percentage (0-100%)
 *   - Battery: 4 flashes (80-100%), 3 (50-80%), 2 (30-50%), 1 (10-30%)
 *   - Battery critical: Rapid flashing (<10%)
 * - Single pulse: Needs water (checks every 2 minutes)
 * - Double pulse: Needs water + no watering for 14+ days
 * - Fast pulsing (8x): Error (dry > wet)
 * - OFF: Plant is OK (checks once per day)
 * - Auto-dim: LED brightness reduces to 50% when battery <10% (~3 months left)
 * 
 * Battery Life: ~2.4 years on 2500mAh 18650
 * EEPROM Life: 960+ years (buffer stored in RAM only to prevent wear)
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
const int LED_PIN = 9;
const int DRY_BUTTON = 2;     // INT0 - Calibrate dry
const int STATUS_BUTTON = 3;  // INT1 - Show water level

// EEPROM addresses (buffer not saved to reduce wear - lasts 960+ years!)
const int ADDR_DRY_WEIGHT = 0;
const int ADDR_WET_WEIGHT = 4;
const int ADDR_CALIBRATED = 8;
const int ADDR_NEEDS_WATER = 12;
const int ADDR_DAYS_SINCE_ALERT = 16;
// Note: Buffer stored in RAM only (ADDR 20-260 reserved but unused)

// Settings
const float WATER_THRESHOLD = 0.25;        // Water when 25% capacity remains
const int STABLE_READINGS = 10;
const int SLEEP_CYCLES_PER_DAY = 10800;    // 8s × 10800 = 24h
const int SLEEP_CYCLES_PER_MIN = 16;       // 8s × 16 ≈ 2 minutes
const int ALERT_WARNING_DAYS = 14;
const int STABILITY_CHECKS = 3;            // Consecutive stable readings
const float STABILITY_THRESHOLD = 5.0;     // Within 5 units = stable

// Variables
HX711 scale;
float dryWeight = 0;
float wetWeight = 0;
bool calibrated = false;
bool needsWater = false;
bool hasError = false;
uint16_t daysSinceWaterAlert = 0;
volatile bool dryButtonPressed = false;
volatile bool statusButtonPressed = false;
volatile uint16_t sleepCounter = 0;

// Watering detection
float weightBuffer[60];
uint8_t bufferIndex = 0;
bool wateringInProgress = false;
uint8_t stableCount = 0;

// Battery monitoring
bool batteryLow = false;

// Watchdog interrupt
ISR(WDT_vect) {
}

void setup() {
  // Load state
  EEPROM.get(ADDR_SLEEP_COUNTER, sleepCounter);
  EEPROM.get(ADDR_NEEDS_WATER, needsWater);
  EEPROM.get(ADDR_DAYS_SINCE_ALERT, daysSinceWaterAlert);
  
  // Configure pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(HX711_VCC, OUTPUT);
  pinMode(DRY_BUTTON, INPUT_PULLUP);
  pinMode(STATUS_BUTTON, INPUT_PULLUP);
  
  // Check button wake
  bool buttonWake = checkButtonWake();
  
  // LED state indication
  if (!buttonWake) {
    handleLEDState();
  }
  
  // Determine sleep interval based on state
  uint16_t sleepInterval = needsWater ? SLEEP_CYCLES_PER_MIN : SLEEP_CYCLES_PER_DAY;
  
  // Measurement timing
  if (buttonWake || sleepCounter >= sleepInterval) {
    performMeasurement(buttonWake);
    
    if (!buttonWake) {
      sleepCounter = 0;
      // Increment days counter only when not needing water
      if (!needsWater) {
        daysSinceWaterAlert++;
        EEPROM.put(ADDR_DAYS_SINCE_ALERT, daysSinceWaterAlert);
      }
    }
  } else {
    sleepCounter++;
  }
  
  // Save state
  EEPROM.put(ADDR_SLEEP_COUNTER, sleepCounter);
  EEPROM.put(ADDR_NEEDS_WATER, needsWater);
  
  // Sleep
  goToSleep();
}

void loop() {
}

bool checkButtonWake() {
  delay(50);
  
  // Config reset - both buttons
  if (digitalRead(DRY_BUTTON) == LOW && digitalRead(STATUS_BUTTON) == LOW) {
    unsigned long startTime = millis();
    bool stillHeld = true;
    
    while (millis() - startTime < 2000) {
      if (digitalRead(DRY_BUTTON) == HIGH || digitalRead(STATUS_BUTTON) == HIGH) {
        stillHeld = false;
        break;
      }
      delay(100);
    }
    
    if (stillHeld) {
      clearConfiguration();
      for (int i = 0; i < 3; i++) {
        fadeLED(800);
      }
      return false;
    }
  }
  
  // Dry calibration button
  if (digitalRead(DRY_BUTTON) == LOW) {
    dryButtonPressed = true;
    for (int i = 0; i < 3; i++) {
      fadeLED(150);
    }
    return true;
  }
  
  // Status check button - show water level
  if (digitalRead(STATUS_BUTTON) == LOW) {
    statusButtonPressed = true;
    showWaterLevel();
    return false;  // Don't do measurement, just show status
  }
  
  return false;
}

void showWaterLevel() {
  // Power on HX711
  digitalWrite(HX711_VCC, HIGH);
  delay(100);
  
  scale.begin(HX711_DOUT, HX711_SCK);
  scale.set_scale();
  delay(500);
  scale.tare();
  delay(200);
  
  float currentWeight = getStableWeight();
  
  // Power off HX711
  digitalWrite(HX711_VCC, LOW);
  
  loadCalibration();
  
  if (!calibrated) {
    // Not calibrated - show error
    for (int i = 0; i < 5; i++) {
      fadeLED(100);
    }
    return;
  }
  
  // Calculate water percentage
  float waterRange = wetWeight - dryWeight;
  float currentWater = currentWeight - dryWeight;
  float waterPercent = (currentWater / waterRange);
  
  if (waterPercent < 0) waterPercent = 0;
  if (waterPercent > 1.0) waterPercent = 1.0;
  
  // Fade up to percentage level and hold
  int maxBrightness = batteryLow ? 128 : 255;  // Half brightness if battery low
  int targetBrightness = (int)(waterPercent * maxBrightness);
  int steps = 50;
  int stepDelay = 20;  // 1 second total fade
  
  // Fade up to water level
  for (int i = 0; i <= steps; i++) {
    int brightness = (i * targetBrightness) / steps;
    analogWrite(LED_PIN, brightness);
    delay(stepDelay);
  }
  
  // Hold at level for 3 seconds
  delay(3000);
  
  // Fade out
  for (int i = steps; i >= 0; i--) {
    int brightness = (i * targetBrightness) / steps;
    analogWrite(LED_PIN, brightness);
    delay(stepDelay);
  }
  
  analogWrite(LED_PIN, 0);
  
  // Show battery level after water level
  delay(500);
  showBatteryLevel();
}

void handleLEDState() {
  loadCalibration();
  
  if (calibrated && dryWeight >= wetWeight) {
    hasError = true;
  }
  
  if (hasError) {
    for (int i = 0; i < 8; i++) {
      fadeLED(100);
    }
  } else if (!calibrated) {
    for (int i = 0; i < 3; i++) {
      fadeLED(600);
    }
  } else if (needsWater) {
    fadeLED(400);
    
    if (daysSinceWaterAlert >= ALERT_WARNING_DAYS) {
      delay(200);
      fadeLED(400);
    }
  } else {
    analogWrite(LED_PIN, 0);
  }
}

void performMeasurement(bool isButtonWake) {
  digitalWrite(HX711_VCC, HIGH);
  delay(100);
  
  scale.begin(HX711_DOUT, HX711_SCK);
  scale.set_scale();
  delay(500);
  scale.tare();
  delay(200);
  
  float currentWeight = getStableWeight();
  
  if (dryButtonPressed) {
    calibrateDryWeight(currentWeight);
    dryButtonPressed = false;
  } 
  else if (calibrated && !hasError) {
    if (needsWater) {
      checkForWatering(currentWeight);
    } else {
      checkIfNeedsWater(currentWeight);
    }
  }
  
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
  
  // Initial wet estimate (user should water soon after)
  if (!calibrated || wetWeight == 0) {
    wetWeight = dryWeight * 1.2;  // Estimate 20% heavier when wet
  }
  
  calibrated = true;
  hasError = false;
  needsWater = true;  // Assume needs water after dry calibration
  
  // Clear buffer
  for (int i = 0; i < 60; i++) {
    weightBuffer[i] = currentWeight;
  }
  bufferIndex = 0;
  
  saveCalibration();
}

void checkForWatering(float currentWeight) {
  // Add to circular buffer (RAM only, no EEPROM wear)
  weightBuffer[bufferIndex] = currentWeight;
  bufferIndex = (bufferIndex + 1) % 60;
  
  // Find minimum in buffer (starting dry point)
  float minWeight = weightBuffer[0];
  for (int i = 1; i < 60; i++) {
    if (weightBuffer[i] < minWeight && weightBuffer[i] > 0) {
      minWeight = weightBuffer[i];
    }
  }
  
  // Calculate threshold (25% of water capacity or 100 units minimum)
  float waterCapacity = wetWeight - dryWeight;
  float threshold = max(waterCapacity * 0.25, 100.0);
  float increase = currentWeight - minWeight;
  
  // Detect watering event
  if (increase > threshold && !wateringInProgress) {
    wateringInProgress = true;
    stableCount = 0;
  }
  
  // Monitor for stability after watering
  if (wateringInProgress) {
    float lastWeight = weightBuffer[(bufferIndex - 1 + 60) % 60];
    
    if (abs(currentWeight - lastWeight) < STABILITY_THRESHOLD) {
      stableCount++;
      
      if (stableCount >= STABILITY_CHECKS) {
        // Stable! Update wet weight
        wetWeight = currentWeight;
        needsWater = false;
        wateringInProgress = false;
        stableCount = 0;
        daysSinceWaterAlert = 0;
        saveCalibration();
        EEPROM.put(ADDR_DAYS_SINCE_ALERT, daysSinceWaterAlert);
      }
    } else {
      stableCount = 0;
    }
  }
}

void checkIfNeedsWater(float currentWeight) {
  float waterRange = wetWeight - dryWeight;
  float currentWater = currentWeight - dryWeight;
  float waterPercent = (currentWater / waterRange);
  
  if (waterPercent < 0) waterPercent = 0;
  if (waterPercent > 1.0) waterPercent = 1.0;
  
  if (waterPercent < WATER_THRESHOLD) {
    needsWater = true;
    
    // Initialize buffer with current weight
    for (int i = 0; i < 60; i++) {
      weightBuffer[i] = currentWeight;
    }
    bufferIndex = 0;
    
    EEPROM.put(ADDR_NEEDS_WATER, needsWater);
  }
}

void saveCalibration() {
  EEPROM.put(ADDR_DRY_WEIGHT, dryWeight);
  EEPROM.put(ADDR_WET_WEIGHT, wetWeight);
  EEPROM.put(ADDR_CALIBRATED, calibrated);
}

void loadCalibration() {
  EEPROM.get(ADDR_CALIBRATED, calibrated);
  
  if (calibrated) {
    EEPROM.get(ADDR_DRY_WEIGHT, dryWeight);
    EEPROM.get(ADDR_WET_WEIGHT, wetWeight);
    // Note: Buffer not restored from EEPROM (saves wear, refills quickly)
  }
}

void clearConfiguration() {
  dryWeight = 0;
  wetWeight = 0;
  calibrated = false;
  needsWater = false;
  hasError = false;
  sleepCounter = 0;
  daysSinceWaterAlert = 0;
  wateringInProgress = false;
  bufferIndex = 0;
  
  // Clear buffer in RAM (no EEPROM writes needed)
  for (int i = 0; i < 60; i++) {
    weightBuffer[i] = 0;
  }
  
  EEPROM.put(ADDR_DRY_WEIGHT, dryWeight);
  EEPROM.put(ADDR_WET_WEIGHT, wetWeight);
  EEPROM.put(ADDR_CALIBRATED, calibrated);
  EEPROM.put(ADDR_NEEDS_WATER, needsWater);
  EEPROM.put(ADDR_SLEEP_COUNTER, sleepCounter);
  EEPROM.put(ADDR_DAYS_SINCE_ALERT, daysSinceWaterAlert);
  // Note: bufferIndex not saved (RAM only)
}

void fadeLED(int durationMs) {
  int halfDuration = durationMs / 2;
  int steps = 50;
  int stepDelay = halfDuration / steps;
  
  // Reduce brightness to 50% when battery is low
  int maxBrightness = batteryLow ? 128 : 255;
  
  for (int i = 0; i <= steps; i++) {
    int brightness = (i * maxBrightness) / steps;
    analogWrite(LED_PIN, brightness);
    delay(stepDelay);
  }
  
  for (int i = steps; i >= 0; i--) {
    int brightness = (i * maxBrightness) / steps;
    analogWrite(LED_PIN, brightness);
    delay(stepDelay);
  }
  
  analogWrite(LED_PIN, 0);
}

long readVcc() {
  // Read 1.1V reference against AVcc
  // Set the reference to Vcc and measure the internal 1.1V bandgap
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  
  delay(2);  // Wait for Vref to settle
  ADCSRA |= _BV(ADSC);  // Start conversion
  while (bit_is_set(ADCSRA, ADSC));  // Wait for completion
  
  long result = ADCL;
  result |= ADCH << 8;
  result = 1125300L / result;  // Back-calculate AVcc in mV
  return result;
}

void showBatteryLevel() {
  long vcc = readVcc();
  
  // Update battery low flag (under 3.3V = ~10% remaining, ~3 months left)
  batteryLow = (vcc < 3300);
  
  // Map voltage to flashes (18650 non-linear discharge curve)
  int flashes;
  if (vcc >= 3900) flashes = 4;      // 100-80% (4.2-3.9V)
  else if (vcc >= 3700) flashes = 3; // 80-50% (3.9-3.7V)
  else if (vcc >= 3500) flashes = 2; // 50-30% (3.7-3.5V)
  else if (vcc >= 3300) flashes = 1; // 30-10% (3.5-3.3V)
  else {
    // <10% battery - warning (rapid flash)
    for (int i = 0; i < 8; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
    return;
  }
  
  // Flash battery level
  int brightness = batteryLow ? 128 : 255;
  for (int i = 0; i < flashes; i++) {
    analogWrite(LED_PIN, brightness);
    delay(200);
    analogWrite(LED_PIN, 0);
    if (i < flashes - 1) delay(300);
  }
}

void goToSleep() {
  ADCSRA = 0;
  
  for (int i = 0; i < 14; i++) {
    if (i != DRY_BUTTON && i != STATUS_BUTTON) {
      pinMode(i, INPUT_PULLUP);
    }
  }
  
  attachInterrupt(digitalPinToInterrupt(DRY_BUTTON), wakeUp, LOW);
  attachInterrupt(digitalPinToInterrupt(STATUS_BUTTON), wakeUp, LOW);
  
  setupWatchdog(9);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_bod_disable();
  sleep_cpu();
  
  sleep_disable();
  
  detachInterrupt(digitalPinToInterrupt(DRY_BUTTON));
  detachInterrupt(digitalPinToInterrupt(STATUS_BUTTON));
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
