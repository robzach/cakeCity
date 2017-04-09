/*
   for use with pulseIn_receiver sketch

   sends pulses to another Arduino, to test for fidelity and
   accuracy of pulse width measurement

   I used two knockoff Nanos for testing.

   my testing on 3/24/17 found that using direct port access
   decreased pulse width by approx. 4 microseconds as compared
   with using digitalWrite commands.

   4/5/17
    - modified to send longer pulses, at 10, 20, 30 milliseconds
      since those are the expected control pulse values for our project
    - now using unsigned long data type for pulse width

   wire a switch from 12 to ground; close it to send a pulse

   attach pin 8 to the other Arduino's input, and tie the grounds together
*/

bool directPortAccess = true;
bool sendPattern = true;

void setup() {
  pinMode(12, INPUT_PULLUP);
  pinMode(8, OUTPUT);
  if (sendPattern) pattern();
}

void loop() {
  if (digitalRead(12) == LOW) sendPulse(1000);
}


void sendPulse(unsigned long width){
   if (directPortAccess) PORTB = B1; // set 8 HIGH
    else digitalWrite(8, HIGH);
    delayMicroseconds(width);
    if (directPortAccess) PORTB = B10000; // set 8 LOW and return 12 to HIGH
    else digitalWrite(8, LOW);
    delayMicroseconds(10000); // millisecond pause between pulses
}

void pattern() {

  // test sending 100 10000's
  for (int i = 0; i < 100; i++){
    sendPulse(10000);
  }

  // test sending 100 20000's
  for (int i = 0; i < 100; i++){
    sendPulse(20000);
  }

  // test sending 100 30000's
  for (int i = 0; i < 100; i++){
    sendPulse(30000);
  }
  
}


