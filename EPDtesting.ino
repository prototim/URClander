
// uncomment next line to use class GFX of library GFX_Root instead of Adafruit_GFX
//#include <GFX.h>

#include <GxEPD2_BW.h>
//#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>

// Require keyboard control library
#include <KeyboardController.h>

// Initialize USB Controller
USBHost usb;

// Attach keyboard controller to USB
KeyboardController keyboard(usb);

byte keyPressCounter = 0;

//for output testing only with ISRs
int y = 0;
int x = 0;


//----------------SECTION FOR JOYSTICK AND GAUGE OUTPUT 3V------------------------------------------

#include <Servo.h>

Servo pan, tilt;

//timing periods
unsigned long time_now = 0;
const int periodServo = 500; //ms
const int periodScreen = 2000;
const int fiftyms = 50;

//initialize the servo variables
//tracking variables
byte azimuthPos = 0;
byte elevationPos = 0;
byte panPos = 0;
byte tiltPos = 0;

//Initialize
byte jogSpeed = 5; //what the incremental movement is
byte azimuthInitial = 40;
byte elevationInitial = 100;
byte panInitial = 100;
byte tiltInitial = 30;

//Limits and desired
byte gaugeMin = 0;
byte gaugeMax = 190;
byte servoMax = 160;
byte servoMin = 10;
byte desiredAzimuthMax = 20;
byte desiredElevationMax = 130;
byte desiredAzimuthMin = 10;
byte desiredElevationMin = 120;

//pin assignments for the joystick
const byte joyUp = 23;
const byte joyDown = 25;
const byte joyLeft = 22;
const byte joyRight = 24;

//pin assignment for the servos
const byte panServo = 2; //PWM pin
const byte tiltServo = 3; //PWM pin

//pin assignments for the azimuth and elevation meters
const byte azimuthGauge = 4; //PWM pin
const byte elevationGauge = 5; //PWM pin

//antennaAligned
//bool antennaAligned = false;

//variables to keep track of the timing of recent interrupts
unsigned long oxygenValve_time = 0;
unsigned long methaneValve_time = 0;
unsigned long turboPump_time = 0;
unsigned long landerLock_time = 0;
unsigned long ignitionEnabled_time = 0;
unsigned long masterArm_time = 0;
unsigned long telemetry_time = 0;
unsigned long initiateLaunch_time = 0;
unsigned long joyUp_time = 0;
unsigned long joyDown_time = 0;
unsigned long joyLeft_time = 0;
unsigned long joyRight_time = 0;

unsigned long last_oxygenValve_time = 0;
unsigned long last_methaneValve_time = 0;
unsigned long last_turboPump_time = 0;
unsigned long last_landerLock_time = 0;
unsigned long last_ignitionEnabled_time = 0;
unsigned long last_masterArm_time = 0;
unsigned long last_telemetry_time = 0;
unsigned long last_initiateLaunch_time = 0;
unsigned long last_joyUp_time = 0;
unsigned long last_joyDown_time = 0;
unsigned long last_joyLeft_time = 0;
unsigned long last_joyRight_time = 0;
//----------------END SECTION FOR JOYSTICK AND GAUGE OUTPUT 3V------------------------------------------




//----------------SECTION FOR SWITCHES------------------------------------------


//Switch inputs

const byte telemetrySwitch = 38;
const byte oxygenValveSwitch = 40;//50;
const byte methaneValveSwitch = 42;//48;
const byte turboPumpSwitch = 44;//46;
const byte landerLockSwitch = 46;//44;
const byte ignitionEnabledSwitch = 48;//42;
const byte masterArmSwitch = 50;//40;
const byte initiateLaunchSwitch = 52;//38;

//coprocessor board pin communication
const byte resetPanTiltBoard = 53;// resets the other board via a ground sink
const byte antennaLockPanTiltBoard = 51;// reads the true antenna lock output

bool switchReadTelemetry = false;
bool switchReadOxygen = false;
bool switchReadMethane = false;
bool switchReadTurboPump = false;
bool switchReadLanderLock = false;
bool switchReadIgnitionEnabled = false;
bool switchReadMasterArm = false;
bool switchReadLaunchButton = false;
bool antennaLock = false;

