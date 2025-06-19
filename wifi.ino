#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- WiFi & MQTT config ---
#define WIFI_SSID "Arduino"
#define WIFI_PASSWORD "arduinouno"
const char* mqttServer = "thingsboard.cloud";
const int mqttPort = 1883;
const char* token = "SGFN7uIb6klnaFoGXk3L";

// -- Hardware config --
#define IR_SENSOR_A_IN 13
#define IR_SENSOR_A_OUT 12
#define IR_SENSOR_B_IN 14
#define IR_SENSOR_B_OUT 27
#define IR_DETECTED LOW
#define MAX_SLOTS 10
#define DEBOUNCE_DELAY 500

#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

// -- Globals --
WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

int region_A = MAX_SLOTS;
int region_B = MAX_SLOTS;
bool lastAIn = HIGH, lastAOut = HIGH, lastBIn = HIGH, lastBOut = HIGH;
unsigned long lastDetectTimeA_In = 0, lastDetectTimeA_Out = 0;
unsigned long lastDetectTimeB_In = 0, lastDetectTimeB_Out = 0;
unsigned long lastLcdUpdateTime = 0, lastTelemetryTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(IR_SENSOR_A_IN, INPUT);
  pinMode(IR_SENSOR_A_OUT, INPUT);
  pinMode(IR_SENSOR_B_IN, INPUT);
  pinMode(IR_SENSOR_B_OUT, INPUT);
  lcd.init(); lcd.backlight();
  lcd.setCursor(0, 0); lcd.print("Parking System");
  lcd.setCursor(0, 1); lcd.print("Connecting...");

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

  bool stateChanged = false;
  unsigned long currentTime = millis();
  bool currentAIn = digitalRead(IR_SENSOR_A_IN);
  bool currentAOut = digitalRead(IR_SENSOR_A_OUT);
  bool currentBIn = digitalRead(IR_SENSOR_B_IN);
  bool currentBOut = digitalRead(IR_SENSOR_B_OUT);

  // IR logic/debounce for 2 zones
  if ((currentAIn == IR_DETECTED) && (lastAIn != IR_DETECTED) && (currentTime - lastDetectTimeA_In > DEBOUNCE_DELAY)) {
    if (region_A > 0) { region_A--; stateChanged = true; }
    lastDetectTimeA_In = currentTime;
  }
  if ((currentAOut == IR_DETECTED) && (lastAOut != IR_DETECTED) && (currentTime - lastDetectTimeA_Out > DEBOUNCE_DELAY)) {
    if (region_A < MAX_SLOTS) { region_A++; stateChanged = true; }
    lastDetectTimeA_Out = currentTime;
  }
  if ((currentBIn == IR_DETECTED) && (lastBIn != IR_DETECTED) && (currentTime - lastDetectTimeB_In > DEBOUNCE_DELAY)) {
    if (region_B > 0) { region_B--; stateChanged = true; }
    lastDetectTimeB_In = currentTime;
  }
  if ((currentBOut == IR_DETECTED) && (lastBOut != IR_DETECTED) && (currentTime - lastDetectTimeB_Out > DEBOUNCE_DELAY)) {
    if (region_B < MAX_SLOTS) { region_B++; stateChanged = true; }
    lastDetectTimeB_Out = currentTime;
  }

  lastAIn = currentAIn; lastAOut = currentAOut;
  lastBIn = currentBIn; lastBOut = currentBOut;

  // LCD display
  if (stateChanged || millis() - lastLcdUpdateTime > 1000) {
    lcd.clear(); lcd.setCursor(0, 0);
    if (region_A == 0) lcd.print("Zone A: FULL");
    else { lcd.print("Zone A: "); lcd.print(region_A); lcd.print(" Left"); }
    lcd.setCursor(0, 1);
    if (region_B == 0) lcd.print("Zone B: FULL");
    else { lcd.print("Zone B: "); lcd.print(region_B); lcd.print(" Left"); }
    lastLcdUpdateTime = millis();
  }

  // Send telemetry on change or every 10sec
  if (stateChanged || millis() - lastTelemetryTime > 10000) {
    sendTelemetry();
  }
}

void sendTelemetry() {
  if (!client.connected()) return;
  String payload = "{";
  payload += "\"region_A\":" + String(region_A) + ",";
  payload += "\"region_B\":" + String(region_B);
  payload += "}";
  Serial.println("Sending: " + payload);
  client.publish("v1/devices/me/telemetry", (char*)payload.c_str());
  lastTelemetryTime = millis();
}