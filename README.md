ESPHome LD2450 mmWave Radar Sensor Custom Component

## Installation
Set wifi_ssid and wifi_password in your esphome's secrets.yaml first

1. Place ld2450_uart.h into the components of your esphome configuration folder
2. Create new device with the yaml in this repository

## Wiring
ESP8266  | | LD2450
---------|-|-------|
5V      |<->| VCC
GND     |<->| GND
TX      |<->| RX
RX      |<->| TX

## Notice
1. The RX, TX need pull-up resister.
2. Need to upgrade after firmware 1.1.230605.
3. Use hardware serial if use ESP8266.
4. The commands only work on firmware 2.02.23090617.
5. Do not update to the firmware 2.0.4.23101915 if you want to use the component.