bool switchArray[10] = {switchReadTelemetry, switchReadOxygen, switchReadMethane, switchReadTurboPump, switchReadLanderLock, switchReadIgnitionEnabled, switchReadMasterArm, switchReadLaunchButton};




//----------------END SECTION FOR SWITCHES------------------------------------------

//pinMode(pin#, INPUT);

// select the display class and display driver class in the following file (new style):
#include "GxEPD2_display_selection_new_style.h"

void setup()
{


  //initialize switches
  pinMode(telemetrySwitch, INPUT_PULLUP);
  pinMode(oxygenValveSwitch, INPUT_PULLUP);
  pinMode(methaneValveSwitch, INPUT_PULLUP);
  pinMode(turboPumpSwitch, INPUT_PULLUP);
  pinMode(landerLockSwitch, INPUT_PULLUP);
  pinMode(ignitionEnabledSwitch, INPUT_PULLUP);
  pinMode(masterArmSwitch, INPUT_PULLUP);
  pinMode(initiateLaunchSwitch, INPUT_PULLUP);
  //  pinMode(switchInput09, INPUT_PULLUP);
  //  pinMode(switchInput10, INPUT_PULLUP);
  //  pinMode(switchInput11, INPUT_PULLUP);
  //  pinMode(switchInput12, INPUT_PULLUP);
  //  pinMode(switchInput13, INPUT_PULLUP);


  //-----------ANTENNA-------------------------
  //initialize output meters and joystick
  pinMode(joyUp, INPUT_PULLUP);
  pinMode(joyDown, INPUT_PULLUP);
  pinMode(joyLeft, INPUT_PULLUP);
  pinMode(joyRight, INPUT_PULLUP);
  pinMode(resetPanTiltBoard, INPUT_PULLUP);
  pinMode(antennaLockPanTiltBoard, INPUT_PULLUP);
  pinMode(azimuthGauge, OUTPUT);
  pinMode(elevationGauge, OUTPUT);

  //Attaching servos and setting initial position
  pan.attach(panServo);
  tilt.attach(tiltServo);

  pan.write(panInitial);
  tilt.write(tiltInitial);
  analogWrite(azimuthGauge, azimuthInitial);
  analogWrite(elevationGauge, elevationInitial);

  panPos = panInitial;
  tiltPos = tiltInitial;

  azimuthPos = azimuthInitial;
  elevationPos = elevationInitial;


  //------------END ANTENNA-------------------




  //-----------------INTERRUPTS------------------
  //  attachInterrupt(digitalPinToInterrupt(oxygenValveSwitch), oxygenValve_ISR, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(methaneValveSwitch), methaneValve_ISR, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(turboPumpSwitch), turboPump_ISR, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(landerLockSwitch), landerLock_ISR, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(ignitionEnabledSwitch), ignitionEnabled_ISR, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(masterArmSwitch), masterArm_ISR, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(telemetrySwitch), telemetry_ISR, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(initiateLaunchSwitch), initiateLaunch_ISR, RISING);
  //  attachInterrupt(digitalPinToInterrupt(joyUp), joyUp_ISR, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(joyDown), joyDown_ISR, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(joyLeft), joyLeft_ISR, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(joyRight), joyRight_ISR, FALLING);

  //---------------------------------------------------------
  Serial.begin(9600);
  display.init();
  helloURC();
  time_now = millis();

  delay(2000);
  statusScreen();

}


/*void checkSwitches()
  {


  }
  /**/



//keyboard
// This function intercepts key press

const int keyBuffer = 12;
char keyLogger[keyBuffer];

void keyPressed()
{
  if (keyPressCounter > keyBuffer) {
    keyPressCounter = 0;
  }

  if (keyboard.getOemKey() == 42) //If backspace is pressed, go back to previous keyLogger index and replace with blank value
  {
    keyPressCounter--;
    keyLogger[keyPressCounter] = ' ';
  }
  else
  {
    keyLogger[keyPressCounter] = keyboard.getKey();
    keyPressCounter++;
  }

  updateKeyboardScreen(); //send keys to screen when pressed

}

