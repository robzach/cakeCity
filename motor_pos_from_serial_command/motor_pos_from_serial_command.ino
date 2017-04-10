/*
    Driving the linear actuator to a position command sent via serial monitor

    4-10-17
    It was likely an electrical fault in the on-board voltage regulator on the Nano, though
    I also had switched the wiring assignments of motor pin A and B (that itself wasn't
    the trouble).
    Today:
      - added proportional (PWM) movement control
      - adding analogRead smoothing function, which isn't done yet


    4-9-17
    As of this writing, I didn't get it to work properly and there's a smell of burnt electronics
    so I'm going to call it a night since I'm not sure what I did wrong
*/


const int MOTORPINA = 4;
const int MOTORPINB = 2;
const int PWMPIN = 3;
const int POSREADPIN = A0;

const int PWMMIN = 32;
const int PWMMAX = 64;

int posCommand = 0;
int pos = 0;

void setup() {
  Serial.begin(9600);
  pinMode(MOTORPINA, OUTPUT);
  pinMode(MOTORPINB, OUTPUT);
  pinMode(PWMPIN, OUTPUT);
}

void loop() {
  motorMove();
}

void motorMove() {
  int pos = analogRead(POSREADPN);
//  pos += int(0.5 * analogRead(POSREADPIN)); beginning of exponential decay smoothing?
  int PWMsignal = map(abs(pos - posCommand), 0, 900, PWMMIN, PWMMAX);
  
  if (posCommand > pos) {
    analogWrite(PWMPIN, PWMsignal);
    digitalWrite(MOTORPINA, HIGH);
    digitalWrite(MOTORPINB, LOW);
  }
  else if (posCommand < pos) {
    analogWrite(PWMPIN, PWMsignal);
    digitalWrite(MOTORPINA, LOW);
    digitalWrite(MOTORPINB, HIGH);
  }
  else {
    analogWrite(PWMPIN, 0);
    digitalWrite(MOTORPINA, LOW);
    digitalWrite(MOTORPINB, LOW);
  }
}

void serialEvent() {
  while (Serial.available()) {
    posCommand = Serial.parseInt();
    Serial.print("posCommand = ");
    Serial.print(posCommand);
    Serial.print("\tPOSREADPIN value = ");
    Serial.println(analogRead(POSREADPIN));
  }
}


