#include <WiFi.h>            // For ESP32/ESP8266
#include <PubSubClient.h>    // MQTT client

// Wi-Fi credentials
const char* ssid = "XXXXXX";
const char* password = "XXXXXX";

// ThingsBoard credentials
const char* tb_host = "XXXXXX";  // Or your custom ThingsBoard host
const int tb_port = 1883;
const char* tb_token = "XXXXXX";  // Use device token

// Hardware pins
#define SMOKE_SENSOR_PIN 34   // Analog pin (ESP32 ADC)
#define BUZZER_PIN 15         // Digital pin for buzzer

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastSendTime = 0;
int buzzerState = LOW;

// Function to connect to Wi-Fi
void setupWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

// Callback for handling RPC (buzzer control)
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Incoming RPC: ");
  payload[length] = '\0';
  String message = String((char*)payload);
  Serial.println(message);

  // Parse simple ON/OFF
  if (message.indexOf("params") > 0) {
    if (message.indexOf("true") > 0) {
      digitalWrite(BUZZER_PIN, HIGH);
      buzzerState = HIGH;
    } else if (message.indexOf("false") > 0) {
      digitalWrite(BUZZER_PIN, LOW);
      buzzerState = LOW;
    }
  }
}

// Connect to ThingsBoard
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard...");
    if (client.connect("ESP32Client", tb_token, NULL)) {
      Serial.println("connected");
      client.subscribe("v1/devices/me/rpc/request/+");  // Sub to RPC
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  setupWiFi();
  client.setServer(tb_host, tb_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Send smoke data every 5 seconds
  if (millis() - lastSendTime > 5000) {
    int smokeValue = analogRead(SMOKE_SENSOR_PIN);
    String payload = "{\"smoke\":" + String(smokeValue) + "}";
    client.publish("v1/devices/me/telemetry", payload.c_str());
    Serial.println("Sent: " + payload);
    lastSendTime = millis();
  }
}
