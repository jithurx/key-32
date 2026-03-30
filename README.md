# Key32 ESP32 Gaming Keychain

An ESP32-based retro gaming keychain project featuring a custom startup sequence and an interactive game selection menu plotted on an SSD1306 OLED display.

## Features

- Custom startup logo and spinning star boot animation (3 seconds)
- Interactive game selection menu with visual cursor highlighting
- Configurable push-button directional and action UI controls
- Non-blocking state-machine architecture for the display loops

## Hardware Requirements

- ESP32 Development Board
- SSD1306 OLED Display (128x64 pixels)
- 5x Push Buttons (Tactile switches)
- Jumper wires

## Wire Connections

Connect the OLED display to the ESP32 using the following I2C pins:

| SSD1306 OLED Pin | ESP32 Pin | Description |
| ---------------- | --------- | ----------- |
| VCC | 3.3V | Power (3.3V recommended) |
| GND | GND | Ground |
| SCL (Clock) | GPIO 22 | I2C Clock |
| SDA (Data) | GPIO 21 | I2C Data |

### Menu Switches

Connect your 5 navigational push buttons between the listed GPIO pins and GND. The internal pull-up resistors are enabled, so no external resistors are required.

| Switch | ESP32 Pin |
| ------ | --------- |
| UP | GPIO 32 |
| DOWN | GPIO 33 |
| LEFT | GPIO 25 |
| RIGHT | GPIO 26 |
| SELECT | GPIO 27 |

*Note: Depending on your specific ESP32 board, the location of 3.3V and GND might vary, but I2C pins 21 and 22 are the standard hardware defaults for SDA and SCL respectively.*

## Usage

1. Install the required libraries in the Arduino IDE:
   - `Adafruit GFX Library`
   - `Adafruit SSD1306`
2. Open the `key32/key32.ino` file in the Arduino IDE.
3. Select your exact ESP32 board model and appropriate COM port.
4. Compile and upload the sketch to the ESP32. Set your serial monitor to **115200 baud** to see any error messages (if the display fails to allocate memory).
