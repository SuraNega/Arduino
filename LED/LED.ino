int LEDpin1 = 13; // First LED
int LEDpin2 = 12; // Second LED
int delayT1 = 1500; // Delay for the first LED
int delayT2 = 500; // Delay for the second LED

void setup() {
  // put your setup code here, to run once:
  pinMode(LEDpin1, OUTPUT);
  //pinMode(LEDpin2, OUTPUT);
}

void loop() {
  // Blink the first LED
  digitalWrite(LEDpin1, HIGH);
  delay(delayT1);
  digitalWrite(LEDpin1, LOW);
  delay(delayT1);

  // Blink the second LED
  digitalWrite(LEDpin2, HIGH);
  delay(delayT2);
  digitalWrite(LEDpin2, LOW);
  delay(delayT2);
}