void updateSwitchScreen()
{

  if (digitalRead(telemetrySwitch) == LOW) {
    Serial.println("switch1");
    switchArray[0] = true;
  }
  if (digitalRead(oxygenValveSwitch) == LOW) {
    Serial.println("switch2");
    switchArray[1] = true;
  }
  if (digitalRead(methaneValveSwitch) == LOW) {
    Serial.println("switch3");
    switchArray[2] = true;
  }
  if (digitalRead(turboPumpSwitch) == LOW) {
    Serial.println("switch4");
    switchArray[3] = true;
  }
  if (digitalRead(landerLockSwitch) == LOW) {
    Serial.println("switch5");
    switchArray[4] = true;
  }
  if (digitalRead(ignitionEnabledSwitch) == LOW) {
    Serial.println("switch6");
    switchArray[5] = true;
  }
  if (digitalRead(masterArmSwitch) == LOW) {
    Serial.println("switch7");
    switchArray[6] = true;
  }
  if (digitalRead(initiateLaunchSwitch) == HIGH) {
    Serial.println("switch8");
    switchArray[7] = true;
  }
  //  if(digitalRead(switchInput09) == LOW){ Serial.println("switch9");switchArray[8] = true;}
  //  if(digitalRead(switchInput10) == LOW){ Serial.println("switch10");switchArray[9] = true;}

  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;


  uint16_t x_input = 24;
  uint16_t y_input = 8;

  //remember to use increment of 8
  display.setPartialWindow(x_input, y_input - 8, 24, 152);
  display.firstPage();
  do
  {
    if (digitalRead(telemetrySwitch) == LOW) {
      switchArray[0] = true;
      display.fillRect(x_input, y_input, 24, 8, GxEPD_BLACK);
    }
    if (digitalRead(oxygenValveSwitch) == LOW) {
      switchArray[1] = true;
      display.fillRect(x_input, y_input + 20, 24, 8, GxEPD_BLACK);
    }
    if (digitalRead(methaneValveSwitch) == LOW) {
      switchArray[2] = true;
      display.fillRect(x_input, y_input + 40, 24, 8, GxEPD_BLACK);
    }
    if (digitalRead(turboPumpSwitch) == LOW) {
      switchArray[3] = true;
      display.fillRect(x_input, y_input + 60, 24, 8, GxEPD_BLACK);
    }
    if (digitalRead(landerLockSwitch) == LOW) {
      switchArray[4] = true;
      display.fillRect(x_input, y_input + 80, 24, 8, GxEPD_BLACK);
    }
    if (digitalRead(ignitionEnabledSwitch) == LOW) {
      switchArray[5] = true;
      display.fillRect(x_input, y_input + 100, 24, 8, GxEPD_BLACK);
    }
    if (digitalRead(masterArmSwitch) == LOW) {
      switchArray[6] = true;
      display.fillRect(x_input, y_input + 100, 24, 8, GxEPD_BLACK);
    }
    if (digitalRead(initiateLaunchSwitch) == HIGH) {
      switchArray[7] = true;
      display.fillRect(x_input, y_input + 120, 24, 8, GxEPD_BLACK);
    }
    //      display.fillRect(x_input, y_input, 24, 8, GxEPD_BLACK);
    //      display.fillRect(x_input, y_input+20, 24, 8, GxEPD_BLACK);
    //      display.fillRect(x_input, y_input+40, 24, 8, GxEPD_BLACK);
    //      display.fillRect(x_input, y_input+60, 24, 8, GxEPD_BLACK);
    //      display.fillRect(x_input, y_input+80, 24, 8, GxEPD_BLACK);
    //      display.fillRect(x_input, y_input+100, 24, 8, GxEPD_BLACK);
    //     display.fillRect(x_input, y_input+120, 24, 8, GxEPD_BLACK);



  }
  while (display.nextPage());
}


void updateKeyboardScreen()
{
  display.setRotation(0);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;


  uint16_t x_input = 24;
  uint16_t y_input = 240;

  //display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center the bounding box by transposition of the origin:
  //uint16_t x = ((display.width() - tbw) / 2) - tbx;
  //uint16_t y = ((display.height() - tbh) / 2) - tby;
  //top left of screen
  //display.setFullWindow();

  //remember to use increment of 8
  display.setPartialWindow(x_input - 24, y_input - 24, 376, 48);
  display.firstPage();
  do
  {
    display.setCursor(x_input - 16, y_input);
    display.print(">>");
    for (int i = 0; i < keyBuffer; i++) {
      display.print(keyLogger[i]);
    }

  }
  while (display.nextPage());




}



