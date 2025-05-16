// Optical Sensor Pin Definitions
#define ENTRY_SENSOR_1  2
#define EXIT_SENSOR_1   3
#define ENTRY_SENSOR_2  4
#define EXIT_SENSOR_2   5

// Segment pins
#define SEG_A  6
#define SEG_B  7
#define SEG_C  8
#define SEG_D  9
#define SEG_E  12
#define SEG_F  13
#define SEG_G  14

// Digit selector pins for two-digit display
#define DIGIT1  10
#define DIGIT2  11

// Parking space counters
int region1_spaces = 10;
int region2_spaces = 10;
const int max_spaces = 10;

const byte digitSegments[10][7] = {
  {1,1,1,1,1,1,0}, // 0
  {0,1,1,0,0,0,0}, // 1
  {1,1,0,1,1,0,1}, // 2
  {1,1,1,1,0,0,1}, // 3
  {0,1,1,0,0,1,1}, // 4
  {1,0,1,1,0,1,1}, // 5
  {1,0,1,1,1,1,1}, // 6
  {1,1,1,0,0,0,0}, // 7
  {1,1,1,1,1,1,1}, // 8
  {1,1,1,1,0,1,1}  // 9
};

void setup() {
  // Pin modes for optical sensors
  pinMode(ENTRY_SENSOR_1, INPUT_PULLUP);
  pinMode(EXIT_SENSOR_1, INPUT_PULLUP);
  pinMode(ENTRY_SENSOR_2, INPUT_PULLUP);
  pinMode(EXIT_SENSOR_2, INPUT_PULLUP);

  // Segment pins
  pinMode(SEG_A, OUTPUT);
  pinMode(SEG_B, OUTPUT);
  pinMode(SEG_C, OUTPUT);
  pinMode(SEG_D, OUTPUT);
  pinMode(SEG_E, OUTPUT);
  pinMode(SEG_F, OUTPUT);
  pinMode(SEG_G, OUTPUT);

  // Digit select pins
  pinMode(DIGIT1, OUTPUT);
  pinMode(DIGIT2, OUTPUT);

  digitalWrite(DIGIT1, LOW);
  digitalWrite(DIGIT2, LOW);
}

void loop() {
  handleSensors();
  updateDisplay();
}

void handleSensors() {
  bool currentEntry1 = digitalRead(ENTRY_SENSOR_1);
  bool currentExit1 = digitalRead(EXIT_SENSOR_1);
  bool currentEntry2 = digitalRead(ENTRY_SENSOR_2);
  bool currentExit2 = digitalRead(EXIT_SENSOR_2);

  if (lastEntry1 == HIGH && currentEntry1 == LOW && region1_spaces > 0) {
    region1_spaces--;
  }
  if (lastExit1 == HIGH && currentExit1 == LOW && region1_spaces < max_spaces) {
    region1_spaces++;
  }
  if (lastEntry2 == HIGH && currentEntry2 == LOW && region2_spaces > 0) {
    region2_spaces--;
  }
  if (lastExit2 == HIGH && currentExit2 == LOW && region2_spaces < max_spaces) {
    region2_spaces++;
  }

  lastEntry1 = currentEntry1;
  lastExit1 = currentExit1;
  lastEntry2 = currentEntry2;
  lastExit2 = currentExit2;

  delay(100);
}

void updateDisplay() {
  int value = constrain(region1_spaces, 0, 99);
  int tens = value / 10;
  int ones = value % 10;

  // Display tens digit
  digitalWrite(DIGIT1, HIGH);
  digitalWrite(DIGIT2, LOW);
  setSegments(tens);
  delay(5);

  // Display ones digit
  digitalWrite(DIGIT1, LOW);
  digitalWrite(DIGIT2, HIGH);
  setSegments(ones);
  delay(5);

  // Turn off both digits
  digitalWrite(DIGIT1, LOW);
  digitalWrite(DIGIT2, LOW);
}

void setSegments(int num) {
  digitalWrite(SEG_A, digitSegments[num][0]);
  digitalWrite(SEG_B, digitSegments[num][1]);
  digitalWrite(SEG_C, digitSegments[num][2]);
  digitalWrite(SEG_D, digitSegments[num][3]);
  digitalWrite(SEG_E, digitSegments[num][4]);
  digitalWrite(SEG_F, digitSegments[num][5]);
  digitalWrite(SEG_G, digitSegments[num][6]);
}

