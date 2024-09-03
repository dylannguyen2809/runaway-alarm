#include <Wire.h>
#include <RTClib.h>
#include <SevSeg.h>
#include <Encoder.h>
#include "pitches.h"

RTC_DS1307 rtc;
SevSeg sevseg; // Create an instance of the SevSeg object
Encoder encoder(32, 33); // Rotary encoder connected to pins 32 and 33
const int buttonPin = 34; // Button connected to pin 34
const int speakerPin = 40; // Speaker connected to pin 40

uint32_t counter = 0;
unsigned long previousMillis = 0;  // Stores the last time the loop was updated
const long interval = 1000;        // Interval at which to read sensors and update time (milliseconds)
unsigned long blinkMillis = 0;     // Stores the last time the display was toggled
const long blinkInterval = 500;    // Interval at which to blink the display (milliseconds)
unsigned long alarmMillis = 0;     // Stores the time when the alarm started
const long alarmDuration = 10000;  // Duration for which the alarm sound should play (milliseconds)
unsigned long lastButtonPress = 0; // Stores the last time the button was pressed
const long debounceDelay = 50;     // Debounce delay for the button (milliseconds)

int alarmHour = 0; // Alarm hour
int alarmMinute = 0; // Alarm minute
int buttonPressCount = 0; // Count the number of button presses
bool buttonPressed = false; // Flag to indicate if the button is pressed
bool settingAlarm = false; // Flag to indicate if the alarm is being set
bool displayOn = true; // Flag to indicate if the display is on or off for blinking
bool alarmTriggered = false; // Flag to indicate if the alarm has been triggered

int melody[] = {
  NOTE_E5, NOTE_D5, NOTE_FS4, NOTE_GS4, 
  NOTE_CS5, NOTE_B4, NOTE_D4, NOTE_E4, 
  NOTE_B4, NOTE_A4, NOTE_CS4, NOTE_E4,
  NOTE_A4
};

int durations[] = {
  8, 8, 4, 4,
  8, 8, 4, 4,
  8, 8, 4, 4,
  2
};

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println(__FILE__);

  // Initialize I2C communication
  Wire.begin();
  Serial.println("I2C bus initialized.");

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  Serial.println("RTC initialized.");

  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("RTC time set.");
  }

  // Initialize the 7-segment display
  byte numDigits = 4;
  byte digitPins[] = {2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
  bool resistorsOnSegments = true; // 'false' means resistors are on digit pins
  bool updateWithDelays = false; // Default 'false' is recommended
  bool leadingZeros = true; // Use 'true' if you want leading zeros

  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros);
  sevseg.setBrightness(80); // Adjust brightness as needed

  // Initialize button pin
  pinMode(buttonPin, INPUT_PULLUP);
  // Initialize speaker pin
  pinMode(speakerPin, OUTPUT);

  // Test the speaker
  tone(speakerPin, 1000); // Play a 1000 Hz tone on the speaker
  delay(1000); // Wait for 1 second
  noTone(speakerPin); // Stop the sound
}

void playNokiaTune() {
  int size = sizeof(durations) / sizeof(int);
  for (int note = 0; note < size; note++) {
    int duration = 1000 / durations[note];
    tone(speakerPin, melody[note], duration);
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);
    noTone(speakerPin);
  }
}

void loop()
{
  unsigned long currentMillis = millis();

  // Read the rotary encoder
  long newPosition = encoder.read() / 4; // Adjust for encoder resolution
  if (newPosition != 0) {
    settingAlarm = true; // Start setting the alarm if the encoder is moved
    if (buttonPressCount == 1) {
      alarmHour = (alarmHour + newPosition) % 24;
      if (alarmHour < 0) alarmHour += 24;
    } else if (buttonPressCount == 2) {
      alarmMinute = (alarmMinute + newPosition) % 60;
      if (alarmMinute < 0) alarmMinute += 60;
    }
    encoder.write(0); // Reset encoder position
  }

  // Check if the button is pressed
  if (digitalRead(buttonPin) == LOW && (currentMillis - lastButtonPress > debounceDelay)) {
    lastButtonPress = currentMillis;
    buttonPressCount++;
    if (buttonPressCount > 2) {
      // Confirm the alarm set time and switch back to clock mode
      settingAlarm = false;
      buttonPressCount = 0;
    }

    // Stop the alarm if it is currently triggered
    if (alarmTriggered) {
      noTone(speakerPin); // Stop the sound
      alarmTriggered = false; // Reset the alarm triggered flag
    }
  }

  // Blink the display if setting the alarm
  if (settingAlarm) {
    if (currentMillis - blinkMillis >= blinkInterval) {
      blinkMillis = currentMillis;
      displayOn = !displayOn; // Toggle display state
    }
    if (displayOn) {
      int displayAlarmTime = (alarmHour * 100) + alarmMinute; // Format HHMM
      sevseg.setNumber(displayAlarmTime, 2); // 2 decimal places for HH:MM
    } else {
      sevseg.blank(); // Turn off display
    }
    sevseg.refreshDisplay(); // Must be called repeatedly
  } else {
    // Display the current time on the 7-segment display
    DateTime now = rtc.now();
    int displayCurrentTime = (now.hour() * 100) + now.minute(); // Format HHMM
    sevseg.setNumber(displayCurrentTime, 2); // 2 decimal places for HH:MM
    sevseg.refreshDisplay(); // Must be called repeatedly
  }

  // Update RTC time every second
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (counter % 10 == 0) {
      Serial.println("\nCNT");
    }

    Serial.print(counter);
    Serial.println();

    counter++;

    // Read and print RTC time
    DateTime now = rtc.now();
    Serial.print("Current Date and Time: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.println(now.second(), DEC);

    // Check if the current time matches the alarm time
    if (now.hour() == alarmHour && now.minute() == alarmMinute && !alarmTriggered) {
      // Trigger alarm action (e.g., sound a buzzer, turn on an LED, etc.)
      Serial.println("ALARM! Time to wake up!");
      playNokiaTune(); // Play the Nokia ringtone
      alarmMillis = currentMillis; // Record the time when the alarm started
      alarmTriggered = true; // Set the alarm triggered flag
    }
  }

  // Stop the alarm sound after the specified duration
  if (alarmTriggered && currentMillis - alarmMillis >= alarmDuration) {
    noTone(speakerPin); // Stop the sound
    alarmTriggered = false; // Reset the alarm triggered flag
  }
}