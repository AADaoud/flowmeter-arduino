# Flow Sensor HTTP Server

This firmware runs on a WiFi-enabled microcontroller and serves real-time flow velocity data from a magnetohydrodynamic (MHD) sensor over HTTP. It reads analog signals from an ADS1115 ADC and converts the measured voltage into an estimated flow velocity, which can be accessed via a simple API. This setup was used as part of a university project focused on fluid sensing through a 3D-printed Halbach array.

## Features

* Connects to a WiFi network and starts an HTTP server
* Reads differential voltage from ADS1115 via I²C
* Maps voltage to approximate flow velocity using a calibration function
* Serves data as JSON at `/v` endpoint
* Includes CORS headers for integration with mobile and web apps

## Hardware Requirements

* WiFi-capable microcontroller (e.g. Arduino Uno R4 WiFi or ESP32)
* Adafruit ADS1115 analog-to-digital converter
* MHD flow sensing setup (magnetic ring + electrodes + fluid loop)
* Power supply and I²C wiring

## Wiring

* ADS1115 SDA → Microcontroller SDA
* ADS1115 SCL → Microcontroller SCL
* VDD and GND connected appropriately
* A0 input connected to the voltage output of the sensing electrodes

## Network Behavior

The microcontroller connects to a WiFi network on startup and begins serving HTTP requests on port 80. A request to `/v` returns a JSON object with the current estimated velocity:

```
GET http://<device_ip>/v
```

Example response:

```json
{
  "velocity": 28.42
}
```

## Setup Instructions

1. Install the [Arduino IDE](https://www.arduino.cc/en/software)
2. Install the required libraries:

   * `WiFiS3` (for Uno R4 WiFi) or `WiFi` (for ESP32)
   * `Adafruit ADS1X15`
3. Connect your hardware as described
4. Update the following lines in the sketch with your WiFi credentials:

   ```cpp
   const char* ssid = "your_SSID";
   const char* password = "your_password";
   ```
5. Upload the sketch to your board
6. Open the Serial Monitor at 115200 baud to see connection status and live readings

## Calibration Notes

The `mapVoltageToVelocity()` function is used to approximate flow velocity from the measured voltage. These parameters should be adjusted based on experimental calibration with known flow rates:

```cpp
const float minV = 0.72;
const float maxV = 1.03;
```

## Companion App

This server is designed to work with a mobile Flutter app that displays flow velocity data in real time. The app connects to the microcontroller over WiFi and polls the `/v` endpoint.

## License

This project is released under the MIT License. Use and modify freely for academic or prototyping purposes.
