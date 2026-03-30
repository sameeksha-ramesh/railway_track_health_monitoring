# railway_track_health_monitoring
Author: Sameeksha R — VIT Chennai 
Board: TI CC3200 LaunchPad (Energia IDE)
Year: 2024

Overview
Real-time railway safety monitoring system that detects overspeed and abnormal track vibrations, transmits instant Wi-Fi alerts to a remote server, and logs all data to an SD card for post-accident black-box analysis.

System Architecture
[Accelerometer] ──ADC──┐
[Piezo Sensor]  ──ADC──┤
                        ├── TI CC3200 ──WiFi──► Remote Server (Python/Flask)
[GPS NEO-6M]    ──UART─┘       │
                                └──SPI──► SD Card (Black-box log)

Repository Structure
├── railway_monitor.ino   — Main Energia sketch (CC3200)
├── sensor_utils.h        — ADC→g conversion, RMS, EMA filter
├── gps_parser.h          — Lightweight NMEA parser (GPRMC)
└── server.py             — Python Flask server (receives Wi-Fi alerts)

Features
FeatureDetailVibration detection3-axis accelerometer RMS (g) + piezo sensorSpeed monitoringGPS NEO-6M via UART — knots → km/hAnomaly detectionConfigurable speed + vibration thresholdsWi-Fi alertHTTP POST JSON payload to Flask serverBlack-box loggingAll readings → SD card CSV (even without Wi-Fi)Dual redundancyWi-Fi alert + SD log happen simultaneously

Hardware Setup
ComponentCC3200 PinInterfaceAccelerometer XPin 2ADCAccelerometer YPin 3ADCAccelerometer ZPin 4ADCPiezo SensorPin 5ADCGPS NEO-6M TXPin 8UART (Serial1)SD Card CSPin 8SPI

Thresholds (configurable in railway_monitor.ino)
cppconst float SPEED_THRESHOLD_KMPH = 120.0f;   // Max allowed speed
const float VIBRATION_THRESHOLD  = 2.5f;     // Acceleration RMS in g
const float PIEZO_THRESHOLD      = 600.0f;   // Piezo ADC value

Build & Flash

Install Energia IDE
Select board: CC3200-LAUNCHXL
Open railway_monitor.ino
Set your Wi-Fi SSID/password and server IP
Flash via USB

Run Server
bashpip install flask
python server.py
# Server listens on port 8080
# GET  http://localhost:8080/status  → health check
# POST http://localhost:8080/alert   → receives CC3200 alerts

SD Card Log Format
timestamp_ms, speed_kmph, accel_rms_g, piezo_adc, lat, lon, event
15234, 45.2, 0.312, 124, 13.082680, 80.270718, NORMAL
18901, 134.7, 3.821, 712, 13.082695, 80.270891, OVERSPEED+VIBRATION
