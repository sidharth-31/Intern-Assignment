# ESP32 Heater Controller with DHT22 Sensor and Relay

## Overview

This project is an ESP32 based heater controller using a DHT22 temperature and humidity sensor and a relay module. The system uses a simple state machine to control the heater based on ambient temperature and safety overheat protection.

### Features

- Reads temperature from DHT22 sensor.
- Controls a relay to switch a heater ON/OFF.
- Five states: IDLE, HEATING, STABILIZING (with LED blink), TARGET_REACHED, and OVERHEAT (emergency shutdown).
- Visual indicators using two LEDs:
  - Green LED for system status (ON while heating, blinking while stabilizing, OFF otherwise).
  - Red LED for constant ON during overheating.
- Serial Monitor debugging output of state and temperature.
- Safety overheat cutoff.

## Hardware Connections

| Component    | ESP32 Pin | Notes                                       |
|--------------|------------|---------------------------------------------|
| DHT22 DATA   | GPIO 21    | Use 10kΩ pull-up resistor between DATA and VCC if needed. |
| Relay IN     | GPIO 22    | Controls relay; active LOW to turn ON       |
| Green LED    | GPIO 19    | Status LED                                  |
| Red LED      | GPIO 18    | Overheat indicator                          |
| Power & GND  | 3.3V/5V, GND | Ensure common ground and correct voltage powering. |

## Software Setup

- Install [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32) in your Arduino IDE.
- Install the [DHT sensor library by Adafruit](https://github.com/adafruit/DHT-sensor-library).
- Load `heater_controller.ino` into the Arduino IDE.
- Adjust pin definitions or temperature thresholds in the code if required.
- Upload to ESP32 board.

## Usage

- The heater will turn ON when temperature falls below 20°C.
- Heater turns OFF when temperature reaches or exceeds 25°C.
- When temperature is near the target range (24-25°C), the green LED blinks indicating the stabilizing state.
- If temperature rises above 30°C, the heater shuts off and the red LED turns ON as an overheat warning.
- Serial output shows real-time temperature and state transitions for debugging.

## Safety Warning

- This code controls a relay switching mains or high-power loads; ensure relay rating matches your heater specs.
- Be cautious with high voltages and loads; perform testing on a safe and isolated environment.
- Relay modules often need external power (5V) and proper common ground with ESP32.


