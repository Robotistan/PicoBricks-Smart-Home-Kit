// Libraries
#include <picobricks.h>    

// Pin Definition
#define RGB_PIN 6           // RGB LED control pin
#define LED_PIN 7           // LED pin
#define BUTTON_PIN 10       // Button pin
#define DHT_PIN 11          // DHT11 temperature sensor pin
#define BUZZER_PIN 20       // Buzzer pin 
#define MOTOR_1 21          // Servo motor 1 pin (Door)
#define MOTOR_2 22          // Servo motor 2 pin (Window)
#define LDR_PIN 27          // Light-dependent resistor (LDR) pin 

// Defines
#define DECODE_NEC          // IR remote protocol definition 
#define RGB_COUNT 1         // Number of RGB LEDs connected

// Variables
float temperature;          // Variable to hold temperature value
int ldr;                    // Variable to hold LDR light level
int noteDuration;           // Duration of a note
int button = 0;             // Flag to track button press

// Melody for doorbell (frequency and duration)
unsigned long door_bell[][2] = {
  {262, 1}, // C4
  {330, 1}, // E4
  {392, 1}, // G4
  {523, 1}  // C5
};

// Servo angle settings for open/close
int windowOpen = 0;
int windowClose = 80;  
int doorOpen = 0;
int doorClose = 90; 

// Threshold values for environment control
int tempThreshold = 25;    // Temperature threshold (°C) to open the window
int ldrThreshold = 95;     // Light threshold (%) to activate lighting

// Function & Object Declarations
NeoPixel strip (RGB_PIN, RGB_COUNT);       // RGB LED strip instance
DHT11 dht(DHT_PIN);                        // DHT11 sensor instance
ServoSimple ServoDoor(MOTOR_1);            // Servo motor for the door
ServoSimple ServoWindow(MOTOR_2);          // Servo motor for the window

// Interrupt handler for button press
void buttonInterruptHandler() {
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == HIGH) {
    button = 1;  // Set the flag to trigger doorbell routine
  }
}

// Function to play a note using the buzzer
void playNote(int frequency, int duration) {
  long period = 1000000L / frequency;                  // Calculate the period of the wave
  long cycles = (long)frequency * duration / 1000;     // Number of wave cycles

  for (long i = 0; i < cycles; i++) {
    digitalWrite(BUZZER_PIN, HIGH);                    // Turn buzzer ON
    delayMicroseconds(period / 2);
    digitalWrite(BUZZER_PIN, LOW);                     // Turn buzzer OFF
    delayMicroseconds(period / 2);
  }
}

// Function to play the doorbell melody
void play_melody() {
  for (int thisNote = 0; thisNote < 4; thisNote++) {
    playNote(door_bell[thisNote][0], noteDuration);                 // Play note frequency
    noteDuration = 110 * door_bell[thisNote][1];                       // Set duration
    delay(noteDuration * 0.2);                                      // Short pause between notes
  }
}

void setup() {
  Serial.begin(115200);                 // Start serial communication for debugging

  // Configure pin modes
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(MOTOR_1, OUTPUT);
  pinMode(MOTOR_2, OUTPUT);

  // Initialize servos
  ServoDoor.begin();
  ServoWindow.begin();

  // Move servos to default (closed) positions
  ServoDoor.setAngle(doorClose);
  ServoWindow.setAngle(windowClose);

  // Set up interrupt for button press
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterruptHandler, CHANGE);

  // Initialize DHT sensor
  dht.begin();
}

void loop() {
  // If button was pressed
  if (button == 1) {
    digitalWrite(LED_PIN, HIGH);       // Turn ON the LED
    play_melody();                     // Play the doorbell melody
    digitalWrite(LED_PIN, LOW);        // Turn OFF the LED

    ServoDoor.setAngle(doorOpen);      // Open the door
    delay(2000);                       // Wait 2 seconds
    ServoDoor.setAngle(doorClose);     // Close the door
    button = 0;                        // Reset button flag
  }

  // Read light level and control RGB lighting
  ldr = (100 - (analogRead(LDR_PIN) * 100) / 1023);  // Convert analog value to percentage

  if (ldr > ldrThreshold){
    strip.setPixelColor(0, 255, 255, 255); // Turn ON RGB light to white
  } else {
    strip.setPixelColor(0, 0, 0, 0);       // Turn OFF RGB light
  }

  // Read temperature and control window servo
  temperature = dht.readTemperature();     // Read temperature in Celsius

  if (temperature >= tempThreshold){
    ServoWindow.setAngle(windowOpen);      // Open window if too hot
  } else {
    ServoWindow.setAngle(windowClose);     // Close window if temperature is okay
  }
}
