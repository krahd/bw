int enaPin = 5;
int dirPin = 6;
int stepPin = 7;

#define MAX_BPMS 20
#define INITIAL_DELAY 2000
#define MIN_STEPS 50  // TOM TODO verify the tuning of these values
#define MAX_STEPS 2500
#define DEBUG false
#define DELAY_NEW_BPM 3000
#define MIN_HEART 50
#define MAX_HEART 180

#define DELAY_TIME 120  // max speed at which the motor doesn't skip



int totalBpms = 0;
float bpms[MAX_BPMS];  // bpms
unsigned long dt[MAX_BPMS];    // # of milliseconds between beats
unsigned long t[MAX_BPMS];     // time (millis()) of last pull for this heart
unsigned long steps[MAX_BPMS]; // the number of steps that that heart will pull

unsigned long startTime;
unsigned long elapsedTime = 0;

#include "rgb_lcd.h"
#include <Wire.h>   // A4 (SDA), A5 (SCL)
rgb_lcd lcd;

const int colorR = 0;
const int colorG = 255;
const int colorB = 0;

void setup() {

  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  digitalWrite(enaPin, HIGH);

  Serial.begin(9600);

  startTime = millis();

  /*
    addBpm (98.6);
    addBpm (60.4);
    addBpm (80.4);
    addBpm (75.3);
    addBpm (70.3);
    addBpm (118.6);
    addBpm (85.4);
    addBpm (69.4);
    addBpm (77.3);
    addBpm (72.3);
  */

  if (DEBUG) {
    printAll();
  }

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  lcd.setRGB(colorR, colorG, colorB);

  // Print a message to the LCD.
  lcd.print("bw 0.3");
  delay (1000);

}

// beats
float calcDt(float b) {
  return 1 / (b / 60000);
}

// adds one new bpm to the array
void addBpm (float nb) {

  // safety: in case that the max at client and here are not the same.
  if (nb > MAX_HEART) {
    nb = MAX_HEART - random (1, 30);
  }

  // calculate corresponding steps
  unsigned long ns = map (nb, MIN_HEART, MAX_HEART, MAX_STEPS, MIN_STEPS);

  if (DEBUG) {
    Serial.print ("steps: ");
    Serial.println (ns);
  }

  // salt
  ns += random (-50, 50);

  //unsigned long ns = random (MIN_STEPS, MAX_STEPS); // each bpm woudl have random "strength"

  // this shouldn't happen
  if (ns <= 0) {
    lcd.setCursor (0, 0);
    lcd.print ("<= ZERO");
    ns = random (10, 40);
  }

  // if there's room add it to the end
  if (totalBpms < MAX_BPMS) {
    bpms[totalBpms] = nb;
    dt[totalBpms] = calcDt(nb);
    t[totalBpms] = elapsedTime;
    steps[totalBpms] = ns;

    totalBpms++;

    // if the array is 'full' then we replace a random bpm
  } else {

    int place = random (MAX_BPMS);
    bpms [place] = nb;
    dt[place] = calcDt(nb);
    t[place] = elapsedTime;
    steps[place] = ns;
  }

  lcd.setCursor (0, 1);
  lcd.print ("                ");
  lcd.setCursor (0, 1);
  lcd.print ("b");
  lcd.print (nb);
  lcd.print (" s");
  lcd.print (ns);

}

void printAll() {
  for (int i = 0; i < totalBpms; i++) {
    Serial.print ("dt[");
    Serial.print (i);
    Serial.print ("]: ");
    Serial.println (dt[i]);
    Serial.print ("t[");
    Serial.print (i);
    Serial.print ("]: ");
    Serial.println (t[i]);
    Serial.print ("elapsedTime: ");
    Serial.println (elapsedTime);
  }

  Serial.println ("-----------------------------");

}

void pull (int which) {
  if (millis() > INITIAL_DELAY) {

    if (DEBUG) {
      Serial.print (which);
      Serial.print ("]");
    }
    if (DEBUG) {
      for (int i = 0; i < steps[which] / 100; i++) {
        Serial.print ("*");
      }
      Serial.println ();
    }

    digitalWrite(dirPin, HIGH); // one direction
    for (int i = 0; i < steps[which]; i++) {
      digitalWrite(stepPin, LOW);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(DELAY_TIME);
    }

    digitalWrite(dirPin, LOW); // the other direction.
    for (int i = 0; i < steps[which]; i++) {
      digitalWrite(stepPin, LOW);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(DELAY_TIME);
    }

  }
}

void loop() {

  elapsedTime = millis() - startTime;

  if (elapsedTime >= 60000) { // one minute passed
    startTime = millis();
    if (DEBUG) {
      Serial.println ("----------------------------[minute passed]-------------------------------------");
    }

  } else {
    // if under a minute we check if we are at the corresponding time of a bpm

    for (int i = 0; i < totalBpms; i++) {
      if (elapsedTime - t[i] >= dt[i]) {
        t[i] = elapsedTime;
        pull(i);
      }
    }
  }



  if (Serial.available() > 0) {
    float d = Serial.parseFloat ();

    addBpm (d);
    delay (DELAY_NEW_BPM); // stop for n seconds

    if (DEBUG) {
      printAll();
    }
  }

  //printAll();
}
