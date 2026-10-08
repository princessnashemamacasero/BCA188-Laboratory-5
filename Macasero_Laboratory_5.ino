#include <Arduino.h>

const uint8_t BUTTON_PIN = 4;
const uint8_t POT_PIN = 5;
const uint8_t STATUS_LED_PIN = 18;
const uint8_t PWM_LED_PIN = 19;

bool buttonPressed = false;
bool pwmReady = false;
int rawInput = 0;
int requestedDuty = 0;
int appliedDuty = 0;

void readInputs();
void processInputs();
void updateOutputs();

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
  pinMode(PWM_LED_PIN, OUTPUT);
  digitalWrite(PWM_LED_PIN, LOW);
  analogReadResolution(12);
  analogSetPinAttenuation(POT_PIN, ADC_11db);
  pwmReady = ledcAttach(PWM_LED_PIN, 5000, 8);
  if (pwmReady) {
    ledcWrite(PWM_LED_PIN, 0);
  } else {
    Serial.println("PWM setup failed.");
  }
}

void loop() {
  readInputs();
  processInputs();
  updateOutputs();
  delay(20); // Pause for 20 ms between loop iterations.
}

void readInputs() {
  buttonPressed = (digitalRead(BUTTON_PIN) == LOW);
  rawInput = analogRead(POT_PIN);
}

void processInputs() {
  requestedDuty = constrain(map(rawInput, 0, 4095, 0, 255), 0L, 255L);
  if (buttonPressed && pwmReady) {
    appliedDuty = requestedDuty;
  } else {
    appliedDuty = 0;
  }
}

void updateOutputs() {
  digitalWrite(STATUS_LED_PIN, (buttonPressed && pwmReady) ? HIGH : LOW);
  if (pwmReady) {
    ledcWrite(PWM_LED_PIN, appliedDuty);
    Serial.println("Actual Raw ADC= " + String(rawInput) + "\tActual PWM= " + String(requestedDuty));
  }
}