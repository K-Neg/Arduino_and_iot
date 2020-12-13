//IOs
#define led 1
#define motor 0
#define button 4
void turn_on() {
  // Accelerate
  for (int i = 0; i <= 255; i++) {
    analogWrite(motor, i);
    analogWrite(led, i);
    delay(20);
  }
  //keep on 2s and turn off
  delay(2000);
  analogWrite(motor, 0);
  analogWrite(led, 0);
}
void setup() {
  // Set up pins
  pinMode(button, INPUT);
  pinMode(motor, OUTPUT);
  pinMode(led, OUTPUT);
  //turn off everything
  analogWrite(motor, 0);
  analogWrite(led, 0);
}
void loop() {
  //check if button is pressed
  if (!digitalRead(button)) turn_on();
  delay(10);
}