void helloURC()
{
  display.setRotation(0);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  //  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center the bounding box by transposition of the origin:
  //  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  //  uint16_t y = ((display.height() - tbh) / 2) - tby;
  //top left of screen
  uint16_t x = 100;
  uint16_t y = 100;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print("2022 URC");

    display.setCursor(x, y + 30);
    display.print("EQUIPMENT");

    display.setCursor(x, y + 60);
    display.print("SERVICING");
  }
  while (display.nextPage());


}


void statusScreen()
{

  //read switches
  if (digitalRead(telemetrySwitch) == LOW) {
    switchArray[0] = true;
  }
  if (digitalRead(oxygenValveSwitch) == LOW) {
    switchArray[1] = true;
  }
  if (digitalRead(methaneValveSwitch) == LOW) {
    switchArray[2] = true;
  }
  if (digitalRead(turboPumpSwitch) == LOW) {
    switchArray[3] = true;
  }
  if (digitalRead(landerLockSwitch) == LOW) {
    switchArray[4] = true;
  }
  if (digitalRead(ignitionEnabledSwitch) == LOW) {
    switchArray[5] = true;
  }
  if (digitalRead(masterArmSwitch) == LOW) {
    switchArray[6] = true;
  }
  if (digitalRead(initiateLaunchSwitch) == HIGH) {
    switchArray[7] = true;
  }






  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  //display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center the bounding box by transposition of the origin:
  //uint16_t x = ((display.width() - tbw) / 2) - tbx;
  //uint16_t y = ((display.height() - tbh) / 2) - tby;
  //top left of screen
  uint16_t x = 8;
  uint16_t y = 16;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print("[ ");
    if (switchArray[0] == true)
    {
      display.print("X ] ");
    }
    else {
      display.print("  ] ");
    }
    display.print("ENABLE TELEMETRY");

    display.setCursor(x, y + 20);
    display.print("[ ");
    if (switchArray[1] == true)
    {
      display.print("X ] ");
    }
    else {
      display.print("  ] ");
    }
    display.print("OXYGEN ON");

    display.setCursor(x, y + 40);
    display.print("[ ");
    if (switchArray[2] == true)
    {
      display.print("X ] ");
    }
    else {
      display.print("  ] ");
    }
    display.print("METHANE ON");

    display.setCursor(x, y + 60);
    display.print("[ ");
    if (switchArray[3] == true)
    {
      display.print("X ] ");
    }
    else {
      display.print("  ] ");
    }
    display.print("TURBOPUMP ON");

    display.setCursor(x, y + 80);
    display.print("[ ");
    if (switchArray[4] == true)
    {
      display.print("X ] ");
    }
    else {
      display.print("  ] ");
    }
    display.print("LANDER LOCK RELEASED");

    display.setCursor(x, y + 100);
    display.print("[ ");
    if (switchArray[5] == true)
    {
      display.print("X ] ");
    }
    else {
      display.print("  ] ");
    }
    display.print("IGNITION ENABLED");

    display.setCursor(x, y + 120);
    display.print("[ ");
    if (switchArray[6] == true)
    {
      display.print("X ] ");
    }
    else {
      display.print("  ] ");
    }
    display.print("MASTER ARM ON");


    display.setFont(&FreeMonoBold18pt7b);
    display.setCursor(8, 240);
    display.print(">>");
  }


  while (display.nextPage());

}



