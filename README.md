# WaterlevelLogger
ESP based microcontroller to monitor water level with beautiful UI


## Getting Started

### Prerequisites

Before you begin, ensure you have the following libraries installed in your Arduino IDE:

- `ESP8266WiFi.h`
- `ESP8266WebServer.h`
- `FS.h`
- `LittleFS.h`
- `ArduinoJson.h`
- `ESP8266HTTPClient.h`


### Circuit Diagram
The connections between the ESP8266 and the HC-SR04 ultrasonic distance sensor.


| ESP8266 Pin | HC-SR04 Pin |
|-------------|-------------|
| 3V3         | VCC         |
| GND         | GND         |
| GPIO 12     | TRIGPIN     |
| GPIO 14     | ECHOPIN     |


## Installation

1. Go to the https://github.com/YeetTheAnson/WirelessVoltmeter page and download the latest file.
2. Open the `.ino` file in Arduino IDE.
3. Set the configuration by changing the SSID and password in the code.
4. Add ESP8266 to the board manager:
    - Go to `File` >> `Preferences` >> `Additional Boards Manager URLs`
    - Paste the following URL: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
5. Select the board `NodeMCU 1.0 (ESP-12E Module)`.
6. Flash the code to the ESP8266.

