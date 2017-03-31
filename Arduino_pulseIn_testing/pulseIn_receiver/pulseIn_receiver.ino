/*
   Read input data from digital output line in ABB robot

   3/30/17 revising to explicitly use interrupts since some of the data
   coming through testing with Josh Bard on the robot was questionable.
   (The testing was using a prior version that simply read pulseIn and printed
   the result to the serial terminal; that version's source as well as a long
   stream of test results is available in pulseIn_receiver-testing-3-31-17.txt)

   Robert Zacharias, rz@rzach.me
   released by the author to the public domain
*/

const int READPIN = 2; // do not change this casually; it needs to be an interrupt pin and also the PIND port access bitmask below assumes this will be pin 2
long startTime, stopTime, diff;
int fuzz = 500; // number of microseconds to fudge on either side of pulse width result
bool lastState = 0; // state of the pin last time the interrupt was triggered

// target pulse width in microseconds of to-be-defined commands. Array length can be changed,
// but check that motorMode reasonably matches
int command[]= {2000, 4000, 6000, 8000};

void setup() {
  pinMode(READPIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(READPIN), readPulse, CHANGE);
  Serial.begin(115200);
}

void loop() {
  // drive motor based on pulse read
  for (int i = 0; i < sizeof(command); i++){
    if ( abs(diff-command[i]) < fuzz ) motorMode(i);
  }
  Serial.println(i);
}

void readPulse() {
//  int readState = digitalRead(READPIN); // digitalRead is slower
  int readState = (PIND & (0b00000100)); // port access is quicker
  if (readState == lastState) return; // error state: two rises or falls in a row
  if (readState) startTime = micros(); // if just went high, start timer
  else stopTime = micros(); // if just went low, stop timer
  diff = stopTime - startTime;
  lastState = readState;
}

void motorMode(int in){
  switch (in) {
    case 0:
      // drive motor in some way
      break;
    case 1:
      // drive motor in some way
      break;
    case 2:
      // drive motor in some way
      break;
    case 3:
      // drive motor in some way
      break;
    default:
      // stop motor 
      break;    
  }
}

