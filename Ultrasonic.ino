/*
Team Number:
???

Team Members:
1. Nathen Priyonggo - rpriyo2
2. Kyle Sabo - ksabo4

Project Name:
Project ASL Translator

Abstract:
This project presents an innovative text communication device 
using ultrasonic sensors to convert hand distances into letters 
of the alphabet. Users compose messages by confirming letters 
with button presses and transmitting the completed text to a 
second Arduino via serial communication. The recipient Arduino 
displays the message on an LCD, enabling efficient and accessible 
communication without a traditional keyboard. Designed for 
individuals with limited mobility or those seeking alternative 
input methods, this system leverages intuitive sensor technology 
to offer an inclusive, user-friendly solution for text-based 
interaction. It combines simplicity and functionality to bridge 
communication gaps effectively.
*/

// include library code
#include <LiquidCrystal.h>

// these constants won’t change.
const int trigPin = 13;
const int echoPin = 12;
const int rs = 11;
const int en = 10;
const int d4 = 9;
const int d5 = 8;
const int d6 = 7;
const int d7 = 6;
const int ledPin = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int interruptPin1 = 3;
const int interruptPin2 = 2;
String sentence = "";

// these variables will change.
unsigned long prevMillis = 0;
unsigned long curMillis = 0;
unsigned long pulseStartTime = 0;
unsigned long lastInterruptTime = 0;
unsigned long lastBlinkStartTime = 0;
int pulseState = 0;  // 0 = LOW start, 1 = HIGH pulse, 2 = LOW end
int ind = 0;
int prevInd = 0;
int serialInd = 0;
bool doBlink = false;

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  attachInterrupt(digitalPinToInterrupt(interruptPin1), ISR1, FALLING);
  attachInterrupt(digitalPinToInterrupt(interruptPin2), ISR2, FALLING);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
}

void loop() {
  // store current time
  curMillis = millis();

  // Ultrasonic pulse logic
  if ((curMillis - prevMillis) > 50) {
    prevMillis = curMillis;

    // Trigger the ultrasonic sensor with a state-based non-blocking pulse
    if (pulseState == 0 && (curMillis - pulseStartTime) >= 2) {
      // Step 1: Set trigger pin LOW for a clean pulse start
      digitalWrite(trigPin, LOW);
      pulseStartTime = millis();
      pulseState = 1;  // Move to next state
    }
    else if (pulseState == 1 && (curMillis - pulseStartTime) >= 2) {
      // Step 2: Set trigger pin HIGH after pulse delay
      digitalWrite(trigPin, HIGH);
      pulseStartTime = millis();
      pulseState = 2;  // Move to next state
    }
    else if (pulseState == 2 && (curMillis - pulseStartTime) >= 10) {
      // Step 3: Set trigger pin LOW again after the delay
      digitalWrite(trigPin, LOW);
      pulseState = 0;  // Reset state

      // Measure the duration of the echo
      long duration = pulseIn(echoPin, HIGH);

      // Convert duration to distance
      long cm = microsecondsToCentimeters(duration);

      // Use alphaLibrary function to get character based on distance
      char alpha = alphaLibrary(cm);

      if (prevInd != ind) {
        sentence.concat(alpha);
        prevInd = ind;
      }
      // Serial.print(", ");
      lcd.setCursor(ind, 1);
      lcd.print(alpha);
    }
  }

  // Handle incoming  serial data
  while (Serial.available()) {

    if (serialInd == 0) {
      lcd.setCursor(0, 0);
      for (int i = 0; i < 16; i++) {
        lcd.print(" ");
      }
      lcd.setCursor(serialInd, 0);
    }

    char receivedChar = Serial.read();
    // End of sentence
    if (receivedChar == '\n') {
      lcd.setCursor(ind, 1);
      serialInd = 0;
    }
    else {
      lcd.setCursor(serialInd, 0);
      lcd.print(receivedChar);
      serialInd += 1;
    }

    lastBlinkStartTime = millis();
    digitalWrite(ledPin, HIGH);
  }

  // Blink code for notification
  if (millis() - lastBlinkStartTime > 500) {
    digitalWrite(ledPin, LOW);
  }
}

long microsecondsToCentimeters(long microseconds) {
  // Formula : Distance = Time  * Speed of sound  / 2
  // The speed of sound is 340 m/s or 0.034 centimeters per microsecond.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance traveled.
  return microseconds * 0.034 / 2;
}

char alphaLibrary(long distance) {
  char alpha;

  if (distance < 3) {alpha = 'a';}
  else if (distance < 5) {alpha = 'b';}
  else if (distance < 7) {alpha = 'c';}
  else if (distance < 9) {alpha = 'd';}
  else if (distance < 11) {alpha = 'e';}
  else if (distance < 13) {alpha = 'f';}
  else if (distance < 15) {alpha = 'g';}
  else if (distance < 17) {alpha = 'h';}
  else if (distance < 19) {alpha = 'i';}
  else if (distance < 21) {alpha = 'j';}
  else if (distance < 23) {alpha = 'k';}
  else if (distance < 25) {alpha = 'l';}
  else if (distance < 27) {alpha = 'm';}
  else if (distance < 29) {alpha = 'n';}
  else if (distance < 31) {alpha = 'o';}
  else if (distance < 33) {alpha = 'p';}
  else if (distance < 35) {alpha = 'q';}
  else if (distance < 37) {alpha = 'r';}
  else if (distance < 39) {alpha = 's';}
  else if (distance < 41) {alpha = 't';}
  else if (distance < 43) {alpha = 'u';}
  else if (distance < 45) {alpha = 'v';}
  else if (distance < 47) {alpha = 'w';}
  else if (distance < 49) {alpha = 'x';}
  else if (distance < 51) {alpha = 'y';}
  else {alpha = 'z';}

  return alpha;
}

void ISR1() {
  
  curMillis = millis();
  // Debounce delay for pushbutton
  if (curMillis - lastInterruptTime > 100) {
    lastInterruptTime = curMillis;
    if (ind == 15) {
      ind = 0;
      sentence = "";
    }
    else {
      ind += 1;
    }
  }
}


void ISR2() {
  
  curMillis = millis();
  // Debounce delay for pushbutton
  if (curMillis - lastInterruptTime > 100) {
    lastInterruptTime = curMillis;
    
    for (int i = 0; i < sentence.length(); i++) {
      Serial.write(sentence[i]);
    }
    Serial.write('\n');

    sentence = "";
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    ind = 0;
    prevInd = 0;
  }
}
