# Key32 Boot Animation

An Arduino sketch to display a boot animation on an SSD1306 OLED display using an ESP32.

## Hardware Requirements

- ESP32 Development Board
- SSD1306 OLED Display (128x64 pixels)
- Jumper wires

## Wire Connections

Connect the OLED display to the ESP32 using the following I2C pins:

| SSD1306 OLED Pin | ESP32 Pin | Description |
| ---------------- | --------- | ----------- |
| VCC | 3.3V | Power (3.3V recommended) |
| GND | GND | Ground |
| SCL (Clock) | GPIO 22 | I2C Clock |
| SDA (Data) | GPIO 21 | I2C Data |

*Note: Depending on your specific ESP32 board, the location of 3.3V and GND might vary, but I2C pins 21 and 22 are the standard hardware defaults for SDA and SCL respectively.*

## Usage

1. Install the required libraries in the Arduino IDE:
   - `Adafruit GFX Library`
   - `Adafruit SSD1306`
2. Open the `boot/boot.ino` file in the Arduino IDE.
3. Select your exact ESP32 board model and appropriate COM port.
4. Compile and upload the sketch to the ESP32. Set your serial monitor to **115200 baud** to see any error messages (if the display fails to allocate memory).