void panTilt()
{

  //right-pan-azimuth
  if (digitalRead(joyRight) == LOW && azimuthPos <= servoMax) { // && antennaAligned == false){
    if ( millis() > time_now + periodServo)
    {
      time_now = millis();
      //Move elevation gauge
      azimuthPos = azimuthPos + jogSpeed;
//     Serial.print("Az Gauge Pos: ");
//     Serial.print(azimuthPos);
      analogWrite(azimuthGauge, azimuthPos);

      //Move pan servo
      panPos = panPos + jogSpeed;
      pan.write(panPos);
      //delay(500);
//      Serial.print("   Servo right: ");
//      Serial.println(panPos);

      //If both gauges are lined up, then send signal to the Pi
      if (azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin) {
//        Serial.println("postition locked");
        //antennaAligned = true;
      }
    }
  }


  //left-pan-azimuth
  if (digitalRead(joyLeft) == LOW && azimuthPos >= servoMin) { //&& antennaAligned == false){
    if ( millis() > time_now + periodServo)
    {
      time_now = millis();
      //Move elevation gauge
      azimuthPos = azimuthPos - jogSpeed;
      //      Serial.print("Az Gauge Pos: ");
      //      Serial.print(azimuthPos);
      analogWrite(azimuthGauge, azimuthPos);

      //Move pan servo
      panPos = panPos - jogSpeed;
      pan.write(panPos);
      //delay(500);
      //      Serial.print("   Servo left: ");
      //      Serial.println(panPos);


      //If both gauges are lined up, then send signal to the Pi
      if (azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin) {
        Serial.println("postition locked");
        //antennaAligned = true;
      }
    }
  }

  //up-tilt-elevation
  if (digitalRead(joyUp) == LOW && elevationPos <= servoMax) { // && antennaAligned == false){
    if ( millis() > time_now + periodServo)
    {
      time_now = millis();
      //Move elevation gauge
      elevationPos = elevationPos + jogSpeed;
      //      Serial.print("Ev Gauge Pos: ");
      //      Serial.print(elevationPos);
      analogWrite(elevationGauge, elevationPos);

      //Move pan servo
      tiltPos = tiltPos + jogSpeed;
      tilt.write(tiltPos);
      //delay(500);
      //      Serial.print("  Servo Up: ");
      //      Serial.println(tiltPos);


      //If both gauges are lined up, then send signal to the Pi
      if (azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin) {
        Serial.println("postition locked");
        //antennaAligned = true;
      }
    }
  }

  //down-tilt-elevation
  if (digitalRead(joyDown) == LOW && elevationPos >= servoMin) { // && antennaAligned == false){
    if ( millis() > time_now + periodServo)
    {
      time_now = millis();
      //Move elevation gauge
      elevationPos = elevationPos - jogSpeed;
      Serial.print("Ev Gague Pos: ");
      Serial.print(elevationPos);
      analogWrite(elevationGauge, elevationPos);

      //Move pan servo
      tiltPos = tiltPos - jogSpeed;
      tilt.write(tiltPos);
      //delay(500);

      Serial.print("   Servo Down: ");
      Serial.println(tiltPos);

      //If both gauges are lined up, then set a flag to true
      if (azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin) {
        Serial.println("postition locked");
        // antennaAligned = true;
      }
    }

  }

}

