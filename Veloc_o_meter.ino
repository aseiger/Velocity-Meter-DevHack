/*This program is used to test the Digital timer display inside the box! Use this to upload if something goes bad

*/

//Constants to set pin numbers
const int data = 0;
const int clock = 1;

const int clear_btn = 5;
const int function_btn = 8;
const int stop_btn = 6;

const int gate_switch_3 = 13;
const int gate_switch_10 = 7;

const int meters_second = 9;
const int cmeters_second = 11;
const int msec =10 ;
const int sec = 12;

//const long sensordist = 203200;
volatile int state1 = LOW;
volatile int state2 = LOW;
volatile int mode = 1;
volatile long freqcounter = 0;

const byte ready_disp = B11111110;
const byte error_disp = B11011111;
const byte measured_disp = B10011111;



//setup
void setup() {
  pinMode(data, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(gate_switch_3, INPUT);
  pinMode(gate_switch_10, INPUT);
  pinMode(clear_btn, INPUT);
  pinMode(function_btn, INPUT);
  pinMode(stop_btn, INPUT);
  pinMode(meters_second, OUTPUT);
  pinMode(cmeters_second, OUTPUT);
  pinMode(msec, OUTPUT);
  pinMode(sec, OUTPUT);
  writeToDisplay(-1, 1, ready_disp); 
  delay(1000);
}

void loop() {
  //Read in function data
  //if function button is held in at reset of program, go into mode select mode
  delay(500);
  //detach interrupts
  detachInterrupt(0);
  detachInterrupt(1);
  if (!digitalRead(function_btn)){
    writeToDisplay(mode, 0, B11111111);
      //wait for function_btn release
      while (!digitalRead(function_btn)){}
      //increment function until stop button is pressed
      while (digitalRead(stop_btn)){
        if (!digitalRead(function_btn)){
          if (mode >= 4){
            mode = 1;
          }
          else {
            mode++;
          }
          //write mode to display
          writeToDisplay(mode, 0, B11111111);
          //wait for function button release
          while(!digitalRead(function_btn)){
          }
          delay(100);
        }
      }
  }
  
 //Meters/Per/Second mode 
 if (mode == 1) { 
  //Display M/S mode
  attachInterrupt(0, breakBeam1, RISING);
  attachInterrupt(1, breakBeam2, RISING);
  
  digitalWrite(meters_second, 0);
  digitalWrite(cmeters_second, 1);
  digitalWrite(msec, 0);
  digitalWrite(sec, 0);  


  unsigned long time_current = 0;
  unsigned long time_between = 0;
  long um_per_uec = 0;
  unsigned long counter = 0;
  unsigned long sensordist = 0;
  //Reset ISR values
  state1 = LOW;
  state2 = LOW;
  //write starting state to display
  writeToDisplay(-1, 0, ready_disp);   
  //wait for first opto to get broken
  while(!state1){
    time_current = micros();
  }
  //wait for second opto to get broken
    //If this doesnt happen after a few seconds, declare error
  while((!state2) && (counter <= 50000)){
    time_between = micros() - time_current;
    counter++;
  }
  
  //determine sensor distance
  if (digitalRead(gate_switch_3)){
    sensordist = 300000;
  }
  else if (digitalRead(gate_switch_10)){
    sensordist = 1000000;
  }
  else {
    sensordist = 500000;
  }
  
  //calculate speed
  um_per_uec = sensordist/time_between;  
  //determine if there was an error, display accordingly
  if (counter >= 50000){
    writeToDisplay(um_per_uec, 1, error_disp);
  }
  else{
    writeToDisplay(um_per_uec, 1, measured_disp);
  }
  
  //wait for user to read display and press clear button
  while(!digitalRead(clear_btn)){
  }
}
//us_between_mode
else if (mode == 2) {
    //Display usec mode
    
  attachInterrupt(0, breakBeam1, RISING);
  attachInterrupt(1, breakBeam2, RISING);
  
  digitalWrite(meters_second, 1);
  digitalWrite(cmeters_second, 1);
  digitalWrite(msec, 0);
  digitalWrite(sec, 1);  


  unsigned long time_current = 0;
  unsigned long time_between = 0;
  unsigned long counter = 0;
  //Reset ISR values
  state1 = LOW;
  state2 = LOW;
  //write starting state to display
  writeToDisplay(-1, 0, ready_disp);   
  //wait for first opto to get broken
  while(!state1){
    time_current = micros();
  }
  //wait for second opto to get broken
    //If this doesnt happen after a few seconds, declare error
  while((!state2) && (counter <= 50000)){
    time_between = micros() - time_current;
    counter++;
  }  
  //determine if there was an error, display accordingly
  if (counter >= 50000){
    writeToDisplay(time_between, 0, error_disp);
  }
  else{
    writeToDisplay(time_between, 0, measured_disp);
  }
  
  //wait for user to read display and press clear button
  while(!digitalRead(clear_btn)){
  }
}
//Frequency Mode
else if(mode == 3){
  attachInterrupt(0, freqcount, RISING);
   //display frequency mode
  digitalWrite(meters_second, 1);
  digitalWrite(cmeters_second, 1);
  digitalWrite(msec, 1);
  digitalWrite(sec, 0);  
  long millisLast = 0;
  long millisNow = 0;
  
  //Reset ISR values
  freqcounter = 0;
  //write starting state to display
  writeToDisplay(-1, 0, ready_disp);   
  //while the clear button is not pressed, measure RPM
  millisLast = millis();
  millisNow = millis();
  while(!digitalRead(clear_btn)){
    millisNow = millis();
    if (millisNow >= millisLast + 100){
      writeToDisplay((freqcounter*(millisNow - millisLast))/1000, 2, ready_disp);
      delay(500);
      freqcounter = 0;
      millisLast = millis();
    }
  }
  
}
//RPM Mode
else if(mode == 4){
  attachInterrupt(0, freqcount, RISING);
   //display frequency mode
  digitalWrite(meters_second, 1);
  digitalWrite(cmeters_second, 1);
  digitalWrite(msec, 0);
  digitalWrite(sec, 1);  
  long millisLast = 0;
  long millisNow = 0;
  
  //Reset ISR values
  freqcounter = 0;
  //write starting state to display
  writeToDisplay(-1, 0, ready_disp);   
  //while the clear button is not pressed, measure RPM
  millisLast = millis();
  millisNow = millis();
  while(!digitalRead(clear_btn)){
    millisNow = millis();
    if (millisNow >= millisLast + 1000){
      writeToDisplay((6*((1000*freqcounter)/(millisNow - millisLast))), 2, ready_disp);
      delay(500);
      freqcounter = 0;
      millisLast = millis();
    }
  }
}
}


//==================================================================================
//INTERRUPT SERVICE ROUTINES
//==================================================================================
void breakBeam1() {
  state1 = HIGH;
}

void breakBeam2() {
  state2 = HIGH;
}

void freqcount() {
  freqcounter++;
}

//=============================================================
//Function to easily set mode
//=============================================================
void setMode(int value){
  switch(value) {
    case 1:
      digitalWrite(meters_second, 0);
      digitalWrite(cmeters_second, 1);
      digitalWrite(msec, 0);
      digitalWrite(sec, 0);    
    case 2:
      digitalWrite(meters_second, 1);
      digitalWrite(cmeters_second, 0);
      digitalWrite(msec, 0);
      digitalWrite(sec, 0);    
    case 3:
      digitalWrite(meters_second, 1);
      digitalWrite(cmeters_second, 1);
      digitalWrite(msec, 1);
      digitalWrite(sec, 0); 
    case 4:
      digitalWrite(meters_second, 1);
      digitalWrite(cmeters_second, 1);
      digitalWrite(msec, 0);
      digitalWrite(sec, 1);  
    default:
      digitalWrite(meters_second, 1);
      digitalWrite(cmeters_second, 1);
      digitalWrite(msec, 0);
      digitalWrite(sec, 0);
  }
}

//==================================================================================
//DISPLAY ROUTINES
//=================================================================================

void writeToDisplay(int number, int dplace, byte indicators) {
  /*writes a number to display, with the decimal place and indicators
  Any number put in greater than 9999 will blank the display
  the indicators are relativley self explanitory, just put in a binary number
  with zeros where you want the lights to light up!
  */
  int thoudig;
  int hunddig;
  int tendig;
  int onedig;
  
  //split up number into digits
  thoudig = number/1000;
  hunddig = (number-(thoudig*1000))/100;
  tendig = (number - (thoudig*1000) - (hunddig*100))/10;
  onedig = (number - (thoudig*1000) - (hunddig*100) - (tendig*10));
  
  //place the decimal point
  if(dplace == 1){
    tendig = 10+tendig;
  }
  else if (dplace == 2){
    hunddig = 10+hunddig;
  }
  else if (dplace == 3){
    thoudig = 10+thoudig;
  }
  
  //perform proper digit blanking
  if((hunddig == 0) && (thoudig == 0) && (tendig == 0)){
    tendig = 20;
  } 
  if((hunddig == 0) && (thoudig == 0)){
    hunddig = 20;
  }
  if(thoudig == 0){
    thoudig = 20;
  }
  if ((number > 9999) | (number < 0)) {
    onedig = 20;
    hunddig = 20;
    thoudig = 20;
    tendig = 20;
  }
  
  //shiftout the proper data!
  shiftOut(data, clock, MSBFIRST, convertToSegment(thoudig));
  shiftOut(data, clock, MSBFIRST, convertToSegment(hunddig));
  shiftOut(data, clock, MSBFIRST, convertToSegment(tendig));
  shiftOut(data, clock, MSBFIRST, convertToSegment(onedig));
  shiftOut(data, clock, MSBFIRST, indicators);
}

byte convertToSegment(int number) {
  /*putting in a number between 0 and 9 displays that number
  putting in a number between 10 and 19 displays the ones digit with a decimal point to the right
  putting in a number other than these blanks the segment
  */
  switch(number) {
    case 0:
      return B10000100;
    case 1:
      return B10111110;
    case 2:
      return B11001000;
    case 3:
      return B10011000;
    case 4:
      return B10110010;
    case 5:
      return B10010001;
    case 6:
      return B10000001;
    case 7:
      return B10111100;
    case 8:
      return B10000000;
    case 9:
      return B10110000;
    case 10:
      return B00000100;
    case 11:
      return B00111110;
    case 12:
      return B01001000;
    case 13:
      return B00011000;
    case 14:
      return B00110010;
    case 15:
      return B00010001;
    case 16:
      return B00000001;
    case 17:
      return B00111100;
    case 18:
      return B00000000;
    case 19:
      return B00110000;  
    default:
      return B11111111;
  }
}





