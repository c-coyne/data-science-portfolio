// Include the needed libraries to support the LCD screens
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD

// Set up LCD screens
LiquidCrystal_I2C lcd1 = LiquidCrystal_I2C(0x21, 20, 4); // Change to (0x27,16,2) for 16x2 LCD.
LiquidCrystal_I2C lcd2 = LiquidCrystal_I2C(0x27, 20, 4); // Change to (0x27,16,2) for 16x2 LCD.

// SET UP ARDUINO I/O PINS

// Analog pins
int voltSense = A0;   // Input
int potPinRamp = A1;  // Input
int potPinTest = A2;  // Input
int potPinRPM = A3;   // Input
// Note: LCD1 (top screen) SDA is connected to A4
// Note: LCD1 (top screen) SCL is connected to A5

// Digital pins
// Note: pin 0 is reserved for the attached interrupt for encoder A from the motor
// Note: pin 1 is reserved for the attached interrupt for encoder B from the motor
int motEnA = 2;       // Input
int motEnB = 3;       // Input
int buttonSFTY = 4;   // Input
int buttonPRINT = 5;  // Input
int buttonSD = 6;     // Input
int buttonSTART = 7;  // Input
int speakers = 8;     // Output
int PWM = 9;          // Output
int HBridge1 = 10;    // Output
int HBridge2 = 11;    // Output
int limSwitch = 12;   // Input
// Note: LCD2 (bottom screen) SDA is connected to SDA
// Note: LCD2 (bottom screen) SCL is connected to SCL

// SET UP VARIABLES USED

// Safety measure
int okay_to_start = 0;

// Potentiometer knobs
long RAMP_read = 0;
long RAMP_write = 0;
long RAMP_new = 0;
long TEST_read = 0;
long TEST_write = 0;
long TEST_new = 0;
long RPM_read = 0;
long RPM_write = 0;
long RPM_new = 0;
// Start button
int prev_start = 0;
int now_start = 0;

// Computation of position and velocity
volatile long motorPosition = 0;  // [encoder counts] Current motor position (Declared 'volatile', since it is updated in a function called by interrupts)
volatile int encoderStatus = 0;   // [binary] Past and current A&B values of the encoder
// The rightmost two bits of encoderStatus will store the encoder values from the current iteration (A and B)
// The two bits to the left of those will store the encoder values from the previous iteration (A_old and B_old)
float max_RPM = 1000;             // [RPM] maximum rotational speed of the motor
float motor_speed_PWM = 0;        // [0-255] Pulse-width modulation (PWM) signal corresponding to selected rotational speed
float incr = 0;                   // [0-255] Used to increment the motor speed to achieve the selected rotational speed in the selected acceleration window
float PWM_signal = 0;             // [0-255] PWM signal sent to the H-bridge to control voltage supplied to the motor
float motorVelocity = 0;          // [encoder counts / second] Current motor velocity
float motorVelocity_num = 0;
float motorVelocity_denom = 0;
long previousMotorPosition = 0;   // [encoder counts] Motor position the last time a velocity was computed
long previousVelCompTime = 0;     // [microseconds] System clock value the last time a velocity was computed

// Speed control
float KP = 0.50;                  // [] Proportional control parameter
float KI = 0.1;                   // [] Integral control parameter
float KD = 0.01;                  // [] Derivative control parameter
float time_step = 0;              // [microseconds] Time since previous error measurement
float err = 0;                    // [RPM] Difference between input velocity (RPM) and actual velocity (RPM)
float err_prev = 0;               // [RPM] Previous measurement of velocity error
float cum_err = 0;                // [RPM * sec] Integral error = error multiplied by time
float rate_err = 0;               // [RPM / sec] Derivative error = error divided by time
float PID_RPM = 0;                // [RPM] updated RPM based on error values
float output = 0;                 // [RPM] PID output
float disp_time = 0;              // [sec] Variable used to display speed results every second
float lambda = 1.0;               // [unitless] correction factor to allow output RPM to better match input RPM

