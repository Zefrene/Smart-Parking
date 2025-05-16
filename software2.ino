// Hardware pins
#define SMOKE_SENSOR_PIN 34   // Analog input for MQ-2 (ESP32)
#define BUZZER_PIN 15         // Digital output to buzzer

// Threshold for smoke detection (tune this based on your sensor)
#define SMOKE_THRESHOLD 400   // Example: 0–1023 ADC value

unsigned long lastReadTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(SMOKE_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);  // Buzzer initially OFF
}

void loop() {
  if (millis() - lastReadTime > 1000) {  // Check every second
    int smokeValue = analogRead(SMOKE_SENSOR_PIN);
    Serial.print("Smoke Value: ");
    Serial.println(smokeValue);

    if (smokeValue >= SMOKE_THRESHOLD) {
      digitalWrite(BUZZER_PIN, HIGH);  // Smoke detected → buzzer ON
    } else {
      digitalWrite(BUZZER_PIN, LOW);   // No smoke → buzzer OFF
    }

    lastReadTime = millis();
  }
}
