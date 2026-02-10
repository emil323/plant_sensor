# Low-Power Plant Weight Sensor

[![Arduino CI](https://github.com/emil323/plant_sensor/workflows/Arduino%20CI/badge.svg)](https://github.com/emil323/plant_sensor/actions)

Arduino Pro Mini plant monitor using HX711 load cell. Adaptive sampling (2 min when dry, 24h when OK) with automatic watering detection.

## Features

- **Adaptive Deep Sleep**: 2 min when dry, 24h when OK (15-28 year battery life)
- **Automatic Watering Detection**: No button press needed after watering
- **60-Minute Windowed Accumulation**: Handles slow/interrupted watering
- **Battery Level Indicator**: Visual battery status (1-4 flashes)
- **Auto-Dimming**: LED brightness reduces when battery low
- **Manual Dry Calibration**: Single button press to set dry weight
- **Water Level Display**: Press status button to see water percentage
- **LED Visual Feedback**: All status shown via LED patterns
- **Power Control**: HX711 powered via digital pin for ultra-low sleep current

## Hardware

- Arduino Pro Mini (3.3V or 5V)
- HX711 Load Cell Amplifier
- Load Cell (1-5kg capacity)
- LED + 220Ω resistor
- 2x Push buttons
- 2x 10kΩ resistors (pull-up for buttons)

## Wiring

```
HX711:
  E+    -> Load Cell Red
  E-    -> Load Cell Black
  A+    -> Load Cell Green
  A-    -> Load Cell White
  GND   -> Arduino GND
  VCC   -> Arduino Pin 6 (power control)
  DOUT  -> Arduino Pin 4
  SCK   -> Arduino Pin 5

LED:
  Anode  (+) -> Arduino Pin 9 + 220Ω resistor
  Cathode(-) -> GND
  (Pin 9 is PWM capable for smooth fading)

Buttons:
  Dry Button:   Pin 2 -> GND (INT0) - Calibrate dry weight
  Status Button: Pin 3 -> GND (INT1) - Show battery/water level
  (External 10kΩ pull-up resistors to VCC)
```

## LED Behavior

| State | LED Pattern |
|-------|-------------|
| Not Calibrated | Slow breathing (3 pulses, 600ms each) |
| Button Pressed | 3 quick pulses (150ms each) |
| Config Cleared | 3 slow pulses (800ms each) |
| Needs Water | Single gentle pulse (400ms) every wake |
| Water Alert (14+ days) | Double pulse (400ms each) every wake |
| OK | OFF |

All LED patterns use smooth PWM fading for a polished appearance.

## Usage

### Initial Setup

1. **Install HX711 Library**:
   - Arduino IDE → Sketch → Include Library → Manage Libraries
   - Search "HX711" → Install by Bogdan Necula

2. **Upload Code**:
   - Select Board: Arduino Pro or Pro Mini
   - Select Processor: ATmega328P (3.3V 8MHz or 5V 16MHz)
   - Upload sketch

3. **Power Optimization** (Optional):
   - Desolder power LED on Pro Mini (saves ~3mA)
   - Use 3.3V version for lower power

### Calibration

1. **Calibrate Dry** (One-time setup):
   - Place plant on scale when soil is DRY
   - Press DRY button (Pin 2)
   - LED flashes 3 times (confirmation)

2. **Water Normally** (Automatic):
   - Water your plant as usual (any method works!)
   - Device automatically detects watering and learns wet weight
   - No button press needed!
   - Handles: fast pour, slow pour, bottom watering, interrupted watering

3. **Check Status** (Anytime):
   - Press STATUS button (Pin 3) to see:
     - Battery level: 4 flashes (100%) → 1 flash (10%) → rapid (low)
     - Water percentage display

4. **Clear Configuration** (Rare):
   - Press and hold BOTH buttons for 2 seconds
   - LED flashes slowly (confirmation)
   - All calibration data cleared
   - Recalibrate from step 1

### Operation

**Adaptive Sampling for Maximum Battery Life:**
- When DRY (needs water): Measures every 2 minutes (monitors for watering)
- When OK (recently watered): Measures every 24 hours (deep sleep mode)
- Automatically switches between modes based on soil moisture
- Buttons can wake device anytime

**Automatic Watering Detection:**
- 60-minute rolling buffer tracks recent weight measurements
- Detects watering when weight increases significantly
- Works with any watering method - no button press needed!
- After watering detected, device waits for soil to stabilize
- Once stable for 3 readings, updates wet weight reference

**Alerts:**
- When water level < 25%: LED pulses gently at each wake
- When battery < 10%: LED brightness automatically reduces by half

### Automatic Weight Updates

**Watering Detection:**
- Monitors 60-minute history for significant weight increase
- Threshold: max(25% of water capacity, 100 units)
- Waits for stability (3 consecutive readings within 5 units)
- Updates wet weight and saves to EEPROM

**Plant Growth Compensation:**
- When measured weight exceeds wet weight
- Calculates growth: `growth = current - wet`
- Updates all reference weights automatically
- Saves new calibration to EEPROM
- Sanity check: growth must be < 200g

## Power Consumption

| State | Current | Duration | Notes |
|-------|---------|----------|-------|
| Deep Sleep | ~5 µA | 99%+ | ATmega328P + HX711 off |
| Measurement | ~15 mA | ~2-3s | HX711 powered via Pin 6 |
| **Dry Mode** | **2.84 mAh/day** | **When needs water** | Measures every 2 min |
| **OK Mode** | **0.13 mAh/day** | **After watering** | Measures every 24h |

**Battery Life** (Realistic Usage):

Watering frequency varies by plant, season, and conditions. Here's what to expect with **unprotected cells** (no parasitic protection circuit drain):

| Plant Type | Watering Frequency | 2000mAh | 2500mAh | 3500mAh |
|------------|-------------------|---------|---------|---------|
| **Thirsty plants** | 2×/week (e.g., ferns) | 15 years | 19 years | 27 years |
| **Average houseplant** | 1×/week (most common) | **23 years** | **28 years** | 40 years |
| **Slow-drying plants** | 1×/10 days | 24 years | 30 years | 41 years |
| **Succulents/cacti** | 1×/2 weeks | 27 years | 34 years | 47 years |

**Important:** Using **protected** cells adds 10-50 µA parasitic drain, cutting battery life by 50-80%! The TP4056 charger module already provides protection, so unprotected cells are better.

**Key insight:** Device only uses high power when plant actually needs water (~4% of time for typical plant). The rest of the time (96%) it's in ultra-efficient 24h check mode!

**In practice:** Battery will likely outlast the electronics. Plan to replace battery every 10-15 years as a maintenance item, not because it's depleted.

**Recommended Battery:**
- **Unprotected** 18650 Li-ion cell (2000-2500mAh recommended)
- Examples: Samsung 25R, Sony VTC6, LG HG2
- Chemistry: Standard Li-ion (3.0-4.2V)
- **Why unprotected?** The TP4056 charger module already provides protection (over-charge, over-discharge, short-circuit). Protected cells add a parasitic drain (10-50 µA) that can cut battery life in half!
- **CRITICAL:** Must remove Pro Mini power LED for stated battery life!

## Troubleshooting

**LED breathing pattern**:
- Not calibrated yet (only dry weight needed)
- Press DRY button to calibrate
- Then water plant normally - auto-detects!

**LED single pulse (every 2 minutes)**:
- Plant needs water
- Water your plant using any method
- Device will auto-detect and update

**LED double pulse (every 2 minutes)**:
- Plant needs water AND hasn't been watered for 14+ days
- Extra reminder in case you missed earlier alerts

**LED not responding to buttons**:
- Check button wiring (Pin 2: DRY, Pin 3: STATUS)
- Verify buttons connect to GND when pressed
- Check 10kΩ pull-up resistors to VCC

**No weight readings**:
- Check HX711 wiring (especially DOUT/SCK)
- Verify Pin 6 powers HX711
- Check load cell connections (red/black/white/green)

## Customization

Adjust these constants in code:

```cpp
const float WATER_THRESHOLD = 0.25;  // 0.25 = water at 25%
const int SLEEP_CYCLES_PER_MIN = 16;  // Dry mode: 8s × 16 = 2 min
const int SLEEP_CYCLES_PER_DAY = 10800;  // OK mode: 8s × 10800 = 24h
const int STABLE_READINGS = 10;  // Readings needed before updating wet weight
float weightBuffer[60];  // 60-minute rolling buffer for watering detection
```

## How It Works

1. **Wake Cycle**: Device wakes every 8 seconds via watchdog timer
2. **Button Check**: Checks if either button pressed (interrupt-based)
3. **Adaptive Measurement**: 
   - DRY mode: Every 16 cycles (~2 min) - actively monitors for watering
   - OK mode: Every 10,800 cycles (~24h) - conserves battery
4. **Watering Detection**: 
   - Maintains 60-minute rolling buffer of weights
   - Finds minimum weight in buffer (dry starting point)
   - Detects significant weight increase (>25% capacity or >100 units)
   - Waits for stability before updating wet weight
5. **Weight Analysis**: Compares current weight to dry/wet references
6. **LED Update**: Shows status via smooth PWM-faded LED patterns
7. **Deep Sleep**: ATmega328P sleep mode, HX711 powered off via Pin 6
8. **Battery Monitoring**: Measures VCC using internal 1.1V bandgap reference

## License

Open source - use freely for your plant projects!
