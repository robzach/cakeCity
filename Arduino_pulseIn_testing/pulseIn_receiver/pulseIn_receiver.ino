/*
   Read input data from digital output line in ABB robot and move linear actuator accordingly

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

   4/26/17
    - changed pin assignments to match motor driver as wired
    - added motor motion commands

   4/27/17
    - added different retract speeds for testing with icing on the robot (using PWM)
    - added serial input for debugging, and initialization message listing commands for user
    - tried adding non-PWM speed control but it's not steady

   4/28/17
    - added exponentially decaying analogRead-smoothing function, smoothedPos()
    - tried to add function to run a lower speed, but it doesn't run consistently (mode 3 below);
        it seems to place a heavy current demand on the power supply, which conks out for a second
        or so during the drive, leading to movement in fits and starts

   5/1/17 Mayday! in more ways than one
    - fixed previous three dates which were off by a month
    - using motor_pos_from_serial_command as template for movement because it works reliably
    - main loop runs at 100 Hz
    - added jog buttons though they're not yet tested

   Robert Zacharias, rz@rzach.me
   released by the author to the public domain
*/

bool debug = true;

const byte READPIN = 2; // do not change this casually; it needs to be an interrupt pin
// and also the PIND port access bitmask below assumes this will be pin 2

int FUZZ = 1000; // number of microseconds to fudge on either side of pulse width result
volatile unsigned long diff; // volatile because it will be affected by the ISR
volatile byte mode = 0; // global to drive motorMode (will be set by ISR)

// target pulse width in microseconds of to-be-defined commands. Array length can be changed,
// but check that motorMode reasonably matches
unsigned long COMMAND[] = {10000, 20000, 30000, 40000, 50000, 60000};
int COMMANDLENGTH = sizeof(COMMAND) / sizeof(unsigned long);

const int MOTORPINA = 4;
const int MOTORPINB = 6;
const int PWMPIN = 3;
const int POSPIN = A0;
const int JOGEXTEND = 10;
const int JOGRETRACT = 8;

const int PWMMIN = 32;
const int PWMMAX = 128;
const int MAXPOS = 850; // found empirically on this device; approx. 8" of extension
const int MINPOS = 12;  // found empirically; fully retractede


float posCommand = MAXPOS; // variable to store movement command; defaults to fully extended

String INITIALIZATIONMESSAGE = "\nAvailable serial commands: \n\t0 through n will set the motor to the correlating mode as defined in motorMode switch case\n\ts stops the motor\n\te extends the motor\n\tr retracts the motor\n\n";

void setup() {
  pinMode(READPIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(READPIN), readPulse, CHANGE);
  posCommand = analogRead(POSPIN); // initialize value appropriately
  Serial.begin(115200);
  pinMode(MOTORPINA, OUTPUT);
  pinMode(MOTORPINB, OUTPUT);
  pinMode(PWMPIN, OUTPUT);
  digitalWrite(PWMPIN, HIGH);
  pinMode(JOGEXTEND, INPUT_PULLUP);
  pinMode(JOGRETRACT, INPUT_PULLUP);
  Serial.println(INITIALIZATIONMESSAGE);
}

void loop() {
  static unsigned long loopTime = 0;
  int wait = 10; // milliseconds between loops
  if (millis() - loopTime > wait) {
    motorMode(mode);
    loopTime = millis();
  }

  static bool wasJustJogging = false;

  while (digitalRead(JOGEXTEND) == LOW) {
    analogWrite(PWMPIN, 64);
    extend();
    Serial.print("jog extend command to "); Serial.println(posCommand);
    wasJustJogging = true;
  }
  while (digitalRead(JOGRETRACT) == LOW) {
    analogWrite(PWMPIN, 64);
    retract();
    Serial.print("jog retract command to "); Serial.println(posCommand);
    wasJustJogging = true;
  }
  if(wasJustJogging){
    off();
    wasJustJogging = false;
  }
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
      for (int i = 0; i < COMMANDLENGTH; i++) {
        if ( abs(diff - COMMAND[i]) < FUZZ ) {
          mode = i;
          break; // no need to continue loop if it's already found a motorMode
        }
      }
      if (debug) {
        Serial.print("pulse width received = ");
        Serial.print(diff);
        Serial.print("\tmotorMode = ");
        Serial.println(mode);
      }
    }
    lastState = readState;
  }
}

