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
unsigned long time_now = 0;

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

bool antennaAligned = false;

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
const byte up = 23;
const byte down = 25;
const byte left = 22;
const byte right = 24;

//pin assignment for the servos
const byte panServo = 2; //PWM pin
const byte tiltServo = 3; //PWM pin

//pin assignments for the azimuth and elevation meters
const byte azimuthGauge = 4; //PWM pin
const byte elevationGauge = 5; //PWM pin

void setup() {

  // Initialize Switches and Joystick
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(left, INPUT_PULLUP);
  pinMode(right, INPUT_PULLUP);

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
  //helloURC();
  
  //delay(2000);
  statusScreen(); 
  time_now = millis();

  
}

void loop(){
  
  //attach USB task
  usb.Task();

//right-pan-azimuth
  if(digitalRead(right) == LOW && azimuthPos <= servoMax && antennaAligned == false){
    if( millis() > time_now + periodServo)
    {
      time_now = millis();    
      //Move elevation gauge
      azimuthPos = azimuthPos + jogSpeed;
      Serial.print("Az Gauge Pos: ");
      Serial.print(azimuthPos);
      analogWrite(azimuthGauge,azimuthPos);
  
      //Move pan servo
      panPos = panPos + jogSpeed;
      pan.write(panPos);
      //delay(500);
      Serial.print("   Servo right: ");
      Serial.println(panPos);
  
      //If both gauges are lined up, then send signal to the Pi
      if(azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin){
       Serial.println("postition locked");
       antennaAligned = true; 
      }
    }
   }

   
 //left-pan-azimuth
  if(digitalRead(left) == LOW && azimuthPos >= servoMin && antennaAligned == false){
    if( millis() > time_now + periodServo)
    {
      time_now = millis();    
      //Move elevation gauge
      azimuthPos = azimuthPos - jogSpeed;
//      Serial.print("Az Gauge Pos: ");
//      Serial.print(azimuthPos);
      analogWrite(azimuthGauge,azimuthPos);
  
      //Move pan servo
      panPos = panPos - jogSpeed;
      pan.write(panPos);
      //delay(500);
//      Serial.print("   Servo left: ");
//      Serial.println(panPos);
  
  
      //If both gauges are lined up, then send signal to the Pi
      if(azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin){
       Serial.println("postition locked");
       antennaAligned = true; 
      }
    }
   }

//up-tilt-elevation
  if(digitalRead(up) == LOW && elevationPos <= servoMax && antennaAligned == false){
    if( millis() > time_now + periodServo)
    {
      time_now = millis();      
      //Move elevation gauge
      elevationPos = elevationPos + jogSpeed;
//      Serial.print("Ev Gauge Pos: ");
//      Serial.print(elevationPos);
      analogWrite(elevationGauge,elevationPos);
  
      //Move pan servo
      tiltPos = tiltPos + jogSpeed;
      tilt.write(tiltPos);
      //delay(500);
//      Serial.print("  Servo Up: ");
//      Serial.println(tiltPos);
  
      
      //If both gauges are lined up, then send signal to the Pi
      if(azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin){
       Serial.println("postition locked");
       antennaAligned = true; 
      }
    }
   }

//down-tilt-elevation
  if(digitalRead(down) == LOW && elevationPos >= servoMin && antennaAligned == false){
    if( millis() > time_now + periodServo)
    {
      time_now = millis();
      //Move elevation gauge
      elevationPos = elevationPos - jogSpeed;
      Serial.print("Ev Gague Pos: ");
      Serial.print(elevationPos);
      analogWrite(elevationGauge,elevationPos);
  
      //Move pan servo
      tiltPos = tiltPos - jogSpeed;
      tilt.write(tiltPos);
      //delay(500);
      
      Serial.print("   Servo Down: ");
      Serial.println(tiltPos);
  
      //If both gauges are lined up, then set a flag to true
      if(azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin){
       Serial.println("postition locked");
       antennaAligned = true;
      }
    }
    
   }



  
  //updateKeyboardScreen();
}


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

  
  updateKeyboardScreen();
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
    display.print("  ] ");
    display.print("ENABLE TELEMETRY");

    display.setCursor(x, y+20);
    display.print("[ ");
    display.print("  ] ");
    display.print("OXYGEN ON");
    
    display.setCursor(x, y+40);
    display.print("[ ");
    display.print("  ] ");
    display.print("METHANE ON");
    
    display.setCursor(x, y+60);
    display.print("[ ");
    display.print("  ] ");
    display.print("TURBOPUMP ON");
    
    display.setCursor(x, y+80);
    display.print("[ ");
    display.print("  ] ");
    display.print("LANDER LOCK RELEASED");
    
    display.setCursor(x, y+100);
    display.print("[ ");
    display.print("  ] ");
    display.print("IGNITION ENABLED");
    
    display.setCursor(x, y+120);
    display.print("[ ");
    display.print("  ] ");
    display.print("MASTER ARM ON");


    display.setFont(&FreeMonoBold18pt7b);
    display.setCursor(24, 240);
    display.print(">>");
  }  

  
  while (display.nextPage());

}