/////////////////////////////////////////////////////////////////////
// This is a function to update the encoder count in the Arduino.  //
// It is called via an interrupt whenever the value on encoder     //
// channel A or B changes.                                         //
/////////////////////////////////////////////////////////////////////

void updateMotorPosition() {
  // Bitwise shift left by one bit, to make room for a bit of new data:
  encoderStatus <<= 1;
  // Use a compound bitwise OR operator (|=) to read the A channel of the encoder (pin 2)
  // and put that value into the rightmost bit of encoderStatus:
  encoderStatus |= digitalRead(motEnA);
  // Bitwise shift left by one bit, to make room for a bit of new data:
  encoderStatus <<= 1;
  // Use a compound bitwise OR operator (|=) to read the B channel of the encoder (pin 3)
  // and put that value into the rightmost bit of encoderStatus:
  encoderStatus |= digitalRead(motEnB);
  // encoderStatus is truncated to only contain the rightmost 4 bits by using a 
  // bitwise AND operator on mstatus and 15(=1111):
  encoderStatus &= 15;
  if (encoderStatus==2 || encoderStatus==4 || encoderStatus==11 || encoderStatus==13)
  { // the encoder status matches a bit pattern that requires counting up by one
    motorPosition++; // increase the encoder count by one
  }
  else
  { // the encoder status does not match a bit pattern that requires counting up by one.
    // Since this function is only called if something has changed, we have to count downwards
    motorPosition--; // decrease the encoder count by one
  }
}

/////////////////////////////////////////////////////////////////////
// This is a function to calculate the velocity of the motor.      //
/////////////////////////////////////////////////////////////////////

void calculateVelocity() {
    motorVelocity_num = (double)((motorPosition - previousMotorPosition)/(double)640);
    motorVelocity_denom = (double)((micros() - previousVelCompTime)/(double)1000000);
    motorVelocity = abs(((double)motorVelocity_num / (double)motorVelocity_denom)*(double)60);
}

/////////////////////////////////////////////////////////////////////
// This is a function to write the values of the potentiometers    //
// to the LCD screen.                                              //
/////////////////////////////////////////////////////////////////////

void writeValues() {
  // RPM value
  lcd1.setCursor(1,0);
  lcd1.print("Speed [RPM]: ");
  RPM_write = round((1000*RPM_read)/1017);
  // Round to nearest 5
  RPM_write = floor(RPM_write / 5) * 5;
  lcd1.print(RPM_write);
  // Ramp value
  lcd1.setCursor(1,2);
  lcd1.print(" Ramp [sec]: ");
  RAMP_write = round((10*RAMP_read)/1015);
  lcd1.print(RAMP_write);
  // Test value
  lcd1.setCursor(1,1);
  lcd1.print(" Test [sec]: ");
  TEST_write = round((60*TEST_read)/1015);
  lcd1.print(TEST_write);
}

/////////////////////////////////////////////////////////////////////
// This is a function to conduct PID control on motor speed.       //
/////////////////////////////////////////////////////////////////////