void motorMode(byte in) { // based off of motor_pos_from_serial_command sketch

  static unsigned long lastMoveTime = 0;
  float pos = smoothedPos();
  int PWMsignal = constrain(map(abs(pos - posCommand), 0, 50, PWMMIN, PWMMAX), PWMMIN, PWMMAX);

  digitalWrite(PWMPIN, HIGH);

  switch (in) {
    case 0: // stop
      off();
      break;
    case 1: // full speed retract
      retract();
      posCommand = pos; // update for starting point of rate-based retract
      break;
    case 2: // full speed extend
      extend();
      posCommand = pos; // update for starting point of rate-based retract
      break;
    case 3: // rate-based retract
      //    5/1/17 took 4:20 to run full length of empty extruder @ travelWait 200 and retractVal 0.5
      {
        int travelWait = 200;   // millisecond wait between steps
        float retractVal = 0.5; // decrement amount
        if (millis() - lastMoveTime > travelWait) {
          posCommand -= retractVal;
          if (posCommand < MINPOS) posCommand = MINPOS;
          lastMoveTime = millis();
        }
        if (posCommand < pos) {
          analogWrite(PWMPIN, PWMsignal);
          retract();
        }
        else off();

        if (debug) {
          Serial.print("posCommand = ");
          Serial.print(posCommand);
          Serial.print("\tpos = ");
          Serial.print(pos);
          Serial.print("\tMOTORPINA = ");
          Serial.print(digitalRead(MOTORPINA));
          Serial.print("\tMOTORPINB = ");
          Serial.print(digitalRead(MOTORPINB));
          Serial.print("\tPWMsignal = ");
          Serial.println(PWMsignal);
        }

        break;
      }
    case 4: // quarter speed retract using PWM
      analogWrite(PWMPIN, 64);
      retract();
      break;
    case 5: // eighth speed retract using PWM
      analogWrite(PWMPIN, 32);
      retract();
      break;
    default: // stop motor
      off();
      break;
  }
}

void off() {
  digitalWrite(MOTORPINA, LOW);
  digitalWrite(MOTORPINB, LOW);
}

void extend() {
  digitalWrite(MOTORPINA, HIGH);
  digitalWrite(MOTORPINB, LOW);
}

void retract() {
  digitalWrite(MOTORPINA, LOW);
  digitalWrite(MOTORPINB, HIGH);
}

void serialEvent() {
  while (Serial.available()) {

    int in = Serial.read();

    // read numerical value if that's what's transmitted
    for (int i = 0; i < COMMANDLENGTH; i++) {
      int val = in - 48; // 48 is the value of '0' in ascii
      if (val == i) {
        mode = val;
        Serial.print("received serial command: "); Serial.println(val);
        break; // no need to finish loop if a match is found
      }
    }

    // read letter command if that's what's transmitted
    if (in == 's') { // stop
      mode = 0;
      Serial.println("received serial command 's' for stop");
    }
    else if (in == 'e') { // extend
      mode = 2;
      Serial.println("received serial command 'e' for extend");
    }
    else if (in == 'r') { // retract
      mode = 1;
      Serial.println("received serial command 'r' for retract");
    }
  }
}

float smoothedPos() {
  static float smooth = (float)analogRead(POSPIN);
  float decay = 0.2;
  float reading = (float)analogRead(POSPIN);
  smooth = (smooth * (1 - decay)) + (reading * decay);
  return smooth;
}











