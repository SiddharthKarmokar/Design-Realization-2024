# Design-Realization-2024
# ESP32 Sensor Data Processing and Transmission

This project leverages an ESP32 to collect sensor data and send it to an ESP8266 receiver via two possible pathways:
1. **ESP-NOW Communication**: Directly transmits raw sensor data.
2. **Machine Learning Pathway**: Utilizes a logistic regression model to clean and preprocess the data with the help of Google Sheets, ensuring higher data quality before transmission.

## Features
- **Dual Data Transmission Modes**: Choose between direct ESP-NOW communication or ML-enhanced data cleaning.
- **Machine Learning Integration**: Employ logistic regression for data preprocessing, with support from Google Sheets for data management.
- **Seamless ESP32 to ESP8266 Communication**: Ensures reliable data transfer between devices.
- **Flexible Sensor Options**: Originally designed for a flex sensor, but can be substituted with any other Arduino-compatible sensor.

## Getting Started

### Prerequisites
- ESP32 Development Board
- ESP8266 Development Board
- Flex Sensor (or any other Arduino-compatible sensor)
- Google Sheets API credentials
- Logistic Regression model script

### Setup ESP32 for Sensor Data Collection
1. Connect your sensor (e.g., flex sensor) to the ESP32.
2. Initialize the ESP-NOW protocol on the ESP32 for data transmission.

### Integrate Google Sheets API
1. Set up a Google Sheets account and create a new spreadsheet for data logging.
2. Obtain Google Sheets API credentials and set up the API in your project.
3. Implement the logistic regression model to clean and preprocess the sensor data.

### Configure ESP8266 Receiver
1. Prepare your ESP8266 to receive data from the ESP32.
2. Implement the necessary code to handle incoming data and process it accordingly.
