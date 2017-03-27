/*
 * for use with pulseIn_sender sketch, to test the fidelity and
 * accuracy of the Arduino's pulse width reading.
 * 
 * I used two knockoff Nanos for testing.
 * 
 * testing on 3/24/17 demonstrated that most reads came through
 * clear, with about 4 microseconds of variation (2 in each 
 * direction) from the modal value being transmitted. I have not
 * run actual statistics on the received values.
 * 
 * receives pulses on pin 8. Be sure the Arduinos' grounds
 * are tied together for good data transmission.
 */

void setup() {
  pinMode(8, INPUT);
  Serial.begin(9600);
}

void loop() {
  int in = pulseIn(8, HIGH);
  if (in) Serial.println(in);
}
