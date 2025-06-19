#include <WiFi.h>
#include <PubSubClient.h>

// --- WiFi & MQTT config ---
#define WIFI_SSID "Arduino"
#define WIFI_PASSWORD "arduinouno"
const char* mqttServer = "thingsboard.cloud";
const int mqttPort = 1883;
const char* token = "SGFN7uIb6klnaFoGXk3L";

// --- Pin + Logic config ---
#define MQ2_SENSOR 34
#define BUZZER_PIN 26
#define SMOKE_THRESHOLD 2000

// --- Globals ---
WiFiClient espClient;
PubSubClient client(espClient);

int smokeValue = 0;
bool smokeAlarmActive = false;
unsigned long lastBeepTime = 0;
unsigned long lastTelemetryTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(MQ2_SENSOR, INPUT);
  pinMode(BUZZER_PIN, OUTPUT); digitalWrite(BUZZER_PIN, HIGH);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("Connected!");

  client.setServer(mqttServer, mqttPort);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED && !client.connected()) {
    Serial.print("Connecting MQTT...");
    if (client.connect("ESP32Client", token, NULL)) Serial.println(" OK!");
    else { Serial.println(" MQTT failed!"); delay(2000); return; }
  }
  if (client.connected()) client.loop();

  smokeValue = analogRead(MQ2_SENSOR);

  static bool lastAlarm = false;
  // Detect state change
  if (smokeValue > SMOKE_THRESHOLD && !smokeAlarmActive) {
    smokeAlarmActive = true;
    Serial.println("🔥 Smoke Detected! ALARM ACTIVATED.");
    sendTelemetry();
  } else if (smokeValue <= SMOKE_THRESHOLD && smokeAlarmActive) {
    smokeAlarmActive = false;
    Serial.println("Smoke has cleared. ALARM DEACTIVATED.");
    sendTelemetry();
  }

  // While alarm active, beep buzzer + periodic telemetry
  if (smokeAlarmActive) {
    if (millis() - lastBeepTime > 300) {
      digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
      lastBeepTime = millis();
    }
    if (millis() - lastTelemetryTime > 5000) {
      sendTelemetry();
    }
  } else {
    digitalWrite(BUZZER_PIN, HIGH);  // off
  }
}

void sendTelemetry() {
  if (!client.connected()) return;
  String payload = "{";
  payload += "\"smoke_value\":" + String(smokeValue) + ",";
  payload += "\"smoke_alarm\":" + String(smokeAlarmActive ? "true" : "false");
  payload += "}";
  Serial.println("Sending: " + payload);
  client.publish("v1/devices/me/telemetry", (char*)payload.c_str());
  lastTelemetryTime = millis();
}