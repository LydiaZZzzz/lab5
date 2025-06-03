# TECHIN515 Lab 5 – Edge-Cloud Offloading

This project implements an edge-cloud offloading strategy using an ESP32 microcontroller and a gesture recognition system. The ESP32 performs local inference and offloads data to a cloud server hosted on Microsoft Azure when the local prediction confidence is low.

## Project Structure

├── ESP32_to_cloud/                # Arduino sketch and source code
│   └── ESP32_to_cloud.ino        # Main ESP32 logic (local + cloud offloading)
├── trainer_scripts/              # Azure training and deployment scripts
│   ├── train.ipynb               # Model training notebook
│   └── model_register.ipynb     # Model registration and endpoint deployment
├── app/                          # Flask-based web server
│   ├── app.py                    # Web server entrypoint
│   ├── requirements.txt          # Python dependencies for Flask server
│   └── wand_model.h5             # Trained gesture classification model
└── data/                         # Training dataset folders
├── HorizontalSlash/         # Class: HorizontalSlash gesture
├── VerticalSlash/           # Class: VerticalSlash gesture
└── DiagonalSlash/           # Class: DiagonalSlash gesture



## Hardware Requirements

- ESP32-C3 development board
- Adafruit MPU6050 accelerometer
- Pushbutton (digital input)
- RGB LEDs (Red, Green, Blue)
- Breadboard and jumper wires

## Software Requirements

### Arduino

- Arduino IDE with ESP32 board support
- Required libraries:
  - Adafruit MPU6050
  - Adafruit Sensor
  - Wire
  - WiFi
  - HTTPClient
  - ArduinoJson

### Python / Azure

- Python 3.8 or newer
- Required packages (install via `pip install -r requirements.txt`):
  - Flask
  - numpy
  - tensorflow
- Microsoft Azure account with Machine Learning and App Services enabled

## How It Works

1. The ESP32 collects gesture data using the MPU6050 sensor.
2. The onboard model performs local inference. If the confidence is higher than 80%, it displays the result using RGB LEDs.
3. If the confidence is below the threshold, the raw sensor data is sent to the cloud server for a more accurate prediction.
4. The server processes the data using a larger, uncompressed model and returns the prediction.
5. The ESP32 parses the response and updates the LEDs based on the final predicted gesture.

## Cloud Setup

- Model trained in Azure ML using merged training data
- Deployed via Flask app (`app.py`)
- Hosted using Azure App Services
- API endpoint used by ESP32 to POST raw data and receive prediction

## Example Output



## Deliverables

- GitHub repository with:
  - `ESP32_to_cloud.ino` sketch
  - Flask-based cloud API and trained model
  - Azure training scripts
  - Gesture dataset

- Pictures of serial monitor showing both local and cloud inference paths
- Report addressing lab discussion questions and technical reflection

## Acknowledgments

University of Washington - TECHIN515  
Lab 5: Edge-Cloud Offloading Assignment  
