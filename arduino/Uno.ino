// The pins defined in this program are for Arduino Uno.

/////////////////////////
// afancontrol firmware for Arduino Uno – PWM only
// No tachometer – fans remain connected to motherboard
// 25 kHz PWM on pins 9 and 10 (Timer1)
#include <TimerOne.h>

// TimerOne accepts PWM duty in range from 0 to 1023.
// The standard range for PWM fans on Linux is, however, from 0 to 255.
// This macros below does the conversion from 255 to 1023.
#define PWM_255_TO_1023(PWM) ((PWM == 0) ? 0 : (1L * PWM + 1) * 4 - 1)

// These are the pins connected to the Timer 1 on Arduino Uno. 
//Initialized with PWM of 10 
byte currentPWM9 = 10;
byte currentPWM10= 10;

// Macro to set a pins PWM value
#define SET_PWM(PIN, PWM) currentPWM##PIN = (PWM);
#define SET_PWM_HIGH(PIN) SET_PWM(PIN, 255)

// Macro to print a PWM pin's value in JSON format
#define PRINT_PWM_JSON(PIN) \
Serial.print("\""); \
Serial.print(PIN, DEC); \
Serial.print("\":"); \
Serial.print(currentPWM##PIN, DEC);


/////////////////////////
// Serial commands:
    
const char setSpeedCommand = '\xf1'; // The only supported command currently.
char commandBuffer[3];  // Buffer for the incoming command: [command; pin; speed].
int commandPosition = 0;  // The current position in the `commandBuffer`

/////////////////////////


//Execute 3 Byte Serial Commands
void processSerialCommand() {
  // assert (commandBuffer[0] == setSpeedCommand);

  byte pwm = (byte)commandBuffer[2];
  switch (commandBuffer[1]) {
    case 9:  SET_PWM(9,  pwm); break;
    case 10: SET_PWM(10, pwm); break;
    default:
      Serial.print("{\"error\": \"Unknown pin ");
      Serial.print((int)commandBuffer[1], DEC);
      Serial.print(" for the set speed command\"}\n");
  }
}
//Read 3 Byte serial Commands 
void readSerialCommand() {
  while (Serial.available()) {
    char c = Serial.read();
    if (commandPosition == 0 && c != setSpeedCommand) {
      Serial.print("{\"error\": \"Unknown command ");
      Serial.print(c, HEX);
      Serial.print("\"}\n");
      continue;
    }
    commandBuffer[commandPosition] = c;
    commandPosition++;
    if (commandPosition >= 3) {
      // The command buffer is now complete, process it:
      processSerialCommand();

      commandPosition = 0;
    }
  }
}

void setup() {
  // https://github.com/PaulStoffregen/TimerOne/blob/master/examples/FanSpeed/FanSpeed.pde
  Timer1.initialize(40); // 40microseconds == 25kHz

  SET_PWM_HIGH(9);
  SET_PWM_HIGH(10);
  
  Serial.begin(115200);
}

void loop () {

  Timer1.pwm(9, PWM_255_TO_1023(currentPWM9));
  Timer1.pwm(10, PWM_255_TO_1023(currentPWM10));
  // read incomming serial commands
  readSerialCommand();

  // Print the status (in JSON):

  Serial.print("{\"fan_pwm\": {");
  PRINT_PWM_JSON(9);
  Serial.print(",");
  PRINT_PWM_JSON(10);
  Serial.print("}}\n");
  //Update interval to avoid unnecessary serial traffic and clog up the Bus. 250ms is Arduino Firmware standard, lower values for more frequent pwm updates are possible  
  delay(250);
}