void loop() {

  //attach USB task
  usb.Task();
  panTilt();

  //updateSwitchScreen();


  //
  // //right-pan-azimuth
  //  if(digitalRead(right) == LOW && azimuthPos <= servoMax && antennaAligned == false){
  //    if( millis() > time_now + periodServo)
  //    {
  //      time_now = millis();
  //      //Move elevation gauge
  //      azimuthPos = azimuthPos + jogSpeed;
  //      Serial.print("Az Gauge Pos: ");
  //      Serial.print(azimuthPos);
  //      analogWrite(azimuthGauge,azimuthPos);
  //
  //      //Move pan servo
  //      panPos = panPos + jogSpeed;
  //      pan.write(panPos);
  //      //delay(500);
  //      Serial.print("   Servo right: ");
  //      Serial.println(panPos);
  //
  //      //If both gauges are lined up, then send signal to the Pi
  //      if(azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin){
  //       Serial.println("postition locked");
  //       antennaAligned = true;
  //      }
  //    }
  //   }
  //
  //
  // //left-pan-azimuth
  //  if(digitalRead(left) == LOW && azimuthPos >= servoMin && antennaAligned == false){
  //    if( millis() > time_now + periodServo)
  //    {
  //      time_now = millis();
  //      //Move elevation gauge
  //      azimuthPos = azimuthPos - jogSpeed;
  ////      Serial.print("Az Gauge Pos: ");
  ////      Serial.print(azimuthPos);
  //      analogWrite(azimuthGauge,azimuthPos);
  //
  //      //Move pan servo
  //      panPos = panPos - jogSpeed;
  //      pan.write(panPos);
  //      //delay(500);
  ////      Serial.print("   Servo left: ");
  ////      Serial.println(panPos);
  //
  //
  //      //If both gauges are lined up, then send signal to the Pi
  //      if(azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin){
  //       Serial.println("postition locked");
  //       antennaAligned = true;
  //      }
  //    }
  //   }
  //
  ////up-tilt-elevation
  //  if(digitalRead(up) == LOW && elevationPos <= servoMax && antennaAligned == false){
  //    if( millis() > time_now + periodServo)
  //    {
  //      time_now = millis();
  //      //Move elevation gauge
  //      elevationPos = elevationPos + jogSpeed;
  ////      Serial.print("Ev Gauge Pos: ");
  ////      Serial.print(elevationPos);
  //      analogWrite(elevationGauge,elevationPos);
  //
  //      //Move pan servo
  //      tiltPos = tiltPos + jogSpeed;
  //      tilt.write(tiltPos);
  //      //delay(500);
  ////      Serial.print("  Servo Up: ");
  ////      Serial.println(tiltPos);
  //
  //
  //      //If both gauges are lined up, then send signal to the Pi
  //      if(azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin){
  //       Serial.println("postition locked");
  //       antennaAligned = true;
  //      }
  //    }
  //   }
  //
  ////down-tilt-elevation
  //  if(digitalRead(down) == LOW && elevationPos >= servoMin && antennaAligned == false){
  //    if( millis() > time_now + periodServo)
  //    {
  //      time_now = millis();
  //      //Move elevation gauge
  //      elevationPos = elevationPos - jogSpeed;
  //      Serial.print("Ev Gague Pos: ");
  //      Serial.print(elevationPos);
  //      analogWrite(elevationGauge,elevationPos);
  //
  //      //Move pan servo
  //      tiltPos = tiltPos - jogSpeed;
  //      tilt.write(tiltPos);
  //      //delay(500);
  //
  //      Serial.print("   Servo Down: ");
  //      Serial.println(tiltPos);
  //
  //      //If both gauges are lined up, then set a flag to true
  //      if(azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin){
  //       Serial.println("postition locked");
  //       antennaAligned = true;
  //      }
  //    }
  //
  //   }


  //updateSwitchScreen();

}


//--------------------INTERRUPT FUNCTION-----------------------

void taskUpdate(int switchNum) {
  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;


  uint16_t x_input = 24;
  uint16_t y_input = 8;

  //remember to use increment of 8
  display.setPartialWindow(x_input, y_input - 8, 24, 152);
  display.firstPage();
  do
  {
    display.fillRect(x_input, y_input + 20 * switchNum, 24, 8, GxEPD_BLACK);
  }
  while (display.nextPage());


}

void oxygenValve_ISR() {
  oxygenValve_time = millis();
  if (oxygenValve_time - last_oxygenValve_time > 250) {
    taskUpdate(1);
    //    updateSwitchScreen();
    display.setRotation(0);
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    int16_t tbx, tby; uint16_t tbw, tbh;


    uint16_t x_input = 24;
    uint16_t y_input = 8;

    //remember to use increment of 8
    display.setPartialWindow(x_input, y_input - 8, 24, 152);
    display.firstPage();
    do
    {
      display.fillRect(x_input, y_input, 24, 8, GxEPD_BLACK);
    }
    while (display.nextPage());
  }

  last_oxygenValve_time = oxygenValve_time;
}

void methaneValve_ISR() {
  methaneValve_time = millis();
  if (methaneValve_time - last_methaneValve_time > 250) {
    Serial.print("Interrupt2 ");
    Serial.print(y++);
    Serial.println();
    last_methaneValve_time = methaneValve_time;
  }
}


void turboPump_ISR() {
  turboPump_time = millis();
  if (turboPump_time - last_turboPump_time > 250) {
    Serial.print("Interrupt3 ");
    Serial.print(y++);
    Serial.println();
    last_turboPump_time = turboPump_time;
  }
}


