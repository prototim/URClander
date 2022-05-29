//-----------SECTION FOR INCLUDED LIBRARIES-------------
// For EPD
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>

// Require keyboard control library
#include <KeyboardController.h>

#include <Servo.h>

// select the display class and display driver class in the following file (new style):
#include "GxEPD2_display_selection_new_style.h"


//----------SECTION KEYBOARD SETUP--------------
// Initialize USB Controller
USBHost usb;

// Attach keyboard controller to USB
KeyboardController keyboard(usb);
byte keyPressCounter = 0;
char keyLogger[16];

//----------------SECTION FOR JOYSTICK AND GAUGE OUTPUT 3V------------------------------------------
Servo pan, tilt;

//timing periods
unsigned long Servotime_now = 0;
const int periodServo = 500; //ms
const int periodScreen = 2000;
const int fiftyms = 50;

//initialize the servo variables
//tracking variables
byte azimuthPos = 0;
byte elevationPos =0;
byte panPos = 0;
byte tiltPos = 0;

//Initialize
byte jogSpeed = 5; //what the incremental movement is
byte azimuthInitial = 100;
byte elevationInitial = 10;
byte panInitial = 100;
byte tiltInitial = 10;

//Limits and desired
byte gaugeMin = 0;
byte gaugeMax = 190;
byte servoMax = 160;
byte servoMin = 10;
byte desiredAzimuthMax = 20;
byte desiredElevationMax = 130;
byte desiredAzimuthMin = 10;
byte desiredElevationMin = 120;


//-------SECTION PIN ASSIGNMENTS----------
//Switch inputs
const byte oxygenValveSwitch = 52;
const byte methaneValveSwitch = 50;
const byte turboPumpSwitch = 48;
const byte landerLockSwitch = 46;
const byte ignitionEnabledSwitch = 44;
const byte masterArmSwitch = 42;
const byte telemetrySwitch = 40;
const byte initiateLaunchSwitch = 38;

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
bool antennaAligned = false;

bool oxygenValveRead = false;
bool methaneValveRead = false;
bool turboPumpRead = false;
bool landerLockRead = false;
bool ignitionEnabledRead = false;
bool masterArmRead = false;
bool telemetryRead = false;
bool initiateLaunchRead = false;



const byte switchInputArray[13] = {oxygenValveSwitch, methaneValveSwitch, turboPumpSwitch, landerLockSwitch, ignitionEnabledSwitch, masterArmSwitch, telemetrySwitch, initiateLaunchSwitch, joyUp, joyDown, joyLeft, joyRight};
bool switchArrayRead[13]={oxygenValveRead, methaneValveRead, turboPumpRead, landerLockRead, ignitionEnabledRead, masterArmRead, telemetryRead, initiateLaunchRead};

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



//------SECTION FOR TESTING PURPOSES ONLY-------
int y = 0;
int x = 0;

void setup() {

  // Initialize Switches and Joystick
  for (byte i = 0; i < 13; i++){
    pinMode(switchInputArray[i], INPUT_PULLUP);
  }

  //Initialize Gauges PWM 
  pinMode(azimuthGauge, OUTPUT);
  pinMode(elevationGauge, OUTPUT);

  //Attaching servos and setting initial position
  pan.attach(panServo);
  tilt.attach(tiltServo);
  
  pan.write(panInitial);
  tilt.write(tiltInitial);     
  
  analogWrite(azimuthGauge,azimuthInitial);     
  analogWrite(elevationGauge,elevationInitial);
  
  panPos = panInitial;
  tiltPos = tiltInitial;
  
  azimuthPos = azimuthInitial;
  elevationPos = elevationInitial;
  
  Serial.begin(9600);

  //Initial Display
  display.init();
  helloURC();
  Servotime_now = millis();
  
  delay(2000);
  statusScreen(); 

  
  //Creation of interrupts for switches and joyswitch
  attachInterrupt(digitalPinToInterrupt(oxygenValveSwitch), oxygenValve_ISR, LOW);
  attachInterrupt(digitalPinToInterrupt(methaneValveSwitch), methaneValve_ISR, LOW);
  attachInterrupt(digitalPinToInterrupt(turboPumpSwitch), turboPump_ISR, LOW);
  attachInterrupt(digitalPinToInterrupt(landerLockSwitch), landerLock_ISR, LOW);
  attachInterrupt(digitalPinToInterrupt(ignitionEnabledSwitch), ignitionEnabled_ISR, LOW);
  attachInterrupt(digitalPinToInterrupt(masterArmSwitch), masterArm_ISR, LOW);
  attachInterrupt(digitalPinToInterrupt(telemetrySwitch), telemetry_ISR, LOW);
  attachInterrupt(digitalPinToInterrupt(initiateLaunchSwitch), initiateLaunch_ISR, LOW);
//attachInterrupt(digitalPinToInterrupt(joyUp), joyUp_ISR, LOW);
//attachInterrupt(digitalPinToInterrupt(joyDown), joyDown_ISR, LOW);
//attachInterrupt(digitalPinToInterrupt(joyLeft), joyLeft_ISR, LOW);
//attachInterrupt(digitalPinToInterrupt(joyRight), joyRight_ISR, LOW);

     
  
}
//--------------------VOID LOOP-----------------------
void loop(){
  //attach USB task
  usb.Task();
  updateKeyboardScreen();
}


