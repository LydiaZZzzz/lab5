#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define RED_LED_PIN    3
#define GREEN_LED_PIN  4
#define BLUE_LED_PIN   5
#define BUTTON_PIN     9

#define SAMPLE_RATE_MS 10
#define CAPTURE_DURATION_MS 1000
#define FEATURE_SIZE 300  // 100 frames * 3 axes
#define CONFIDENCE_THRESHOLD 80.0

Adafruit_MPU6050 mpu;
float features[FEATURE_SIZE];
bool capturing = false;
bool last_button_state = HIGH;
unsigned long last_sample_time = 0;
unsigned long capture_start_time = 0;
int sample_count = 0;

const char* ssid = "huawei";
const char* password = "20010118";
const char* api_endpoint = "http://13.93.144.193:8000/predict";

void show_prediction_color(String label) {
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(BLUE_LED_PIN, LOW);

  if (label == "HorizontalSlash") {
    digitalWrite(RED_LED_PIN, HIGH);
  } else if (label == "VerticalSlash") {
    digitalWrite(GREEN_LED_PIN, HIGH);
  } else if (label == "DiagonalSlash") {
    digitalWrite(BLUE_LED_PIN, HIGH);
  }
}

void send_data_to_server() {
  WiFiClient client;
  HTTPClient http;
  http.begin(client, api_endpoint);
  http.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc(8192);
  JsonArray data = doc.createNestedArray("data");
  for (int i = 0; i < FEATURE_SIZE; i++) {
    data.add(features[i]);
  }

  String jsonStr;
  serializeJson(doc, jsonStr);
  int httpResponseCode = http.POST(jsonStr);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("Prediction response: ");
    Serial.println(response);

    DynamicJsonDocument resDoc(512);
    DeserializationError error = deserializeJson(resDoc, response);
    if (!error) {
      String label = resDoc["gesture"];
      float confidence = resDoc["confidence"];
      Serial.printf("Predicted: %s (%.2f%%)\n", label.c_str(), confidence);
      show_prediction_color(label);
    } else {
      Serial.println("Failed to parse JSON response from server.");
    }
  } else {
    Serial.print("HTTP Error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void local_inference() {
  // Placeholder for actual inference logic
  // Use a real model if embedded, or simulate with fixed result if not
  String predictedLabel = "DiagonalSlash";  // Replace with model result
  float confidence = 76.42;                 // Replace with real model output

  Serial.printf("Local Prediction: %s (%.2f%%)\n", predictedLabel.c_str(), confidence);

  if (confidence < CONFIDENCE_THRESHOLD) {
    Serial.println("Low confidence - sending raw data to server...");
    send_data_to_server();
  } else {
    show_prediction_color(predictedLabel);
  }
}

void capture_accelerometer_data() {
  if (millis() - last_sample_time >= SAMPLE_RATE_MS) {
    last_sample_time = millis();

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    if (sample_count < FEATURE_SIZE / 3) {
      int idx = sample_count * 3;
      features[idx] = a.acceleration.x;
      features[idx + 1] = a.acceleration.y;
      features[idx + 2] = a.acceleration.z;
      sample_count++;
    }

    if (millis() - capture_start_time >= CAPTURE_DURATION_MS) {
      capturing = false;
      Serial.println("Capture complete, local inference...");
      local_inference();
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050");
    while (1) delay(10);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("MPU6050 ready. Press button to start.");
}

void loop() {
  bool current_button_state = digitalRead(BUTTON_PIN);

  if (last_button_state == HIGH && current_button_state == LOW) {
    Serial.println("Button pressed! Start capture...");
    sample_count = 0;
    capturing = true;
    capture_start_time = millis();
    last_sample_time = millis();
  }

  last_button_state = current_button_state;

  if (capturing) {
    capture_accelerometer_data();
  }
}