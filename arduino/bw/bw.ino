int enaPin = 5;
int dirPin = 6;
int stepPin = 7;

#define MAX_BPMS 7
#define INITIAL_DELAY 2000
#define MIN_STEPS 600  // TOM TODO verify the tuning of these values
#define MAX_STEPS 3000
#define DEBUG false
#define DELAY_NEW_BPM 5000

int totalBpms = 0;
float bpms[MAX_BPMS];  // bpms
unsigned long dt[MAX_BPMS];    // # of milliseconds between beats
unsigned long t[MAX_BPMS];     // time (millis()) of last pull for this heart
unsigned long steps[MAX_BPMS]; // the number of steps that that heart will pull

unsigned long startTime;
unsigned long elapsedTime = 0;

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
  printAll();

}

// beats
float calcDt(float b) {
  return 1 / (b / 60000);
}

// adds one new bpm to the array
void addBpm (float nb) {

  // add a new bpm

  // if there's room add it to the end
  if (totalBpms < MAX_BPMS) {
    bpms[totalBpms] = nb;
    dt[totalBpms] = calcDt(nb);
    t[totalBpms] = elapsedTime;
    steps[totalBpms] = random (MIN_STEPS, MAX_STEPS); // each bpm has a random "strength"

    totalBpms++;

// if the array is 'full' then we replace a random bpm
  } else {
    
    int place = random (MAX_BPMS);
    bpms [place] = nb;
    dt[place] = calcDt(nb);
    t[place] = elapsedTime;
    steps[place] = random (MIN_STEPS, MAX_STEPS); // again, each bpm has a random "strength"    
  }

  delay (DELAY_NEW_BPM); // stop for n seconds
}

void printAll() {
  for (int i = 0; i < totalBpms; i++) {
    if (DEBUG) {
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
  }

  Serial.println ("-----------------------------");

}

void pull (int which) {
  int delayTime = 120;

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

    digitalWrite(dirPin, HIGH);
    for (int i = 0; i < steps[which]; i++) {
      digitalWrite(stepPin, LOW);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(delayTime);
    }

    digitalWrite(dirPin, LOW); // Change direction.

    for (int i = 0; i < steps[which]; i++) {
      digitalWrite(stepPin, LOW);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(delayTime);
    }

  }
}

void loop() {

  elapsedTime = millis() - startTime;

  if (elapsedTime >= 60000) { // one minute passed
    startTime = millis();
    Serial.println ("----------------------------[minute passed]-------------------------------------");

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
    // Serial.print ("received: ");  // if trying to print says port COM3 occupied
    // Serial.print (d);
    addBpm (d);

    printAll();
  }

  //printAll();
}