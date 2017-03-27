/*
   for use with pulseIn_receiver sketch

   sends pulses to another Arduino, to test for fidelity and
   accuracy of pulse width measurement

   I used two knockoff Nanos for testing.

   my testing on 3/24/17 found that using direct port access
   decreased pulse width by approx. 4 microseconds as compared
   with using digitalWrite commands.

   wire a switch from 12 to ground; close it to send a pulse

   attach pin 8 to the other Arduino, and tie the grounds together
*/

bool directPortAccess = true;
bool sendPattern = false;

void setup() {
  pinMode(12, INPUT_PULLUP);
  pinMode(8, OUTPUT);
  if (sendPattern) sendPattern();
}

void loop() {
  if (digitalRead(12) == LOW) {
    if (directPortAccess) PORTB = B1; // set 8 HIGH
    else digitalWrite(8, HIGH);
    delayMicroseconds(1000);
    if (directPortAccess) PORTB = B10000; // set 8 LOW and return 12 to HIGH
    else digitalWrite(8, LOW);
    delay(50);
  }
}

void sendPattern() {

  // test sending 100 100's
  for (int i = 0; i < 100; i++){
    if (directPortAccess) PORTB = B1; // set 8 HIGH
    else digitalWrite(8, HIGH);
    delayMicroseconds(100);
    if (directPortAccess) PORTB = B10000; // set 8 LOW and return 12 to HIGH
    else digitalWrite(8, LOW);
    delay(2);
  }

  // test sending 100 500's
  for (int i = 0; i < 100; i++){
    if (directPortAccess) PORTB = B1; // set 8 HIGH
    else digitalWrite(8, HIGH);
    delayMicroseconds(500);
    if (directPortAccess) PORTB = B10000; // set 8 LOW and return 12 to HIGH
    else digitalWrite(8, LOW);
    delay(2);
  }

  // test sending 100 1000's
  for (int i = 0; i < 100; i++){
    if (directPortAccess) PORTB = B1; // set 8 HIGH
    else digitalWrite(8, HIGH);
    delayMicroseconds(1000);
    if (directPortAccess) PORTB = B10000; // set 8 LOW and return 12 to HIGH
    else digitalWrite(8, LOW);
    delay(2);
  }
  
  // test incrementing by single microseconds
  for (int i = 10; i < 1000; i++) {
    if (directPortAccess) PORTB = B1; // set 8 HIGH
    else digitalWrite(8, HIGH);
    delayMicroseconds(i);
    if (directPortAccess) PORTB = B10000; // set 8 LOW and return 12 to HIGH
    else digitalWrite(8, LOW);
    delay(2);
  }

  // test incrementing by 5 microseconds
  for (int i = 10; i < 5000; i = i + 5) {
    if (directPortAccess) PORTB = B1; // set 8 HIGH
    else digitalWrite(8, HIGH);
    delayMicroseconds(i);
    if (directPortAccess) PORTB = B10000; // set 8 LOW and return 12 to HIGH
    else digitalWrite(8, LOW);
    delay(10);
  }
}

void sendPulse(int width){
   if (directPortAccess) PORTB = B1; // set 8 HIGH
    else digitalWrite(8, HIGH);
    delayMicroseconds(width);
    if (directPortAccess) PORTB = B10000; // set 8 LOW and return 12 to HIGH
    else digitalWrite(8, LOW);
    delayMicroseconds(width * 10);
}

