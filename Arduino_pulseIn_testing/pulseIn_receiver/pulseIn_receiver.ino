/*
   Read input data from digital output line in ABB robot

   3/30/17 revising to explicitly use interrupts since some of the data
   coming through testing with Josh Bard on the robot was questionable.
   (The testing was using a prior version that simply read pulseIn and printed
   the result to the serial terminal; that version's source as well as a long
   stream of test results is available in pulseIn_receiver-testing-3-31-17.txt)

   4/5/17
    - changed data types to unsigned longs
    - fix for() loop limit by adding division by sizeof(unsigned long) to limit
    - Garth helped a lot with cleaning up the ISR
    - changed mode datatype to byte since it will usually be around 5 maximum
    - added debug mode toggled by global boolean for serial feedback

   Robert Zacharias, rz@rzach.me
   released by the author to the public domain
*/

bool debug = true;

const byte READPIN = 2; // do not change this casually; it needs to be an interrupt pin
// and also the PIND port access bitmask below assumes this will be pin 2

int FUZZ = 500; // number of microseconds to fudge on either side of pulse width result
volatile unsigned long diff; // volatile because it will be affected by the ISR
volatile byte mode = 0; // global to drive motorMode (will be set by ISR)

// target pulse width in microseconds of to-be-defined commands. Array length can be changed,
// but check that motorMode reasonably matches
unsigned long COMMAND[] = {10000, 20000, 30000, 40000};

void setup() {
  pinMode(READPIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(READPIN), readPulse, CHANGE);
  Serial.begin(115200);
}

void loop() {
  motorMode(mode);
}

void readPulse() {
  static unsigned long startTime = 0;
  static uint8_t lastState = 0; // state of the pin last time the interrupt was triggered

  uint8_t readState = (PIND & (0b00000100)); // port access is quicker than digitalRead

  if (readState != lastState) {
    if (readState) startTime = micros(); // if just went high, start timer
    else {
      diff = micros() - startTime; // if just went low, stop timer and count difference
      // drive motor based on pulse read
      for (int i = 0; i < sizeof(COMMAND) / sizeof(unsigned long); i++) {
        if ( abs(diff - COMMAND[i]) < FUZZ ) {
          mode = i;
          break; // no need to continue loop if it's already found a motorMode
        }
      }
      if (debug) {
        Serial.print("diff = ");
        Serial.print(diff);
        Serial.print("\tmotorMode = ");
        Serial.println(mode);
      }
    }
    lastState = readState;
  }
}

void motorMode(byte in) {
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

