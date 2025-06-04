
# IoT Environmental Monitoring System with Fire and Smoke Detection (ESP32)

## Project Benefits

This project is an **IoT-based environmental monitoring system** using an **ESP32** microcontroller. It integrates sensors for **temperature, humidity, smoke, carbon monoxide (CO), and dust** to:
- Detect potential fire or smoke hazards in real-time.
- Provide alerts through a buzzer and indicator LEDs.
- Send sensor data to an MQTT broker for remote monitoring.
- Improve safety and air quality awareness in homes, offices, warehouses, or labs.

---

## Hardware Requirements

- **ESP32** (microcontroller)
- **DHT22** (temperature & humidity sensor)
- **MQ7** (carbon monoxide sensor)
- **MQ135** (air quality / smoke sensor)
- **Dust sensor** (optical-based)
- **Buzzer**, **Red/Yellow/Green LEDs**
- Wi-Fi connection
- MQTT Broker (e.g., `broker.emqx.io`)

---

## Required Libraries

- `WiFi.h`
- `PubSubClient.h`
- `DHT.h`
- `ArduinoJson.h`

Install these via Arduino IDE Library Manager.

---

## Configuration

- **Wi-Fi SSID**: `Jo`
- **Password**: `1928374655`
- **MQTT Broker**: `broker.emqx.io`
- **Port**: `1883`
- **MQTT Topic**: `IoT/group8`

---
## Monitoring Web
click this <a href="https://github.com/JordyMail/iot-system-v1">Link</a> to direct you to the websiete monitorng repository

## Sensor Design
![CIRCUIT SMOKING DETECTION FIXXXX](https://github.com/user-attachments/assets/dafa3169-6f42-4310-9329-e16d24c4a585) </br>


## Code Overview

### WiFi Setup

```cpp
void setupWifi()
````

Connects the ESP32 to the configured Wi-Fi network and prints the local IP address.

---

### MQTT Reconnection

```cpp
void reconnectMQTT()
```

Attempts to reconnect to the MQTT broker if disconnected, using client ID `ESP32Client`.

---

### Read Temperature & Humidity

```cpp
float readDHTTemperature()
float readDHTHumidity()
```

Reads values from the DHT22 sensor. Returns 0 if the reading fails.

---

### Dust Sensor Reading

```cpp
int readDustSensor()
```

Triggers the LED inside the dust sensor, reads analog value from dust pin, and calculates concentration.

---

### CO Conversion (MQ7)

```cpp
float convertMQ7(int rawValue)
```

Converts raw analog value into an estimated CO concentration in ppm using a basic formula.

---

### MQTT Publish

```cpp
void publishSensorData(...)
```

Sends a JSON-formatted payload to the MQTT topic with sensor readings and environment status.

---

### Output Control

```cpp
void setOutputs(bool buzzer, bool red, bool yellow, bool green)
```

Controls the buzzer and LEDs (ON/OFF) based on the condition detected.

---

### Setup

```cpp
void setup()
```

* Initializes sensors and pin modes.
* Turns on the green LED as default state.
* Connects to Wi-Fi and sets up MQTT.

---

### Main Loop

```cpp
void loop()
```

* Reads all sensor data.
* Evaluates the following conditions:

  * 🔴 **FIRE DETECTED**: MQ135 > 1800, Temp > 45°C, Dust > 2000, Humidity < 30%
  * 🟡 **SMOKE DETECTED**: MQ135 > 800 and CO > 50 ppm
  * 🟢 **NORMAL**: All values within safe range
* Controls outputs and sends data via MQTT every second.

---

## Example JSON Payload

```json
{
  "temperature": 30.5,
  "humidity": 45.3,
  "dust": 1000,
  "co": 80.2,
  "air_quality": 900,
  "status": "WARNING: Smoke detected"
}
```

---

## Notes

* For accurate CO measurement, calibrate the MQ7 sensor’s R0 resistance value.
* MQTT data can be visualized using tools like Node-RED, ThingsBoard, or custom dashboards.
* Can be extended with cloud logging or mobile notifications.

---

## License

This project is open-source and free to use for educational and non-commercial purposes.

---

## Contributors

Developed by **Group 8** - IoT Project 2025


### Hotel Room Design 1 (700cm x 320cm)
<img src="https://github.com/user-attachments/assets/6dfe74dd-7e4b-40ed-97f6-004bd73f500d" width="600" height="auto">
<img src="https://github.com/user-attachments/assets/7f664707-1eb9-49b4-ab60-53efde93ba5c" width="600" height="auto">
<img src="https://github.com/user-attachments/assets/b0da6783-718f-4908-89d3-02926502c173" width="600" height="auto">
<img src="https://github.com/user-attachments/assets/7b36aaf9-586c-4d98-aeb0-4826dea5536a" width="600" height="auto">
<img src="https://github.com/user-attachments/assets/3a70fef4-8a12-4c4b-b101-34f77a80c59c" width="600" height="auto">



  
