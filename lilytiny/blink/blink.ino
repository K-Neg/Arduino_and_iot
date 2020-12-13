//IOs
#define led 1
void setup() {
  // set LED pin as a output
  pinMode(led, OUTPUT);
}
void loop() {
  //set time for blinking
  int time_blink = 1000;
  //Blink on/off
  digitalWrite(led, HIGH);
  delay(time_blink);
  digitalWrite(led, LOW);
  delay(time_blink);
}
