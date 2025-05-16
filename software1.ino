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
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}
