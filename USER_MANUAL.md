# Plant Sensor User Manual

**Welcome to your intelligent plant monitoring system!** This device automatically tracks your plant's water needs and alerts you when it's time to water. No apps, no WiFi - just simple LED indicators.

---

## Table of Contents
1. [What You Have](#what-you-have)
2. [First Time Setup](#first-time-setup)
3. [Daily Use](#daily-use)
4. [Understanding LED Patterns](#understanding-led-patterns)
5. [Checking Status](#checking-status)
6. [Troubleshooting](#troubleshooting)
7. [Battery & Maintenance](#battery--maintenance)

---

## What You Have

Your plant sensor includes:
- Main sensor unit with load cell
- LED indicator (visible from top)
- Two buttons:
  - **DRY button** (left/pin 2)
  - **STATUS button** (right/pin 3)
- Battery compartment (18650 Li-ion cell recommended)
- USB charging port (TP4056 charger module)

---

## First Time Setup

### Step 1: Install Battery
1. Insert fully charged 18650 Li-ion battery (recommended: Samsung INR18650-35E Protected)
2. Observe correct polarity (+ and -)
3. LED should start breathing slowly (3 pulses) - this means it's ready to calibrate

### Step 2: Position Your Plant
1. Place the sensor on a stable, level surface
2. Center your potted plant on the load cell platform
3. Make sure the plant won't move or shift

### Step 3: Calibrate Dry Weight
**⚠️ IMPORTANT: Only do this when soil is DRY!**

1. Wait until your plant's soil is completely dry (ready for watering)
2. Press the **DRY button** once
3. LED flashes 3 quick times (confirms calibration saved)
4. LED will now pulse once every 2 minutes (needs water)

### Step 4: Water Your Plant
1. Water your plant normally using your preferred method:
   - Top watering (fast or slow pour)
   - Bottom watering
   - Multiple watering sessions - all work!
2. **No button press needed!** The sensor automatically detects watering
3. After watering is detected and soil stabilizes, LED turns OFF
4. Your sensor is now fully set up! ✅

---

## Daily Use

**Good news: You don't need to do anything!** The sensor monitors your plant automatically.

### Normal Operation

**When Plant is OK:**
- LED stays **OFF** (plant has enough water)
- Sensor checks weight once per day (saves battery)
- Battery lasts ~2.4 years before needing replacement

**When Plant Needs Water:**
- LED pulses **once** every 2 minutes (gentle reminder)
- Sensor checks weight every 2 minutes (watching for watering)
- Just water your plant normally - sensor auto-detects!
- LED turns off automatically after watering detected

**Long-term Neglect Warning:**
- If plant needs water for 14+ days: LED pulses **twice** every 2 minutes
- Extra reminder in case you missed earlier alerts

### How Watering Detection Works

The sensor is smart! It:
- Tracks weight over the last 60 minutes
- Detects when weight increases significantly (watering!)
- Waits for soil to stabilize and excess water to drain
- Automatically updates its reference points
- No button press needed from you!

---

## Understanding LED Patterns

| What You See | What It Means | What To Do |
|--------------|---------------|------------|
| **3 slow breaths** (600ms each) | Not calibrated yet | Press DRY button when soil is dry |
| **OFF** | Plant is OK! | Nothing - enjoy! ☕ |
| **1 pulse** every 2 min | Plant needs water | Water your plant (any method) |
| **2 pulses** every 2 min | Needs water for 14+ days | Water your plant soon! |
| **3 quick flashes** | Button press confirmed | You just pressed a button |
| **3 slow pulses** (800ms) | Config cleared | Start over - press DRY when soil dry |
| **LED dimmed** (50% brightness) | Battery low (<10%) | Replace/recharge battery soon (~3 months left) |

**TIP:** All LED patterns use smooth fading - never harsh blinking!

---

## Checking Status

### Check Battery Level
1. Press the **STATUS button** (right button)
2. Count the flashes:
   - **4 flashes** = 80-100% battery (excellent)
   - **3 flashes** = 50-80% battery (good)
   - **2 flashes** = 30-50% battery (okay)
   - **1 flash** = 10-30% battery (low - consider replacing soon)
   - **Rapid flashing** = <10% battery (replace within 1-3 months)

### Check Water Level
Press and hold the **STATUS button** for 1+ second:
- LED brightness shows water percentage
- Brighter = more water
- Dimmer = less water
- OFF = needs watering

---

## Troubleshooting

### LED breathing constantly (3 slow pulses)
**Problem:** Not calibrated yet

**Solution:**
1. Wait until soil is completely DRY
2. Press DRY button once
3. Water plant normally when needed

---

### LED won't turn off after watering
**Problem:** Sensor hasn't detected stable weight yet

**Solution:**
- Wait 10-30 minutes after watering for excess water to drain
- Make sure plant isn't being moved or bumped
- Check that load cell is on stable surface
- If still not working after 2 hours, plant might need more water

---

### LED never comes on (even when plant is dry)
**Problem:** Wet weight reference not set yet

**Solution:**
- This happens if you calibrated dry but never watered enough
- Water plant thoroughly (until you see drainage)
- Wait for drainage to stop (~30 min)
- Sensor will auto-detect and learn wet weight
- LED will turn off once wet weight is learned

---

### Buttons don't respond
**Problem:** Button wiring or power issue

**Solution:**
1. Check battery is installed correctly
2. Try pressing firmly and holding 1 second
3. If battery level shows but DRY button doesn't work, sensor is working - just needs harder press
4. Check for 50ms debounce (don't press too quickly)

---

### Want to start over / recalibrate
**Problem:** You moved the plant or changed pots

**Solution:**
1. Press and hold **BOTH buttons** for 2 seconds
2. LED flashes 3 slow pulses (confirms reset)
3. Start from Step 3 in [First Time Setup](#step-3-calibrate-dry-weight)

---

## Battery & Maintenance

### Battery Life
- **Normal operation:** ~2.4 years on 3500mAh battery
- **When dry mode:** ~2.4 years (device checks more often, but still efficient!)
- **When OK mode:** 52+ years (theoretical, checks once per day)

### When to Replace Battery
- LED automatically dims to 50% when battery <10%
- This gives you ~3 months warning
- STATUS button shows battery level (see [Checking Status](#check-battery-level))

### Recommended Battery
- **Samsung INR18650-35E (Protected)** - 3500mAh
- Must be "protected" version (has built-in safety circuit)
- Button-top preferred for better contact
- Standard Li-ion chemistry (3.0-4.2V)

### Charging
- Plug USB cable into charging port
- Red LED on charger: Charging
- Green LED on charger: Fully charged
- Can leave plant on sensor while charging (no interruption!)

### Power Optimization
For maximum battery life, the Arduino Pro Mini's power LED should be removed (desoldered). This saves ~2mA and is critical for achieving 2+ year battery life. If not removed, expect ~6 months battery life instead.

### Cleaning
- Wipe load cell platform with damp cloth
- Don't submerge sensor in water
- Keep electronics dry
- Clean regularly to prevent soil buildup affecting accuracy

### Plant Growth
- Sensor automatically compensates for plant growth!
- As plant grows heavier, sensor updates its references
- No recalibration needed over time
- Max growth compensation: 200g at once

---

## Technical Specifications

| Feature | Specification |
|---------|---------------|
| **Sleep Current** | ~5 µA (ultra-low power) |
| **Measurement Current** | ~15 mA for 2-3 seconds |
| **Battery Life** | ~2.4 years (3500mAh) |
| **Sampling (Dry)** | Every 2 minutes |
| **Sampling (OK)** | Every 24 hours |
| **Load Cell Capacity** | 1-5kg (depends on model) |
| **Weight Accuracy** | ±5g (stable readings) |
| **Watering Detection** | 60-minute rolling buffer |
| **Operating Voltage** | 3.0-4.2V (Li-ion) |
| **Low Battery Threshold** | 3.3V (~10% remaining) |

---

## Tips for Best Results

✅ **DO:**
- Calibrate DRY when soil is actually bone dry
- Use stable, level surface for sensor
- Keep plant centered on load cell
- Water normally - sensor handles any method!
- Remove Arduino power LED for long battery life

❌ **DON'T:**
- Calibrate dry on wet/damp soil
- Move plant around on sensor
- Bump or jostle the sensor
- Worry about watering method (all work!)
- Expect instant LED updates (waits for stability)

---

## Need Help?

If you're experiencing issues not covered in this manual:
1. Try clearing configuration (both buttons, 2 seconds)
2. Recalibrate from scratch
3. Check battery level
4. Verify load cell is on stable surface
5. Make sure plant isn't being moved or vibrated

Still stuck? Check the technical README.md or raise an issue on GitHub!

---

**Enjoy your low-maintenance plant care! 🌱**

Your plant sensor works silently in the background, watching over your plant so you don't have to. When the LED pulses, water your plant. When it's off, relax!
