int enaPin = 5;
int dirPin = 6;
int stepPin = 7;
int potPin = A0;


void setup()
{
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  digitalWrite(enaPin, HIGH);
  pinMode(potPin, INPUT);
  Serial.begin(9600);
}


void loop()

{
  int potValue = analogRead (potPin);

  potValue = 256; // to test without pot
  
  Serial.println (potValue); // 0 .. 1023

  int steps = potValue * 2;  // 0 .. 2048
  int delayTime = 120;
  
  steps = random (10, 1500);

  int i;
  digitalWrite(dirPin, HIGH);
  

  for (i = 0; i < steps; i++) 
  {
    digitalWrite(stepPin, LOW);  
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayTime);  
  }

  digitalWrite(dirPin, LOW); // Change direction.
  

  for (i = 0; i < steps; i++) 
  {
    digitalWrite(stepPin, LOW);  
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayTime);  

  }

  
}