//--------------------KEYBOARD FUNCTION-----------------------
void keyPressed()
{
  if (keyPressCounter > 16){ keyPressCounter = 0;}
  
//  if (keyboard.getOemKey() == 42) {keyPressCounter--;}
//  else { keyPressCounter++;}
  
  if (keyboard.getOemKey() == 42)
  {
    keyPressCounter--;
    keyLogger[keyPressCounter] = ' ';
  }
  else
  {
    keyLogger[keyPressCounter] = keyboard.getKey();
    keyPressCounter++;
  }

}


//--------------------EPD-----------------------
void updateKeyboardScreen()
{  
  display.setRotation(0);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;


  uint16_t x_input = 24;
  uint16_t y_input = 240;

    //remember to use increment of 8
    display.setPartialWindow(x_input-24,y_input-24,376,48);
    display.firstPage();
    do
    {
      display.setCursor(x_input, y_input);
      display.print(">>");
      display.print(keyLogger[0]);
      display.print(keyLogger[1]);
      display.print(keyLogger[2]);
      display.print(keyLogger[3]);
      display.print(keyLogger[4]);
      display.print(keyLogger[5]);
      display.print(keyLogger[6]);
      display.print(keyLogger[7]);
      display.print(keyLogger[8]);
      display.print(keyLogger[9]);
      display.print(keyLogger[10]);
      display.print(keyLogger[11]);
      display.print(keyLogger[12]);
      display.print(keyLogger[13]);
      display.print(keyLogger[14]);
      display.print(keyLogger[15]);
    }
    while (display.nextPage());
}


void helloURC()
{
  display.setRotation(0);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;

  uint16_t x =100;
  uint16_t y =100;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print("2022 URC");
    
    display.setCursor(x, y+30);
    display.print("EQUIPMENT");
    
    display.setCursor(x, y+60);
    display.print("SERVICING");
  }
  while (display.nextPage());



}

void statusScreen()
{

  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;

  uint16_t x =10;
  uint16_t y =15;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print("[ ");
    if (switchArray[0] == true)
      {
        display.print(" X ] ");
      }
    else{ display.print("  ] ");}
    display.print("ENABLE TELEMETRY");

    display.setCursor(x, y+20);
    display.print("[ ");
    if (switchArray[1] == true)
      {
        display.print("X ] ");
      }
    else{ display.print("  ] ");}
    display.print("OXYGEN ON");
    
    display.setCursor(x, y+40);
    display.print("[ ");
    if (switchArray[2] == true)
      {
        display.print("X ] ");
      }
    else{ display.print("  ] ");}
    display.print("METHANE ON");
    
    display.setCursor(x, y+60);
    display.print("[ ");
    if (switchArray[3] == true)
      {
        display.print("X ] ");
      }
    else{ display.print("  ] ");}
    display.print("TURBOPUMP ON");
    
    display.setCursor(x, y+80);
    display.print("[ ");
    if (switchArray[4] == true)
      {
        display.print("X ] ");
      }
    else{ display.print("  ] ");}
    display.print("LANDER LOCK RELEASED");
    
    display.setCursor(x, y+100);
    display.print("[ ");
    if (switchArray[5] == true)
    {
      display.print("X ] ");
    }
    else{ display.print("  ] ");}
    display.print("IGNITION ENABLED");
    
    display.setCursor(x, y+120);
    display.print("[ ");
    if (switchArray[6] == true)
    {
      display.print("X ] ");
    }
    else{ display.print("  ] ");}
    display.print("MASTER ARM ON");


    display.setFont(&FreeMonoBold18pt7b);
    display.setCursor(24, 240);
    display.print(">>");
  }  

  
  while (display.nextPage());

}

//--------------------INTERRUPT FUNCTION-----------------------

void taskUpdate(int switchNum){
  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;


  uint16_t x_input = 24;
  uint16_t y_input = 8;

    //remember to use increment of 8
    display.setPartialWindow(x_input,y_input-8,24,152);
    display.firstPage();
    do
    {
      display.fillRect(x_input, y_input+20*switchNum, 24, 8, GxEPD_BLACK);
    }
    while (display.nextPage());
  
  
  }

