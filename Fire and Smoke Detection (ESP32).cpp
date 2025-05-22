#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// WiFi Configuration
const char* ssid = "Wifi_name";  // insert your wifi name 
const char* password = "wifi_password"; // insert your wifi password

// MQTT Configuration
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_topic = "IoT/example"; // insert your mqtt topic

// Sensor Pins
#define DHTPIN 23
#define DHTTYPE DHT22
#define MQ7_PIN 34
#define MQ135_PIN 35 
#define DUST_PIN 32
#define DUST_LED 33

// Output Pins
#define BUZZER 25
#define LED_RED 26
#define LED_YELLOW 27
#define LED_GREEN 14

// Sensor Thresholds
#define CO_WARNING_LEVEL 100    // ppm
#define CO_DANGER_LEVEL 200    // ppm
#define SMOKE_THRESHOLD 800    // raw value
#define FIRE_THRESHOLD 1800    // raw value (MQ135)
#define DUST_THRESHOLD 2000    // raw value
#define TEMP_THRESHOLD 45      // °C
#define HUMIDITY_HIGH 70       // %
#define HUMIDITY_LOW 30        // %

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setupWifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 3 seconds");
      delay(3000);
    }
  }
}

float readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Failed to read temperature!");
    return 0;
  }
  return t;
}

float readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read humidity!");
    return 0;
  }
  return h;
}

int readDustSensor() {
  digitalWrite(DUST_LED, LOW);
  delayMicroseconds(280);
  int dust = analogRead(DUST_PIN);
  delayMicroseconds(40);
  digitalWrite(DUST_LED, HIGH);
  delayMicroseconds(9680);
  return dust;
}

float convertMQ7(int rawValue) {
  float voltage = rawValue * (3.3 / 4095.0);
  float rs = (3.3 - voltage) / voltage;
  float ratio = rs / 1.0; // Replace with calibrated R0 value
  return 10.0 * pow(ratio, -1.5); // CO ppm approximation
}

void publishSensorData(float temp, float humidity, int dust, float co, int mq135, const char* status) {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  StaticJsonDocument<256> doc;
  doc["room_id"] = 1;
  
  JsonObject sensorData = doc.createNestedObject("sensorData");
  
  JsonObject dht22 = sensorData.createNestedObject("dht22");
  dht22["temperature"] = temp;
  dht22["humidity"] = humidity;
  
  JsonObject mq7 = sensorData.createNestedObject("mq7");
  mq7["co"] = co;
  
  JsonObject dustObj = sensorData.createNestedObject("dust");
  dustObj["dust"] = dust;
  
  JsonObject mq135Obj = sensorData.createNestedObject("mq135");
  mq135Obj["air_quality"] = mq135;
  
  doc["status"] = status;

  char payload[256];
  serializeJson(doc, payload);
  client.publish(mqtt_topic, payload);
  
  Serial.print("Published: ");
  Serial.println(payload);
}

void setOutputs(bool buzzer, bool red, bool yellow, bool green) {
  digitalWrite(BUZZER, buzzer ? HIGH : LOW);
  digitalWrite(LED_RED, red ? HIGH : LOW);
  digitalWrite(LED_YELLOW, yellow ? HIGH : LOW);
  digitalWrite(LED_GREEN, green ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  
  // Initialize sensors
  dht.begin();
  
  // Set pin modes
  pinMode(MQ7_PIN, INPUT);
  pinMode(MQ135_PIN, INPUT);
  pinMode(DUST_PIN, INPUT);
  pinMode(DUST_LED, OUTPUT);
  
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  
  // Initial state
  setOutputs(false, false, false, true);
  
  // Connect to WiFi
  setupWifi();
  
  // Setup MQTT
  client.setServer(mqtt_server, mqtt_port);
  
  Serial.println("System initialized");
}

void loop() {
  // Read sensors
  float temperature = readDHTTemperature();
  float humidity = readDHTHumidity();
  int dust = readDustSensor();
  int mq7_raw = analogRead(MQ7_PIN);
  float co_ppm = convertMQ7(mq7_raw);
  int mq135 = analogRead(MQ135_PIN);
  
  // Print sensor values
  Serial.print(" | Temp: "); Serial.print(temperature); Serial.println("°C");
  Serial.print(" | Humidity: "); Serial.print(humidity); Serial.println("%");
  Serial.print(" | Dust: "); Serial.println(dust);
  Serial.print(" | CO: "); Serial.print(co_ppm); Serial.println("ppm");
  Serial.print(" | Air Quality: "); Serial.println(mq135);
  
  // Determine status
  const char* status = "normal";
  bool buzzer = false;
  bool red = false;
  bool yellow = false;
  bool green = true;
  
  // Check for fire conditions
  if (mq135 > FIRE_THRESHOLD && temperature > TEMP_THRESHOLD && 
      dust > DUST_THRESHOLD && humidity < HUMIDITY_LOW) {
    status = "fire";
    buzzer = true;
    red = true;
    green = false;
  } 
  // Check for smoke
  else if (mq135 > SMOKE_THRESHOLD && co_ppm > CO_WARNING_LEVEL) {
    status = "smoke";
    yellow = true;
    green = false;
  }
  
  // Set outputs
  setOutputs(buzzer, red, yellow, green);
  
  // Publish data
  publishSensorData(temperature, humidity, dust, co_ppm, mq135, status);
  
  delay(1000); // Wait 1 second between readings
}