void landerLock_ISR() {
  Serial.print("In lander lock");
  landerLock_time = millis();
  // if (landerLock_time - last_landerLock_time > 250){
  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;


  uint16_t x_input = 24;
  uint16_t y_input = 8;

  //remember to use increment of 8
  display.setPartialWindow(x_input, y_input - 8, 24, 152);
  display.firstPage();
  do
  {
    display.fillRect(x_input, y_input, 24, 8, GxEPD_BLACK);
  }
  while (display.nextPage());
  //    last_landerLock_time = landerLock_time;
  //    }
}

void ignitionEnabled_ISR() {
  ignitionEnabled_time = millis();
  if (ignitionEnabled_time - last_ignitionEnabled_time > 250) {
    Serial.print("Interrupt5 ");
    Serial.print(y++);
    Serial.println();
    last_ignitionEnabled_time = ignitionEnabled_time;
  }
}

void masterArm_ISR() {
  masterArm_time = millis();
  if (masterArm_time - last_masterArm_time > 250) {
    Serial.print("Interrupt6 ");
    Serial.print(y++);
    Serial.println();
    last_masterArm_time = masterArm_time;
  }
}

void telemetry_ISR() {
  telemetry_time = millis();
  if (telemetry_time - last_telemetry_time > 250) {
    Serial.print("Interrupt7 ");
    Serial.print(y++);
    Serial.println();
    last_telemetry_time = telemetry_time;
  }
}

void initiateLaunch_ISR() {
  initiateLaunch_time = millis();
  if (initiateLaunch_time - last_initiateLaunch_time > 250) {
    Serial.print("Interrupt8 ");
    Serial.print(y++);
    Serial.println();
    last_initiateLaunch_time = initiateLaunch_time;
  }
}

//joyUp-tilt-elevation
void joyUp_ISR() {
  Serial.print("UP!!");
  // panTilt();
  joyUp_time = millis();
  if (joyUp_time - last_joyUp_time > 500) {

    //Move elevation gauge
    elevationPos = elevationPos + jogSpeed;
    analogWrite(elevationGauge, elevationPos);

    tiltPos = tiltPos + jogSpeed;
    if (servoMin < tiltPos < servoMax) {
      //Move tilt servo
      Serial.print("Moving");
      tilt.write(tiltPos);
    }
    last_joyUp_time = joyUp_time;
    if (azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin) {
      //antennaAligned();
    }
  }
}

//joyDown-tilt-elevation
void joyDown_ISR() {
  joyDown_time = millis();
  if (joyDown_time - last_joyDown_time > 500) {

    //Move elevation gauge
    elevationPos = elevationPos - jogSpeed;
    analogWrite(elevationGauge, elevationPos);

    tiltPos = tiltPos - jogSpeed;
    if (servoMin < tiltPos < servoMax) {
      //Move tilt servo
      tilt.write(tiltPos);
    }
    last_joyDown_time = joyDown_time;

    if (azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin) {
      //antennaAligned();
    }
  }
}

//joyLeft-pan-azimuth
void joyLeft_ISR() {
  joyLeft_time = millis();
  if (joyLeft_time - last_joyLeft_time > 500) {

    //Move azimuth gauge
    azimuthPos = azimuthPos - jogSpeed;
    analogWrite(azimuthGauge, azimuthPos);

    panPos = panPos - jogSpeed;
    if (servoMin < tiltPos < servoMax) {
      //Move pan servo
      pan.write(panPos);
    }
    last_joyLeft_time = joyLeft_time;

    if (azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin) {
      //antennaAligned();
    }
  }
}

//joyRight-pan-azimuth
void joyRight_ISR() {
  joyRight_time = millis();
  if (joyRight_time - last_joyRight_time > 500) {

    //Move azimuth gauge
    azimuthPos = azimuthPos + jogSpeed;
    analogWrite(azimuthGauge, azimuthPos);

    panPos = panPos + jogSpeed;
    if (servoMin < tiltPos < servoMax) {
      //Move pan servo
      pan.write(panPos);
    }
    last_joyRight_time = joyRight_time;

    if (azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin) {
      //antennaAligned();
    }
  }
}

// not using this code wihle the coprocessor exists
//void antennaAligned()
//{
//    //TIMMMMM WRITE ME SOME CODE!!!
//    Serial.println("postition locked");
//}
