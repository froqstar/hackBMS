
#define BAT_COUNT 3

#define BAT0_SENS A0
#define BAT1_SENS A1
#define BAT2_SENS A2

#define BAT0_BALANCE 4
#define BAT1_BALANCE 3
#define BAT2_BALANCE 2

#define BAT0_RESISTOR_FACTOR 1
#define BAT1_RESISTOR_FACTOR 2
#define BAT2_RESISTOR_FACTOR 2.74

#define VOLTAGE_SAMPLES 10
#define SAMPLE_INTERVAL 10

#define VOLTAGE_DELTA 0.01

float voltages[BAT_COUNT];
int sensPins[BAT_COUNT] = {BAT0_SENS, BAT1_SENS, BAT2_SENS};
int balancePins[BAT_COUNT] = {BAT0_BALANCE, BAT1_BALANCE, BAT2_BALANCE};
float resistorFactors[BAT_COUNT] = {BAT0_RESISTOR_FACTOR, BAT1_RESISTOR_FACTOR, BAT2_RESISTOR_FACTOR};

void setup() {
  for(int i=0; i<BAT_COUNT; i++) {
    pinMode(sensPins[i], INPUT);
    pinMode(balancePins[i], OUTPUT);
  }
  
  Serial.begin(57600);
}

void loop() {
  // Measure voltages
  float v_sum = 0.0;
  for(int i=0; i<BAT_COUNT; i++) {
    float raw = readVoltage(sensPins[i]);
    float voltage = raw * resistorFactors[i] - v_sum;
    v_sum += voltage;
    voltages[i] = voltage;
    Serial.print("V");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(voltage);
    Serial.print(" ");
  }
  Serial.println("");

  // Determine lowest/highest cell
  float v_min = voltages[0];
  int v_min_index = 0;
  for(int i=0; i<BAT_COUNT; i++) {
    if (voltages[i] < v_min) {
      v_min = voltages[i];
      v_min_index = i;
    }
  }
  float v_max = voltages[0];
  int v_max_index = 0;
  for(int i=0; i<BAT_COUNT; i++) {
    if (voltages[i] > v_max) {
      v_max = voltages[i];
      v_max_index = i;
    }
  }
  Serial.print("Highest cell: ");
  Serial.print(v_max_index);
  Serial.print(", lowest cell: ");
  Serial.print(v_min_index);
  Serial.print(", Delta: ");
  Serial.print(v_max - v_min);
  Serial.println("V");

  // Turn off balancing for all cells
  for (int i=0; i<BAT_COUNT; i++) {
    digitalWrite(balancePins[i], LOW);
  }
  // Balance highest cell
  if (v_max - VOLTAGE_DELTA > v_min) {
    digitalWrite(balancePins[v_max_index], HIGH);
    Serial.print("Balancing cell ");
    Serial.println(v_max_index);
  }
  Serial.println("");
}

float readVoltage(int pin) {
  int sum = 0;
  for (int i=0; i<VOLTAGE_SAMPLES; i++) {
    sum += analogRead(pin);
    delay(SAMPLE_INTERVAL);
  }
  return ((float)sum / (float)VOLTAGE_SAMPLES * 5.0) / 1024.0;
}