void PID() {
  time_step = (double)((micros() - previousVelCompTime)/(double)1000000);
  // (KP) Proportional error
  err = motorVelocity - RPM_write;
  // (KI) Integral error
  cum_err = (double)(time_step * err);
  // (KD) Derivative error
  rate_err = (double)((err - err_prev) / time_step);
  output = (KP * err) + (KI * cum_err) + (KD * rate_err);
  PID_RPM -= output;
  err_prev = err;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// The setup() function is called when a sketch starts. It is used to intialize variables   //
// and pin modes, start using libraries, etc. The setup function will only run once, after  //
// each powerup or reset of the Arduino board.                                              //
//////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  // Initialize, turn on the backlight, and clear both LCD screens
  lcd1.init();
  lcd2.init();
  lcd1.backlight();
  lcd2.backlight();
  lcd1.clear();
  lcd2.clear();

  // Tell Arduino which pins are inputs and which are outputs
  pinMode(voltSense, INPUT);
  pinMode(potPinRamp, INPUT);
  pinMode(potPinTest, INPUT);
  pinMode(motEnA, INPUT);
  pinMode(motEnB, INPUT);
  pinMode(buttonSFTY, INPUT);
  pinMode(buttonPRINT, INPUT);
  pinMode(buttonSD, INPUT);
  pinMode(speakers, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(HBridge1, OUTPUT);
  pinMode(HBridge2, OUTPUT);
  pinMode(limSwitch, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Turn on the internal pull-up resistors on the encoder channels
  digitalWrite(motEnA, HIGH);
  digitalWrite(motEnB, HIGH);

  // Activate interrupts for encoder pins
  // If either of the two pins changes, the function 'updateMotorPosition' is called
  attachInterrupt(0, updateMotorPosition, CHANGE);
  attachInterrupt(1, updateMotorPosition, CHANGE);

  // Set initial motor speed to 0
  analogWrite(PWM, 0);

  // Write initial potentiometer values to the LCD screen
  RAMP_read = analogRead(potPinRamp);
  TEST_read = analogRead(potPinTest);
  RPM_read = analogRead(potPinRPM);
  writeValues();
}

void loop() {

  // Read safety setup
  // If (1) SFTY switch is engaged and cover is in place, or (2) SFTY switch is not engaged, give the OK for testing
  if ((digitalRead(buttonSFTY) == HIGH && digitalRead(limSwitch) == HIGH) || (digitalRead(buttonSFTY) == LOW)) {
    okay_to_start = 1;
  } else {
    okay_to_start = 0;
  }

  // Read potentiometer values
  RAMP_read = analogRead(potPinRamp);
  TEST_read = analogRead(potPinTest);
  RPM_read = analogRead(potPinRPM);
  // Read START button value
  prev_start = digitalRead(buttonSTART);

  // Wait 1/2 second
  delay(500);

  // Read values again to detect for change
  RAMP_new = analogRead(potPinRamp);
  TEST_new = analogRead(potPinTest);
  RPM_new = analogRead(potPinRPM);
  // If values have changed, display new values
  if ((abs(RAMP_new - RAMP_read) > 2) || (abs(TEST_new - TEST_read) > 2) || (abs(RPM_new - RPM_read) > 2)) {
    lcd1.clear();
    RAMP_read = analogRead(potPinRamp);
    TEST_read = analogRead(potPinTest);
    RPM_read = analogRead(potPinRPM);
    writeValues();
  }

  // Read START button value again
  now_start = digitalRead(buttonSTART);

  // If the START button is in a new state, run the test
  if ((prev_start == 1 && now_start == 0) || (prev_start == 0 && now_start == 1)) {
    if (okay_to_start == 0) {
      lcd2.setCursor(0,0);
      lcd2.print("Please install cover");
      lcd2.setCursor(0,1);
      lcd2.print("prior to testing");
      delay(2000);
      lcd2.clear();
    }
    else {
    // Beep and show 3-second countdown to indicate start of test
    tone(speakers, 440, 500);
    lcd2.setCursor(0,0);
    lcd2.print("Starting test in: ");
    // Sound the speakers to indicate beginning of test
    for (int i = 3; i > 0; i--) {
      lcd2.setCursor(18,0);
      lcd2.print(i);
      delay(1000);
    }

    // Start the test
    // Tell the H-bridge which way to flow current
    digitalWrite(HBridge1, HIGH);
    digitalWrite(HBridge2, LOW);

    ////////////////////////////////////////////////////////////////////////////////////////
    // ACCELERATION PHASE
    // Accelerate to speed per user selections
    // Write to the screen to alert user to acceleration
    lcd2.clear();
    lcd2.setCursor(0,0);
    lcd2.print("Starting");
    lcd2.setCursor(0,1);
    lcd2.print("acceleration");
    lcd2.setCursor(0,2);
    lcd2.print("t = ");
    lcd2.setCursor(0,3);
    lcd2.print("RPM = ");

    // Accelerate the motor to selected rotational speed
    motor_speed_PWM = (RPM_write / max_RPM) * 255 * lambda;
    incr = motor_speed_PWM / (10*RAMP_write);
    PWM_signal = 0;
    for (int i = 0; i <= (10*RAMP_write); i++) {
      analogWrite(PWM, PWM_signal);
      if (i % 10 == 0) {
        lcd2.setCursor(4,2);
        lcd2.print(i/10);
        lcd2.setCursor(6,3);
        lcd2.print(round((PWM_signal * max_RPM) / 255));
        }
      previousMotorPosition = motorPosition;
      previousVelCompTime = micros();
      // calculateVelocity();
      PWM_signal += incr;
      delay(100);
    }

    delay(900);

    ////////////////////////////////////////////////////////////////////////////////////////
    // CONSTANT SPEED PHASE
    // Beep to signal end of the acceleration phase and beginning of constant-speed phase
    tone(speakers, 659, 500);
    
    // The end of the previous loop added an extra incr to PWM_signal, so it is removed below
    // before applying PID control and monitoring RPM at constant speed
    PWM_signal = PWM_signal - incr;

    // Maintain constant speed for duration of test
    lcd2.clear();
    lcd2.setCursor(0,0);
    lcd2.print("Testing at");
    lcd2.setCursor(0,1);
    lcd2.print("constant speed");
    lcd2.setCursor(0,2);
    lcd2.print("t = ");
    lcd2.setCursor(0,3);
    lcd2.print("Current RPM = ");
    
    PID_RPM = RPM_write;

    // If shutdown button is engaged, skip the constant speed phase
    if (digitalRead(buttonSD) == HIGH) { delay(1); }
    // Otherwise, conduct the constant speed phase as normal
    else {
    for (int i = 1; i <= TEST_write; i++) {
      // Calculate the velocity of the motor
      calculateVelocity(); 
      // Update the variables used to calculate velocity
      previousMotorPosition = motorPosition;
      previousVelCompTime = micros();
      // Calculate the error
      // PID();
      // Write the error-adjusted RPM to the motor
      PWM_signal = (PID_RPM / max_RPM) * 255;
      if (PWM_signal >= 255) {
        PWM_signal = 255;
        }
      analogWrite(PWM, PWM_signal);
      
      lcd2.setCursor(4, 2);
      lcd2.print(i);
      lcd2.setCursor(14,3);
      if (digitalRead(buttonPRINT) == HIGH) {
        lcd2.print(round(motorVelocity));
      } else {
        lcd2.print(round((PWM_signal * max_RPM) / 255));
      }
      delay(1000);
    } }

    ////////////////////////////////////////////////////////////////////////////////////////
    // DECELERATION PHASE
    // Decelerate per user input
    // Write to the screen to alert user to acceleration
    lcd2.clear();
    lcd2.setCursor(0,0);
    lcd2.print("Starting");
    lcd2.setCursor(0,1);
    lcd2.print("deceleration");
    lcd2.setCursor(0,2);
    lcd2.print("t = ");
    lcd2.setCursor(0,3);
    lcd2.print("RPM = ");

    // Set up the deceleration increment
    incr = PWM_signal / (10*RAMP_write);
    
    // Decelerate the motor back to stationary
    for (int i = 0; i <= (10*RAMP_write); i++) {
      analogWrite(PWM, PWM_signal);
      if (i % 10 == 0) {
        lcd2.setCursor(4,2);
        lcd2.print(i/10);
        lcd2.setCursor(6,3);
        lcd2.print(round((PWM_signal * max_RPM) / 255));
        }
      // previousMotorPosition = motorPosition;
      // previousVelCompTime = micros();
      // calculateVelocity();
      PWM_signal -= incr;
      if (PWM_signal < 0) {
        PWM_signal = 0;
        }
      delay(100);
    }

    delay(900);

    // Turn off the motor
    digitalWrite(HBridge1, LOW);
    digitalWrite(HBridge2, LOW);

    // Beep to indicate completion of test
    tone(speakers, 440, 500);
  }
  }
  else {
    lcd2.clear();
  }

}