void oxygenValve_ISR(){
  oxygenValve_time = millis();
  if (oxygenValve_time - last_oxygenValve_time > 250){
     taskUpdate(1);
    last_oxygenValve_time = oxygenValve_time;
    }
  }

  void methaneValve_ISR(){
  methaneValve_time = millis();
  if (methaneValve_time - last_methaneValve_time > 250){
    Serial.print("Interrupt2 ");
    Serial.print(y++);
    Serial.println();
    last_methaneValve_time = methaneValve_time;
    }
  }


  void turboPump_ISR(){
  turboPump_time = millis();
  if (turboPump_time - last_turboPump_time > 250){
    Serial.print("Interrupt3 ");
    Serial.print(y++);
    Serial.println();
    last_turboPump_time = turboPump_time;
    }
  }


   void landerLock_ISR(){
  landerLock_time = millis();
  if (landerLock_time - last_landerLock_time > 250){
    Serial.print("Interrupt4 ");
    Serial.print(y++);
    Serial.println();
    last_landerLock_time = landerLock_time;
    }
  }

   void ignitionEnabled_ISR(){
  ignitionEnabled_time = millis();
  if (ignitionEnabled_time - last_ignitionEnabled_time > 250){
    Serial.print("Interrupt5 ");
    Serial.print(y++);
    Serial.println();
    last_ignitionEnabled_time = ignitionEnabled_time;
    }
  }

   void masterArm_ISR(){
  masterArm_time = millis();
  if (masterArm_time - last_masterArm_time > 250){
    Serial.print("Interrupt6 ");
    Serial.print(y++);
    Serial.println();
    last_masterArm_time = masterArm_time;
    }
  }

   void telemetry_ISR(){
  telemetry_time = millis();
  if (telemetry_time - last_telemetry_time > 250){
    Serial.print("Interrupt7 ");
    Serial.print(y++);
    Serial.println();
    last_telemetry_time = telemetry_time;
    }
  }

   void initiateLaunch_ISR(){
  initiateLaunch_time = millis();
  if (initiateLaunch_time - last_initiateLaunch_time > 250){
    Serial.print("Interrupt8 ");
    Serial.print(y++);
    Serial.println();
    last_initiateLaunch_time = initiateLaunch_time;
    }
  }

//joyUp-tilt-elevation
  void joyUp_ISR(){
  joyUp_time = millis();
  if (joyUp_time - last_joyUp_time > 500){
        
      //Move elevation gauge
      elevationPos = elevationPos + jogSpeed;
      analogWrite(elevationGauge,elevationPos);
      
      tiltPos = tiltPos + jogSpeed;
      if(servoMin< tiltPos <servoMax){
        //Move tilt servo  
        tilt.write(tiltPos);
      }
    last_joyUp_time = _time;
      if(azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin){
    antennaAligned();
      }
    }
  }

//joyDown-tilt-elevation
  void joyDown_ISR(){
  joyDown_time = millis();
  if (joyDown_time - last_joyDown_time > 500){
        
      //Move elevation gauge
      elevationPos = elevationPos - jogSpeed;
      analogWrite(elevationGauge,elevationPos);
      
      tiltPos = tiltPos - jogSpeed;
      if(servoMin< tiltPos <servoMax){
        //Move tilt servo  
        tilt.write(tiltPos);
      }
    last_joyDown_time = joyDown_time;
    
      if(azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin){
    antennaAligned();
      }
    }
  }

//joyLeft-pan-azimuth
  void joyLeft_ISR(){
  joyLeft_time = millis();
  if (joyLeft_time - last_joyLeft_time > 500){
        
      //Move azimuth gauge
      azimuthPos = azimuthPos - jogSpeed;
      analogWrite(azimuthGauge,azimuthPos);
      
      panPos = panPos - jogSpeed;
      if(servoMin< tiltPos <servoMax){
        //Move pan servo  
        pan.write(panPos);
      }
    last_joyLeft_time = joyLeft_time;
    
      if(azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin){
    antennaAligned();
      }
    }
  }

//joyRight-pan-azimuth
  void joyRight_ISR(){
  joyRight_time = millis();
  if (joyRight_time - last_joyRight_time > 500){
        
      //Move azimuth gauge
      azimuthPos = azimuthPos + jogSpeed;
      analogWrite(azimuthGauge,azimuthPos);
      
      panPos = panPos + jogSpeed;
      if(servoMin< tiltPos <servoMax){
        //Move pan servo  
        pan.write(panPos);
      }
    last_joyRight_time = joyRight_time;
    
      if(azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin){
    antennaAligned();
      }
    }
  }


  void antennaAligned(){
    //TIMMMMM WRITE ME SOME CODE!!!
    Serial.println("postition locked");
    }
