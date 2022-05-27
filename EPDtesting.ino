// GxEPD2_HelloWorld.ino by Jean-Marc Zingg

// see GxEPD2_wiring_examples.h for wiring suggestions and examples
// if you use a different wiring, you need to adapt the constructor parameters!

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

//----------------SECTION FOR JOYSTICK AND GAUGE OUTPUT 3V------------------------------------------

#include <Servo.h>

Servo pan, tilt;

//timing periods
unsigned long time_now = 0;
const int periodServo = 500; //ms

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

//antennaAligned
bool antennaAligned = false;


//----------------END SECTION FOR JOYSTICK AND GAUGE OUTPUT 3V------------------------------------------





//----------------SECTION FOR SWITCHES------------------------------------------


//Switch inputs

const byte switchInput01 = 52;
const byte switchInput02 = 50;
const byte switchInput03 = 48;
const byte switchInput04 = 46;
const byte switchInput05 = 44;
const byte switchInput06 = 42;
const byte switchInput07 = 40;
const byte switchInput08 = 38;
const byte switchInput09 = 36;
const byte switchInput10 = 34;
const byte switchInput11 = 32;
const byte switchInput12 = 30;
const byte switchInput13 = 28;

bool switchRead01 = false;
bool switchRead02 = false;
bool switchRead03 = false;
bool switchRead04 = false;
bool switchRead05 = false;
bool switchRead06 = false;
bool switchRead07 = false;
bool switchRead08 = false;
bool switchRead09 = false;
bool switchRead10 = false;

bool switchArray[10]={switchRead01, switchRead02, switchRead03, switchRead04, switchRead05, switchRead06, switchRead07, switchRead08, switchRead09 , switchRead10};




//----------------END SECTION FOR SWITCHES------------------------------------------

//pinMode(pin#, INPUT);

// select the display class and display driver class in the following file (new style):
#include "GxEPD2_display_selection_new_style.h"

void setup()
{


  //initialize switches
  pinMode(switchInput01, INPUT_PULLUP);
  pinMode(switchInput02, INPUT_PULLUP);
  pinMode(switchInput03, INPUT_PULLUP);
  pinMode(switchInput04, INPUT_PULLUP);
  pinMode(switchInput05, INPUT_PULLUP);
  pinMode(switchInput06, INPUT_PULLUP);
  pinMode(switchInput07, INPUT_PULLUP);
  pinMode(switchInput08, INPUT_PULLUP);
  pinMode(switchInput09, INPUT_PULLUP);
  pinMode(switchInput10, INPUT_PULLUP);
  pinMode(switchInput11, INPUT_PULLUP);
  pinMode(switchInput12, INPUT_PULLUP);
  pinMode(switchInput13, INPUT_PULLUP);


  //-----------ANTENNA-------------------------
  //initialize output meters and joystick
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(left, INPUT_PULLUP);
  pinMode(right, INPUT_PULLUP);
//  pinMode(restart, INPUT);
//  pinMode(antennaLock, OUTPUT);
  pinMode(azimuthGauge, OUTPUT);
  pinMode(elevationGauge, OUTPUT);

  //Attaching servos and setting initial position
  pan.attach(panServo);
  tilt.attach(tiltServo);
  pan.write(panInitial);
  delay(50);
  tilt.write(tiltInitial);
  delay(50);
    analogWrite(azimuthGauge,azimuthInitial);
  analogWrite(elevationGauge,elevationInitial);
  panPos = panInitial;
  tiltPos = tiltInitial;
  
  azimuthPos = azimuthInitial;
  elevationPos = elevationInitial;


  //------------END ANTENNA-------------------
  
  Serial.begin(9600);
  display.init();
  helloURC();
  delay(2000);
  statusScreen();
  delay(200);
}


/*void checkSwitches()
{

  
}
/**/



//keyboard
// This function intercepts key press

char keyLogger[16];

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


//  Serial.print(keyLogger[0]);
//  Serial.print(keyLogger[1]);
//  Serial.print(keyLogger[2]);
//  Serial.print(keyLogger[3]);
//  Serial.print(keyLogger[4]);
//  Serial.print(keyLogger[5]);
//  Serial.print(keyLogger[6]);
//  Serial.print(keyLogger[7]);
//  Serial.print(keyLogger[8]);
//  Serial.print(keyLogger[9]);
//  Serial.print(keyLogger[10]);
//  Serial.print(keyLogger[11]);
//  Serial.print(keyLogger[12]);
//  Serial.print(keyLogger[13]);
//  Serial.print(keyLogger[14]);
//  Serial.print(keyLogger[15]);
//  Serial.println("");

  
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
    display.setPartialWindow(x_input-24,y_input-24,360,48);
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
    
// This function intercepts key release
//void keyReleased() {
//  //Serial.print("Released: ");
//  printKey();
//}

