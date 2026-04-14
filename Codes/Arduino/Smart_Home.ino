#include <Servo.h>
#include <picobricks.h>

// Pin Definition
#define RGB_PIN 6
#define LED_PIN 7
#define BUTTON_PIN 10
#define DHT_PIN 11
#define BUZZER_PIN 20
#define MOTOR_1 21
#define MOTOR_2 22
#define LDR_PIN 27

#define RGB_COUNT 1

// Objects
NeoPixel strip(RGB_PIN, RGB_COUNT);
DHT11 dht(DHT_PIN);

Servo servoDoor;
Servo servoWindow;

// Variables
float temperature;
int ldr;
int button = 0;
int noteDuration;

// Servo angles
int windowOpen = 0;
int windowClose = 80;
int doorOpen = 0;
int doorClose = 90;

// Thresholds
int tempThreshold = 27;
int ldrThreshold = 85;

// Melody
unsigned long door_bell[][2] = {
  {262, 1},
  {330, 1},
  {392, 1},
  {523, 1}
};

// Interrupt
void buttonInterruptHandler() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    button = 1;
  }
}

// Buzzer
void playNote(int frequency, int duration) {
  long period = 1000000L / frequency;
  long cycles = (long)frequency * duration / 1000;

  for (long i = 0; i < cycles; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(period / 2);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(period / 2);
  }
}

void play_melody() {
  for (int i = 0; i < 4; i++) {
    noteDuration = 110 * door_bell[i][1];
    playNote(door_bell[i][0], noteDuration);
    delay(noteDuration * 0.2);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);

  // Servo setup
  servoDoor.attach(MOTOR_1);
  servoWindow.attach(MOTOR_2);

  servoDoor.write(doorClose);
  servoWindow.write(windowClose);

  // Interrupt
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterruptHandler, FALLING);

  // Sensor
  dht.begin();
}

void loop() {

  // BUTTON CONTROL
  if (button == 1) {
    digitalWrite(LED_PIN, HIGH);

    play_melody();

    digitalWrite(LED_PIN, LOW);

    servoDoor.write(doorOpen);

    delay(2000);

    servoDoor.write(doorClose);

    button = 0;
  }

  // LDR CONTROL
  ldr = (100 - (analogRead(LDR_PIN) * 100) / 1023);

  Serial.print("LDR: ");
  Serial.println(ldr);

  if (ldr < ldrThreshold) {
    strip.setPixelColor(0, 255, 255, 255);
  } else {
    strip.setPixelColor(0, 0, 0, 0);
  }

  // TEMPERATURE CONTROL
  temperature = dht.readTemperature();

  Serial.print("Temp: ");
  Serial.println(temperature);

  if (temperature >= tempThreshold) {
    servoWindow.write(windowOpen);
  } else {
    servoWindow.write(windowClose);
  }

  delay(100);
}
