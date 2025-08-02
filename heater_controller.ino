#include "DHT.h"

// Pins
#define DHTPIN 21        // GPIO21
#define RELAYPIN 22      // GPIO22
#define LEDPIN 19        // GPIO19
#define OVERHEAT_LED 18  // GPIO18

#define DHTTYPE DHT22

// Thresholds (°C)
const float TEMP_HEAT_ON = 20.0;
const float TEMP_HEAT_OFF = 25.0;
const float TEMP_STABILIZE_LOW = 24.0;
const float TEMP_STABILIZE_HIGH = 25.0;
const float TEMP_OVERHEAT = 30.0;

DHT dht(DHTPIN, DHTTYPE);

// State enum
enum HeaterState { IDLE, HEATING, STABILIZING, TARGET_REACHED, OVERHEAT };
HeaterState state = IDLE;
unsigned long lastBlinkTime = 0;
bool ledState = LOW;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, HIGH);

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  pinMode(OVERHEAT_LED, OUTPUT);
  digitalWrite(OVERHEAT_LED, LOW);

  Serial.println("Heater Control System with State Machine and 2 LEDs Starting...");
}

void loop() {
  float temperature = dht.readTemperature();

  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }
  Serial.print("Current Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, ");

  switch(state) {
    case IDLE:
      digitalWrite(RELAYPIN, HIGH);    // Heater OFF
      digitalWrite(LEDPIN, LOW);       // Main LED OFF
      digitalWrite(OVERHEAT_LED, LOW); // Overheat LED OFF
      Serial.println("State: IDLE");

      if (temperature >= TEMP_OVERHEAT) {
        state = OVERHEAT;
        Serial.println("Transition to OVERHEAT");
      } else if (temperature < TEMP_HEAT_ON) {
        state = HEATING;
        Serial.println("Transition to HEATING");
      } else if (temperature >= TEMP_STABILIZE_LOW && temperature <= TEMP_STABILIZE_HIGH) {
        state = STABILIZING;
        Serial.println("Transition to STABILIZING");
      } else if (temperature > TEMP_STABILIZE_HIGH && temperature < TEMP_OVERHEAT) {
        state = TARGET_REACHED;
        Serial.println("Transition to TARGET_REACHED");
      }
      break;

    case HEATING:
      digitalWrite(RELAYPIN, LOW);     // Heater ON
      digitalWrite(LEDPIN, HIGH);      // Main LED ON
      digitalWrite(OVERHEAT_LED, LOW); // Overheat LED OFF
      Serial.println("State: HEATING");
      if (temperature >= TEMP_STABILIZE_LOW && temperature <= TEMP_STABILIZE_HIGH) {
        state = STABILIZING;
        Serial.println("Transition to STABILIZING");
      } else if (temperature >= TEMP_OVERHEAT) {
        state = OVERHEAT;
        Serial.println("Transition to OVERHEAT");
      }
      break;

    case STABILIZING:
      digitalWrite(RELAYPIN, HIGH);    // Heater OFF
      // Blink main LED slowly
      if (millis() - lastBlinkTime >= 500) {
        ledState = !ledState;
        digitalWrite(LEDPIN, ledState);
        lastBlinkTime = millis();
      }
      digitalWrite(OVERHEAT_LED, LOW);
      Serial.println("State: STABILIZING");
      if (temperature < TEMP_STABILIZE_LOW) {
        state = HEATING;
        Serial.println("Temp dropped - back to HEATING");
        digitalWrite(LEDPIN, HIGH);
      } else if (temperature > TEMP_STABILIZE_HIGH) {
        state = TARGET_REACHED;
        Serial.println("Reached TARGET");
        digitalWrite(LEDPIN, LOW);
      } else if (temperature >= TEMP_OVERHEAT) {
        state = OVERHEAT;
        Serial.println("Transition to OVERHEAT");
      }
      break;

    case TARGET_REACHED:
      digitalWrite(RELAYPIN, HIGH);    // Heater OFF
      digitalWrite(LEDPIN, LOW);       // Main LED OFF
      digitalWrite(OVERHEAT_LED, LOW); // Overheat LED OFF
      Serial.println("State: TARGET_REACHED");
      if (temperature < TEMP_HEAT_ON) {
        state = HEATING;
        Serial.println("Temp dropped - back to HEATING");
      } else if (temperature >= TEMP_OVERHEAT) {
        state = OVERHEAT;
        Serial.println("Transition to OVERHEAT");
      }
      break;

    case OVERHEAT:
      digitalWrite(RELAYPIN, HIGH);    // Heater OFF
      digitalWrite(LEDPIN, LOW);       // Main LED OFF
      digitalWrite(OVERHEAT_LED, HIGH); // Turn ON overheat LED constantly
      Serial.println("State: OVERHEAT! Heater OFF - Emergency");
      if (temperature < TEMP_HEAT_OFF) {
        state = IDLE;
        Serial.println("Temperature safe - back to IDLE");
        digitalWrite(OVERHEAT_LED, LOW);
      }
      break;
  }
  delay(2000);
}
