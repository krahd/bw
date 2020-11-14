int enaPin = 5;
int dirPin = 6;
int stepPin = 7;

#define MAX_BPMS 40
#define INITIAL_DELAY 2000

int totalBpms = 0;
float bpms[MAX_BPMS];  // bpms
unsigned long dt[MAX_BPMS];    // # of milliseconds between beats
unsigned long t[MAX_BPMS];     // time (millis()) of last pull for this heart

unsigned long startTime;

unsigned long elapsedTime = 0;

void setup() {

  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  digitalWrite(enaPin, HIGH);

  Serial.begin(9600);

  startTime = millis();

  addBpm (98.6);
  addBpm (60.4);
  addBpm (80.4);
  addBpm (75.3);

  for (int i = 0; i < totalBpms; i++) {
    Serial.print ("dt[");
    Serial.print (i);
    Serial.print ("]: ");
    Serial.println (dt[i]);
    Serial.print ("t[");
    Serial.print (i);
    Serial.print ("]: ");
    Serial.println (t[i]);
  }

}

// beats
float calcDt(float b) {
  return 1 / (b / 60000);
}

void addBpm (float nb) {

  // add a new bpm
  if (totalBpms < MAX_BPMS) {
    bpms[totalBpms] = nb;
    dt[totalBpms] = calcDt(nb);
    t[totalBpms] = elapsedTime;
    totalBpms++;

  } else {

    // if the array is 'full' then we replace a random bpm
    int place = random (MAX_BPMS);
    bpms [place] = nb;
    dt[place] = calcDt(nb);
    t[place] = elapsedTime;
  }
}

void pull () {
  if (millis() > INITIAL_DELAY) {
    Serial.println (" ***************************************");
  }
}

void loop() {

  elapsedTime = millis() - startTime;

  if (elapsedTime >= 60000) { // one minute passed
    startTime = millis();
    Serial.print ("-----------------------------------------------------------------");

  } else {
    // if under a minute we check if we are at the corresponding time of a bpm

    for (int i = 0; i < MAX_BPMS; i++) {
      if (elapsedTime - t[i] == dt[i]) {
        Serial.print(i);
        t[i] = elapsedTime;
        pull();
      }
    }

  }
}
