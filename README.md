# AdaptiveMicroclimate_Irrigation
LoRa-connected farm tech that auto-waters based on soil and weather conditions.

## TL;DR
- What: Low-power sensor nodes (C) that send soil/weather telemetry over LoRa to a Python gateway/server which decides when to water and issues valve commands.
- Who: Hobbyists, small farms, researchers building low-cost adaptive irrigation.
- Quick: Flash firmware → run gateway → configure thresholds/API token.

## Features
- Soil moisture, battery, optional temp/humidity telemetry.
- LoRa (SX127x family) radio comms: node → gateway → server.
- Server rule engine: soil threshold + weather/forecast + safety constraints.
- Power-efficient sleep mode for nodes.
- Simple JSON API and simulation helpers.

## Supported languages
- C (firmware) — ~77%
- Python (gateway/server) — ~23%

## Hardware (replace placeholders)
- MCU: (e.g., ESP32 / Arduino Pro Mini / STM32) — set in firmware config
- LoRa radio: SX1276/SX1278 (adjust freq, SF in firmware)
- Soil moisture sensor (analog or digital)
- Valve: solenoid + relay or valve driver
- Power: battery ± solar charge controller

## Quickstart — Firmware (device)
1. Edit board & radio settings:
   - Open firmware/config.h or firmware/boards/<board>.h and set MCU, pins, frequency, spreading factor, and node ID.
2. Build & flash (examples):
   - PlatformIO:
     ```
     cd firmware
     pio run -e <env> -t upload
     ```
   - Make / avr:
     ```
     cd firmware
     make BOARD=<board>
     make flash
     ```
3. Behavior:
   - Node wakes, reads sensors, sends compressed telemetry packet over LoRa, sleeps. It accepts simple control packets to open valve for N seconds.

## Quickstart — Gateway / Server (Python)
1. Create venv and install:
