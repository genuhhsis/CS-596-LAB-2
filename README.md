# CS:596 LAB 3
 * Genesis Anne Villar (RED ID: 824435476)
 * Steven Gervacio (RedID: 825656527)
 * CS 596 IOT - Prof. Donyanavard
 * Due Date: 4/2/2025

# Temperature and Humidity Cloud-Connected IoT System
This project implements a cloud-connected IoT sensor system using an ESP32 TTGO T-Display board, a DHT20 temperature and humidity sensor, and an AWS cloud instance. The system collects environmental data and transmits it to a cloud server in real-time, demonstrating a complete sensor-to-cloud architecture.

# Hardware Setup
The DHT20 sensor is connected to the ESP32 using I2C:
- SDA: GPIO 21 (default I2C pins on ESP32)
- SCL: GPIO 22 (default I2C pins on ESP32)
- VCC: 3.3V
- GND: Ground

![Hardware Setup Diagram 

# Video Demonstration
- Link: [Video Demo Link]()

# Features 
- Real-time temperature and humidity monitoring
- WiFi connectivity to transmit sensor data to AWS cloud
- 10-second interval data collection cycle
- AWS EC2 instance with Flask web server for data reception
- TFT display showing current readings and countdown timer
- HTTP GET requests with temperature and humidity parameters
- Error handling for sensor and network issues

# TFT Display Interface
The TTGO T-Display shows:
- Current temperature in Celsius
- Current humidity percentage
- Countdown timer to next reading
- Connection and transmission status

# Cloud Architecture
- AWS EC2 Ubuntu instance running a Flask application
- Web server listening on port 5000
- GET requests containing temperature and humidity data
- Server console displaying received sensor values

# System Operation
- **System Initialization**
  - Connect to WiFi network
  - Initialize DHT20 sensor
  - Set up TFT display
  
- **Data Collection Cycle**
  - Read temperature and humidity values from DHT20
  - Display readings on TFT screen
  - Send data to AWS cloud server
  - Start 10-second countdown to next reading
  
- **Error Handling**
  - Sensor read failures display on TFT
  - Network connection issues reported to serial monitor
  - Automatic retry mechanism for temporary failures

# Implementation Notes
The system uses HTTP GET requests to transmit data in the following format:
```
GET /?temp=23.5&humid=45.7 HTTP/1.1
```

The AWS Flask server receives these parameters and logs them to the console, with future potential for database storage and visualization.
