#include <WiFiS3.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

// WiFi credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";

// Networking
WiFiServer server(80);
WiFiClient client;

// ADS1115
Adafruit_ADS1115 ads;
const float ADS_VOLTAGE_RANGE = 4.096;
const float VOLTAGE_DIVIDER_RATIO = 1.0;

// Velocity
float voltage = 0.0;
int velocity = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  Wire.begin();
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS1115!");
    while (1);
  }
  ads.setGain(GAIN_EIGHT); // Suitable for ±0.512V range
}

void loop() {
  client = server.available();

  if (client) {
    Serial.println("Client connected");

    while (client.connected() && !client.available()) {
      delay(1);
    }

    String request = client.readStringUntil('\r');
    Serial.println("Request: " + request);
    client.read();

    if (request.indexOf("GET /v") >= 0) {
      readSensor();
      String json = "{\"velocity\":" + String(velocity) + "}";

      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json");
      client.println("Access-Control-Allow-Origin: *");
      client.println("Connection: close");
      client.println();
      client.println(json);
    } else {
      client.println("HTTP/1.1 404 Not Found");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println("Not found");
    }

    delay(1);
    client.stop();
    Serial.println("Client disconnected");
  }
}

void readSensor() {
  int16_t adc0 = ads.readADC_SingleEnded(0);
  voltage = (adc0 * ADS_VOLTAGE_RANGE) / 32768.0 * VOLTAGE_DIVIDER_RATIO;
  velocity = (int) mapVoltageToVelocity(voltage);

  Serial.print("Voltage: ");
  Serial.print(voltage, 4);
  Serial.print(" V\tVelocity: ");
  Serial.println(velocity);
}

float mapVoltageToVelocity(float v) {
  const float minV = 0.72;
  const float maxV = 1.03;

  if (v < minV)  return 34;
  if (v < 0) return 24;

  float norm = (v - maxV) / (minV - maxV);
  float curved = sqrt(norm);
  return 24 + curved * (40 - 24);
}