# Low-Power Plant Weight Sensor

Arduino Pro Mini plant monitor using HX711 load cell. Measures weight once per day with deep sleep for ultra-low power consumption.

## Features

- **Deep Sleep**: Wakes once per day, ~5µA sleep current
- **Button Calibration**: Two-point calibration (dry/wet)
- **Growth Tracking**: Automatically adjusts for plant growth
- **LED Indicators**: Visual status without serial monitor
- **Power Control**: HX711 powered via digital pin

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
  Dry Button: Pin 2 -> GND (INT0)
  Wet Button: Pin 3 -> GND (INT1)
  (External 10kΩ pull-up resistors to VCC)
```

## LED Behavior

| State | LED Pattern |
|-------|-------------|
| Not Calibrated | Slow breathing (3 pulses, 600ms each) |
| Button Pressed | 3 quick pulses (150ms each) |
| Config Cleared | 3 slow pulses (800ms each) |
| Error (dry ≥ wet) | Fast pulsing (8 pulses, 100ms each) |
| Needs Water | Single gentle pulse (400ms) every wake |
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

1. **Calibrate Dry**:
   - Place plant on scale when soil is DRY
   - Press DRY button (Pin 2)
   - LED flashes 3 times (confirmation)

2. **Calibrate Wet**:
   - Water plant thoroughly
   - Wait for drainage to stop
   - Press WET button (Pin 3)
   - LED flashes 3 times (confirmation)

3. **Clear Configuration**:
   - Press and hold BOTH buttons for 2 seconds
   - LED flashes 5 times slowly (confirmation)
   - All calibration data cleared
   - Recalibrate from step 1

### Operation

- Device wakes every 8 seconds to check buttons
- Performs measurement once per 24 hours
- If water level < 25%, LED flashes at each wake cycle
- Buttons can wake device anytime for recalibration

### Growth Compensation

When measured weight exceeds wet weight:
- Calculates growth: `growth = current - wet`
- Updates all reference weights automatically
- Saves new calibration to EEPROM
- Sanity check: growth must be < 200g

## Power Consumption

| State | Current | Duration |
|-------|---------|----------|
| Deep Sleep | ~5 µA | 99.9% |
| Measurement | ~15 mA | ~2s/day |
| **Average** | **~8 µA** | **Per day** |

**Battery Life** (CR2032 220mAh):
- Theoretical: ~3 years
- Practical: ~1-2 years

## Troubleshooting

**LED constant ON**:
- Not calibrated yet
- Press DRY then WET buttons

**LED fast flashing**:
- Error: dry weight ≥ wet weight
- Recalibrate both points

**LED not responding to buttons**:
- Check button wiring to pins 2 and 3
- Verify buttons connect to GND

**No weight readings**:
- Check HX711 wiring (especially DOUT/SCK)
- Verify Pin 6 powers HX711
- Check load cell connections (red/black/white/green)

## Customization

Adjust these constants in code:

```cpp
const float WATER_THRESHOLD = 0.25;  // 0.25 = water at 25%
const int SLEEP_CYCLES_PER_DAY = 10800;  // Adjust measurement frequency
const int STABLE_READINGS = 10;  // More = slower but more stable
```

## How It Works

1. **Wake Cycle**: Device wakes every 8 seconds via watchdog timer
2. **Button Check**: Checks if either button pressed (interrupt)
3. **Daily Measurement**: Every 10,800 wake cycles (24h), takes reading
4. **Weight Analysis**: Compares to dry/wet references
5. **LED Update**: Shows status via LED pattern
6. **Sleep**: Returns to deep sleep mode

## License

Open source - use freely for your plant projects!
