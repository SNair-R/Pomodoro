# Pico Pomodoro Timer

A hardware Pomodoro timer built on a Raspberry Pi Pico W, with an I2C LCD display, a potentiometer for setting durations, a pushbutton for navigation, and a piezo buzzer for alerts.

<img width="3024 / 2" height="4032/ 4" alt="pomodoro_timer" src="https://github.com/user-attachments/assets/f767423c-edab-4d52-b0d8-9ab194a8ba43" />
   
## Overview

This device runs a full 4-cycle Pomodoro session:

1. **Set Study Time** — turn the potentiometer to choose a study duration, press the button to confirm.
2. **Set Chill Time** — same process, for short breaks between study sessions.
3. **Set Relax Time** — same process, for the long break after the 4th study session.
4. The device then automatically runs **4 study/break cycles**:
   - Study → short break (x3)
   - Study → long break (after the 4th study session)
5. A buzzer sounds at the end of each timer, with a distinct pattern for study, short break, and long break.
6. When all cycles finish, the screen prompts for a button press to reset and start over.

The LCD shows the current phase name ("Study Time," "Chill Time," "Relax Time") on the top line and a live MM:SS countdown on the bottom line.

## Hardware

| Component | Details |
|---|---|
| Microcontroller | Raspberry Pi Pico W |
| Display | 16x2 character LCD with PCF8574 I2C backpack (address `0x27`) |
| Input | Potentiometer (duration selection) + tactile pushbutton (confirm/navigate) |
| Output | Piezo buzzer (PWM-driven) |

### Pin Assignments

| Signal | GPIO |
|---|---|
| I2C SDA (LCD) | GPIO 14 |
| I2C SCL (LCD) | GPIO 15 |
| Potentiometer (ADC) | GPIO 28 (ADC2) |
| Button | GPIO 16 |
| Buzzer (PWM) | GPIO 13 |

The LCD is wired directly to 3.3V — the Pico's I2C bus operates at 3.3V, and the PCF8574 backpack is run at the same voltage rather than through a level shifter.

## How Time Selection Works

Each "set time" screen reads the potentiometer via the ADC, maps the raw reading into discrete half-minute steps, and displays the resulting duration live as MM:SS. Turning the dial updates the display in real time; pressing the button locks in the current value and advances to the next screen.

- Study time: up to ~60 minutes, in 30-second increments.
- Chill (short break) time: up to ~30 minutes, in 30-second increments.
- Relax (long break) time: up to ~60 minutes, in 30-second increments — set independently of the other two.

## Timer Logic

Each countdown is driven by the Pico's onboard millisecond clock (`to_ms_since_boot`), comparing elapsed time against a rolling one-second interval rather than relying on `sleep_ms()` for the whole duration. This keeps the LCD responsive and the countdown accurate over long sessions.

## Buzzer

The buzzer is driven via hardware PWM. Each phase transition (end of study, end of short break, end of long break) plays a distinct buzz pattern — number of beeps and duration — so you can tell which phase just ended without looking at the screen.

## Building

This project uses the Raspberry Pi Pico C SDK and CMake, following the standard Pico SDK project structure. Build with the Pico SDK toolchain and flash the resulting `.uf2` file to the Pico W in BOOTSEL mode.

## Files

- `main.c` — main program flow (intro screen, time-selection UI, cycle loop)
- `Pomodoro.c` — LCD driver, timer/countdown logic, buzzer control, button debounce
- `Pomodoro.h` — pin definitions and LCD command constants
- `CMakeLists.txt` — build configuration
