const int ledPin = 12;   //ledPin
const int buttonPin = 23; //Button to perform interrupt 
int x=0, y=0;            //some variables to increment 
int ledToggle = LOW;     //led state 

//variables to keep track of the timing of recent interrupts
unsigned long button_time = 0;  
unsigned long last_button_time = 0; 

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(buttonPin), button_ISR, CHANGE);
}

void loop(){
  Serial.print("loop " );
  Serial.print(x++ );
  Serial.println();
  delay(500);
}

void button_ISR(){
  button_time = millis();
  //check to see if increment() was called in the last 250 milliseconds
  if (button_time - last_button_time > 250){
    Serial.print("Interrupt ");
    Serial.print(y++);
    Serial.println();
    ledToggle = !ledToggle;
    digitalWrite(ledPin, ledToggle);
    last_button_time = button_time;
    }
}
