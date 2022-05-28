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

const byte switchInputArray[13] = {switchInput01, switchInput02, switchInput03, switchInput04, switchInput05, switchInput06, switchInput07, switchInput08, switchInput09,switchInput10, switchInput11, switchInput12, switchInput13}:
bool switchArrayRead[13]={switchRead01, switchRead02, switchRead03, switchRead04, switchRead05, switchRead06, switchRead07, switchRead08, switchRead09 , switchRead10, switchRead11, switchRead12, switchRead13};

//variables to keep track of the timing of recent interrupts
unsigned long switch_time01 = 0;
unsigned long switch_time02 = 0;
unsigned long switch_time03 = 0;
unsigned long switch_time04 = 0;
unsigned long switch_time05 = 0;
unsigned long switch_time06 = 0;
unsigned long switch_time07 = 0;
unsigned long switch_time08 = 0;
unsigned long switch_time09 = 0;
unsigned long switch_time10 = 0;
unsigned long switch_time11 = 0;
unsigned long switch_time12 = 0;
unsigned long switch_time13 = 0;

unsigned long last_switch_time01 = 0;
unsigned long last_switch_time02 = 0;
unsigned long last_switch_time03 = 0; 
unsigned long last_switch_time04 = 0; 
unsigned long last_switch_time05 = 0; 
unsigned long last_switch_time06 = 0; 
unsigned long last_switch_time07 = 0; 
unsigned long last_switch_time08 = 0; 
unsigned long last_switch_time09 = 0;
unsigned long last_switch_time10 = 0;
unsigned long last_switch_time11 = 0;
unsigned long last_switch_time12 = 0;
unsigned long last_switch_time13 = 0;

void setup() {
  for (byte i = 0; i < 14; i++){
    pinMode(switchInputArray[i], INPUT_PULLUP)
  }
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(switchInput01), switch01_ISR, LOW);
  attachInterrupt(digitalPinToInterrupt(switchInput02), switch02_ISR, LOW);
  attachInterrupt(digitalPinToInterrupt(switchInput03), switch03_ISR, LOW);
  attachInterrupt(digitalPinToInterrupt(switchInput04), switch04_ISR, LOW);
  attachInterrupt(digitalPinToInterrupt(switchInput05), switch05_ISR, LOW);
  attachInterrupt(digitalPinToInterrupt(switchInput06), switch06_ISR, LOW);
  attachInterrupt(digitalPinToInterrupt(switchInput07), switch07_ISR, LOW);
  attachInterrupt(digitalPinToInterrupt(switchInput08), switch08_ISR, LOW);
//  attachInterrupt(digitalPinToInterrupt(switchInput09), switch09_ISR, LOW);
//  attachInterrupt(digitalPinToInterrupt(switchInput10), switch10_ISR, LOW);
//  attachInterrupt(digitalPinToInterrupt(switchInput11), switch11_ISR, LOW);
//  attachInterrupt(digitalPinToInterrupt(switchInput12), switch12_ISR, LOW);
//  attachInterrupt(digitalPinToInterrupt(switchInput13), switch13_ISR, LOW);
 
  
}

void loop(){
  Serial.print("loop " );
  Serial.print(x++ );
  Serial.println();
  delay(500);
}

void switch01_ISR(){
  switch_time01 = mills();
  if (switch_time01 - last_switch_time01 > 250){
    Serial.print("Interrupt1 ");
    Serial.print(y++);
    Serial.println();
    last_switch_time01 = switch_time01;
    }
  }

  void switch02_ISR(){
  switch_time02 = mills();
  if (switch_time02 - last_switch_time02 > 250){
    Serial.print("Interrupt2 ");
    Serial.print(y++);
    Serial.println();
    last_switch_time02 = switch_time02;
    }
  }


  void switch03_ISR(){
  switch_time03 = mills();
  if (switch_time03 - last_switch_time03 > 250){
    Serial.print("Interrupt3 ");
    Serial.print(y++);
    Serial.println();
    last_switch_time03 = switch_time03;
    }
  }


   void switch04_ISR(){
  switch_time04 = mills();
  if (switch_time04 - last_switch_time04 > 250){
    Serial.print("Interrupt4 ");
    Serial.print(y++);
    Serial.println();
    last_switch_time04 = switch_time04;
    }
  }

   void switch05_ISR(){
  switch_time05 = mills();
  if (switch_time05 - last_switch_time05 > 250){
    Serial.print("Interrupt5 ");
    Serial.print(y++);
    Serial.println();
    last_switch_time05 = switch_time05;
    }
  }

   void switch06_ISR(){
  switch_time06 = mills();
  if (switch_time06 - last_switch_time06 > 250){
    Serial.print("Interrupt6 ");
    Serial.print(y++);
    Serial.println();
    last_switch_time06 = switch_time06;
    }
  }

   void switch07_ISR(){
  switch_time07 = mills();
  if (switch_time07 - last_switch_time07 > 250){
    Serial.print("Interrupt7 ");
    Serial.print(y++);
    Serial.println();
    last_switch_time07 = switch_time07;
    }
  }

   void switch08_ISR(){
  switch_time08 = mills();
  if (switch_time08 - last_switch_time08 > 250){
    Serial.print("Interrupt8 ");
    Serial.print(y++);
    Serial.println();
    last_switch_time08 = switch_time08;
    }
  }