//void printKey() {
//  // getOemKey() returns the OEM-code associated with the key
//  Serial.print(" key:");
//  Serial.print(keyboard.getOemKey());
//
//  // getModifiers() returns a bits field with the modifiers-keys
//  int mod = keyboard.getModifiers();
//  Serial.print(" mod:");
//  Serial.print(mod);
//
//  Serial.print(" => ");
//
//  if (mod & LeftCtrl) {
//    Serial.print("L-Ctrl ");
//  }
//  if (mod & LeftShift) {
//    Serial.print("L-Shift ");
//  }
//  if (mod & Alt) {
//    Serial.print("Alt ");
//  }
//  if (mod & LeftCmd) {
//    Serial.print("L-Cmd ");
//  }
//  if (mod & RightCtrl) {
//    Serial.print("R-Ctrl ");
//  }
//  if (mod & RightShift) {
//    Serial.print("R-Shift ");
//  }
//  if (mod & AltGr) {
//    Serial.print("AltGr ");
//  }
//  if (mod & RightCmd) {
//    Serial.print("R-Cmd ");
//  }
//
//    // getKey() returns the ASCII translation of OEM key
//  // combined with modifiers.
//  Serial.write(keyboard.getKey());
//  Serial.println();
//}
//end keyboard

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

  //read switches
  if(digitalRead(switchInput01) == LOW){switchArray[0] = true;}
  if(digitalRead(switchInput02) == LOW){switchArray[1] = true;}
  if(digitalRead(switchInput03) == LOW){switchArray[2] = true;}
  if(digitalRead(switchInput04) == LOW){switchArray[3] = true;}
  if(digitalRead(switchInput05) == LOW){switchArray[4] = true;}
  if(digitalRead(switchInput06) == LOW){switchArray[5] = true;}
  if(digitalRead(switchInput07) == LOW){switchArray[6] = true;}
  if(digitalRead(switchInput08) == LOW){switchArray[7] = true;}
  if(digitalRead(switchInput09) == LOW){switchArray[8] = true;}
  if(digitalRead(switchInput10) == LOW){switchArray[9] = true;}

  Serial.print(switchArray[0]);




  
  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  //display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center the bounding box by transposition of the origin:
  //uint16_t x = ((display.width() - tbw) / 2) - tbx;
  //uint16_t y = ((display.height() - tbh) / 2) - tby;
  //top left of screen
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


void loop() {
  
  //attach USB task
  usb.Task();


//  if(digitalRead(restart) == HIGH){
//    digitalWrite(antennaLock, LOW);
//    Serial.println("Resetting");
//    //Reset servos and gauges
//    pan.write(panInitial);
//    delay(500);
//    tilt.write(tiltInitial);
//    delay(500);
//    analogWrite(azimuthGauge,azimuthInitial);
//    analogWrite(elevationGauge,elevationInitial); 
//    //Reset postions
//    panPos = panInitial;
//    tiltPos = tiltInitial;
//    azimuthPos = azimuthInitial;
//    elevationPos = elevationInitial;
//    digitalWrite(antennaLock, LOW);
//   }

 //right-pan-azimuth
  if(digitalRead(right) == LOW && azimuthPos <= servoMax){
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
  if(digitalRead(left) == LOW && azimuthPos >= servoMin){
    if( millis() > time_now + periodServo)
    {
      time_now = millis();    
      //Move elevation gauge
      azimuthPos = azimuthPos - jogSpeed;
      Serial.print("Az Gauge Pos: ");
      Serial.print(azimuthPos);
      analogWrite(azimuthGauge,azimuthPos);
  
      //Move pan servo
      panPos = panPos - jogSpeed;
      pan.write(panPos);
      //delay(500);
      Serial.print("   Servo left: ");
      Serial.println(panPos);
  
  
      //If both gauges are lined up, then send signal to the Pi
      if(azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin){
       Serial.println("postition locked");
       antennaAligned = true; 
      }
    }
   }

//up-tilt-elevation
  if(digitalRead(up) == LOW && elevationPos <= servoMax){
    if( millis() > time_now + periodServo)
    {
      time_now = millis();      
      //Move elevation gauge
      elevationPos = elevationPos + jogSpeed;
      Serial.print("Ev Gauge Pos: ");
      Serial.print(elevationPos);
      analogWrite(elevationGauge,elevationPos);
  
      //Move pan servo
      tiltPos = tiltPos + jogSpeed;
      tilt.write(tiltPos);
      //delay(500);
      Serial.print("  Servo Up: ");
      Serial.println(tiltPos);
  
      
      //If both gauges are lined up, then send signal to the Pi
      if(azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin){
       Serial.println("postition locked");
       antennaAligned = true; 
      }
    }
   }

//down-tilt-elevation
  if(digitalRead(down) == LOW && elevationPos >= servoMin){
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
  
      //If both gauges are lined up, then send signal to the Pi
      if(azimuthPos <= desiredAzimuthMax && azimuthPos >= desiredAzimuthMin && elevationPos <= desiredElevationMax && elevationPos >= desiredElevationMin){
       Serial.println("postition locked");
       antennaAligned = true;
      }
    }
    
   }

//Delay so they can look at the gauges

//delay(50);
  

}
