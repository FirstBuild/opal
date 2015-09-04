// opal_9_1
// This program drives the neopixels on the nugget ice prototype responds to some environmental iputs. 
// The core of this program is a switch case statement that selects the current display mode.
// Each function runs through the display linearly, so it sets the leds immediately then waits within the
// function until the next time it needs the update. In opal_timer each light mode works as a function of time.
// Authors: Ricardo Aguiar and John Nolan
// last updated September 1

#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
//#include <chillhub.h>
//#include <crc.h>
#include <QueueArray.h>

#define CAPTOUCH   2    // Digital IO pin connected to the button.  This will be
                          // driven with a pull-up resistor so the switch should
                          // pull the pin to ground momentarily.  On a high -> low
                          // transition the button press logic will execute.
#define MOTORRPM 3        // motor feedback
#define MOTORPWM 4        // rate at which we want to drive the motor
#define ICE 5             // The led's that illuminate the ice inside of the bucket
#define NEOPIXEL    6     // Digital IO pin connected to the NeoPixels.
#define DAYNIGHT 7        // Logic input, either dim all lights or keep them bright
#define MOTORTIMOUT 8     // count in between pulses so we know when the motor times out
#define COMPRESSOR 9      // drive the relay to turn on the compressor
#define PUMP     10       // turn the MOSFET on to drive the waterpump

#define COMPRESSORFAN 12   // turn on the MOSFET to turn on the fan
#define PIXEL_COUNT 12     // # of pixels in on the neopixel ring
#define ICELEVELTEMP A2    // temperature reading of the ice in the bucket to stop making ice
#define MOLDBODYTEMP A3    // temperaturne of the mold body  
#define IRSENSOR A4        // how much ice in the bucket

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, NEOPIXEL, NEO_GRB + NEO_KHZ800);

bool oldState = HIGH;                                            
int showType = 4;// "showType" initial state

//ir sensor setup
float IR = A4; 
float IRvalue;
float IRavg[10];
float distance = 0;
boolean flag =false;;
QueueArray<float> averageQueue;
const int AVG_COUNT = 100;


void setup() {
  pinMode(CAPTOUCH, INPUT);                             // Set the pin as in input (might need to be INPUT not INPUT_PULLUP)
  pinMode(ICE, OUTPUT);             // set up pin to send signal to transistor
  pinMode(PUMP, OUTPUT); // send signals to the pump 
  pinMode(COMPRESSOR, OUTPUT);
  pinMode(COMPRESSORFAN, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(ICELEVELTEMP, INPUT); //-
  pinMode(IRSENSOR, INPUT); //-
  digitalWrite(A0, HIGH); // set low water pin to input_pullup
  pinMode(A1, INPUT);
  digitalWrite(A1, HIGH); // set high water pin to input_pullup
  pinMode(MOTORPWM, OUTPUT); 
  analogWrite(MOTORPWM, 127);
  strip.begin();                                                 // This initializes the NeoPixel library.
  strip.show(); 
  Serial.begin(9600);	// Debugging only
  Serial.println("setup");
  
// Initialise the IO and ISR
// Initialize all pixels to 'off'  
//digitalWrite(ICE, HIGH);         // just set leds on for now, might use interrupt to change when motor powers on
 digitalWrite(COMPRESSOR, HIGH);
 digitalWrite(COMPRESSORFAN, HIGH);

 powerOn(strip.Color(0, 255, 0), 20);  // power on cycle // it might miss the char that exits this
}
//---------------------------------------------------------------

void loop() {

  ICELEVELDETECT1();          //--  
  MOLDBODYTEMPERATURE();             //--
  Dim();                            //--
  double id = analogRead(IR) *5/1023;
  IRvalue = id;
  averageQueue.enqueue(IRvalue);
   if (averageQueue.count() >= AVG_COUNT) {
     distance = getAverage();
     distance = 0.0048*id*id-0.13*id+1.2403;
     Serial.print("Distance away from the IR Sensor (cm): ");
     Serial.println(distance);
     Serial.print("Dimmed: "); 
     Serial.println(digitalRead(DAYNIGHT));
   }
   Wait(20);
   /**********start Show*************/
 /*if(startShow(showType)) { // detected a button press during the wait block
      if (showType > 5) showType = 0; // testing (from 100)
      else showType++; // increment if 0 through 5
  }*/
  /******Un comment this to see the neopixel display******
   *This statement runs the lights with the current 'showtype' (0 through 5) and increments
   * the showtype when the user presses a button. This let us cycle through the different
   * light states. The problem is that we read the IR sensor in this loop. Unless we use an interrupt
   * there was not a good way to keep a constant average.
    */
    /*****************************/
     delay(20);
}

//----------------------------------------------------------------
//"startShow" is called in the loop and contains all other functions within itself
// Assign colors here (R, G, B).  0 being off, 255 being the brightest.
char startShow(char c) {
  switch(c){ // switch input from the transmitter

    case 0: //filling up the ice case
           Serial.println("Starting Ice");
           return Ice(0, 0, 155, 50);    //color function of ice
           break;
           
    case 1: for (uint16_t ioff=0; ioff <= 11; ioff++) { // no ice case, slosh with top half off
                setRing(ioff, 0, 0, 0); // set top half off
            }
            return Slosh(15, 5, 45, 1000); 
            break;
            
    case 2: for (uint16_t iw= 0; iw <= 11; iw++) { // needs water case, slosh with top half white
                setRing(iw, 100, 100, 100); // set top half color, white
            }
            return Slosh(5, 3, 25, 1000);
            break;
            
    case 3: // ice bucket full, strobe with white
            return fullIce(20, 1000); 
            break;
            
    case 4: // cleaning cycle (with yellow) 
            return Clean(255, 255, 0, 500);
            break;
            
    case 5: // you messed something up
            return Error(strip.Color(255, 0, 0), 2000);
            break;
            
    case 6: break; // no button down, skip this state
    default: return Error(strip.Color(255, 0, 0), 2000);
             break;
}
}
//---------------------------------------------------------------------


//-------------------------------------------------------------------
//"Nugget-Ice power on"
//when the unit powers up pulse green then back off. 

char powerOn(uint32_t c, uint8_t wait) {
  char sigc;

 for (int ic = 0; ic <= 25; ic++) {
  for(int i=0;i<PIXEL_COUNT;i++){
    setRing(i, ic, 105, ic); 
  } 
  strip.show();
  
  if (sigc = Wait(wait)) return sigc;
 }
 
  for (int ic = 25; ic >= 0; ic--) {
  for(int i=0;i<PIXEL_COUNT;i++){
    setRing(i, ic, 105, ic); 
  } 
  strip.show();
  if (sigc = Wait(wait)) return sigc;
  
  }
 
  if (sigc = Wait(1500)) return sigc;
 
 for (int ic = 105; ic >= 0; ic--) {
   for(int i=0;i<PIXEL_COUNT;i++){
    setRing(i, 0,ic,0); 
 }
 strip.show();
  if (sigc = Wait(wait)) return sigc;
 } // fade out
 if (sigc = Wait(wait)) return sigc;
 }
//----------------------------------------------------------------------


int Wait(int time) { // wait for button press this amount of time

  unsigned long previousMillis;
  unsigned long buttonMillis; // time of last button press
  unsigned long defaultMillis;
  int buttonDebounce = 10;//10;
  boolean first_press = 0;
  boolean first_default_press = 0;
  boolean pressed = 0;
  boolean newDefaultState = HIGH;  //invert all default states to switch when releasing the button
  boolean oldDefaultState = LOW;
  boolean newState = LOW; // button state just read
  boolean oldState = HIGH; // button state immediately before

  if (time < 50) time = 50; // needs some extra time to debounce
  
  previousMillis = millis(); // set previous Millis for counter once
  do {
    delay(5); // give it some extra time.
      //currentMillis = millis(); // continually reset the current count
       newState = digitalRead(CAPTOUCH);                      // read the button
       //Serial.println(newState);
       //newDefaultState = digitalRead(BUTTON);
  if (newDefaultState == LOW && oldDefaultState == HIGH ) {                    // Check if state changed from high to low (button press).
     defaultMillis = millis(); // begin timing
     first_default_press = 1; // probably unecessary, since oldstate must be high while new is low at some point
     Serial.println("Red Button!");
  } else if (newState == HIGH && oldState == LOW ) {                    // Check if state changed from high to low (button press).
     //oldstate = newstate; // reset to stay out of this if statement
     buttonMillis = millis(); // begin timing
     first_press = 1; // keep these two button presses seperats.
  }
  if (millis() - defaultMillis > buttonDebounce && first_default_press && newDefaultState == LOW) {     // Short delay to debounce button.
     pressed = 1; // if enought time has pressed since last affirmative read, reset timer
     first_default_press = 0;
  } else if (millis() - buttonMillis > buttonDebounce && first_press && newState == HIGH) {     // Short delay to debounce button.
     pressed = 1; // if enought time has pressed since last affirmative read, reset timer
     first_press = 0;
  }      
  oldState = newState;                                          // Set the last button state to the old state.
  oldDefaultState = newDefaultState;
  //Serial.println(millis() - previousMillis); 
 
 // check reed switch logic //
 
 if (digitalRead(A1) == LOW) { 
   digitalWrite(PUMP, LOW); // set pump motor off when high tank signal received

 } else if (digitalRead(A0) == LOW) { // check if the signals are pulled to ground
   digitalWrite(PUMP, HIGH); // set pump on when low tank signal received

 }
 
} while ((millis() - previousMillis) < time && !pressed);
  return pressed;
}
//-----------------------------------------------------------------------
//"NEED MORE WATER" 
//move blue clockwise through the bottom four pixels
//move a lighter blue counterclockwise throught the bottom four pixels
int Slosh(int rb, int gb, int bb, uint8_t wait) {
  
  int sigc;
  
  for (int irepeat = 0; irepeat <= 2; irepeat++) {
    // sloshes bottom rather than the right side as before. (the neopixels should be rotated.
    // Start at about 3, go to 8 in a rotation of essentially 0 through -180, 
    //considering pixel 0 as 90deg.
    if (sigc = lerpWheel(0, 0, 250, rb, gb, bb, 8, 3, wait)) return sigc; // slosh-to // was lerping 5 to 0
     //if (sigc = Wait(20)) return sigc;

   // strip.show();
    if(sigc = lerpWheel(0, 0, 250, rb, gb, bb, 3, 8, wait)) return sigc; // slosh-fro
     //if (sigc = Wait(20)) return sigc;//delay(20);
   // strip.show();
  }
}
//------------------------------------------------------------------------


//------------------------------------------------------------------------
//"ice making starts"
//ring is blue with white begining on top and slowly coming down both clock-wise and counter clock-wise

int Ice(int rb, int gb, int bb, uint8_t wait){
  int sigc;  
  for(uint16_t a=0; a <= 11; a++) { 
      setRing(a, rb, gb, bb);
      strip.show();
  }
   if (sigc = Wait(200)) return sigc; //delay(200);
   Serial.println("Passed ice wait");
   
   for(uint16_t a=11; a > 5; a--) {
      if (sigc = lerpPixels(rb, gb, bb, 105, 105, 105, 1000, a, 11 - a)) return sigc;    //white
      //setRing(11-a, 255, 255, 255);
      strip.show();
      //delay(1000);
   }
  return sigc;
}
//--------------------------------------------------------------------------



//-------------------------------------------------------------------------
//"Full Icebucket"
//White LED's will pulse slowly off and on only once.
//When the icebucket is full, the pulsing stops and the ring is lit 3/4 white

int fullIce(uint8_t b, uint32_t wait) { // specify brightness and the time for one transition from light to dark
   int sigc; // the signal character from the transmitter
   int levels = (155 - b)/10;
   uint32_t level_delay; // delay between each light level change
   //Serial.println("In fullIce");
   if (levels <= 0) {
         level_delay = wait;
   }
   else {
         level_delay = wait/levels; // pulse should take the length of time specified by wait
   }
  
   for (int ib = 155; ib > b; ib-=10) {
        for (uint8_t ip = 0; ip <= 11; ip++) { // apply to all pixels
            setRing(ip, ib, ib, ib);
            //setRing(ip, ib, ib, ib);
            strip.show();
        }
         if (sigc = Wait(level_delay)) return sigc; //delay(level_delay);
         Serial.println(ib);
   }
   for (int ib = b; ib < 155; ib+=10) {
        for (uint8_t ip = 0; ip <= 11; ip++) { // apply to all pixels
            setRing(ip, ib, ib, ib);
            //setRing(ip, ib, ib, ib);
            strip.show();
        }
        Serial.println(ib);
         if (sigc = Wait(level_delay)) return sigc; //delay(level_delay);
   }
   return sigc;
}
//-----------------------------------------------------------------------


//-------------------------------------------------------------------------
//"Nugget-Ice Error"
//when something is wrong, harshly blink red 

int Error(uint32_t c, uint8_t wait) {
  
  int sigc;

for (int istrobe = 0; istrobe < 3; istrobe++) {

  for(int ir=0;ir<PIXEL_COUNT;ir++){
    strip.setPixelColor(ir, c); 
    //TODO, change this color so setRing can use it
    strip.show(); 
  }
   if (sigc = Wait(200)) return sigc; //delay(50); // button needed more time
 
   for(int i=0;i<PIXEL_COUNT;i++){
    setRing(i, 25,0,0); // dimmer red
    strip.show(); 
 }
  if (sigc = Wait(200)) return sigc; //delay(50);
}

for(int ir=0;ir<PIXEL_COUNT;ir++){
    strip.setPixelColor(ir, c); 
    strip.show(); 
  }
  //if (sigc = Wait(24000)) return sigc;
  
  return sigc;
}
/*-------------------------------------------------------------------------------*******/
//"Cleaning Cycle"
/*Yellow chasing no light at the begining of the cycle.  Lighter yellow chasing a backlit light yellow. 
Pulse yellow as the cleaning cycle finishes.*/

int Clean(int rd, int gd, int bd, uint8_t wait){

  int sigc;
  int pbr, pbg, pbb, pdr, pdg, pdb; // progress background and dot color
  int ebr, ebg, ebb, edr, edg, edb; // empty portion background and dot color
  int progress_wait; // time to wait between each pixel in progress wheel
  int ifade; // for last stage
  
  pbr = 105; // progress background
  pbg = 105;
  pbb = 0;
  pdr = 155; // progress dot
  pdg = 155; // describes the color in the area that has been filled
  pdb = 25;
  ebr = 20; // empty background
  ebg = 20;
  ebb = 5;
  edr = 50; // empty dot
  edg = 50; // describes the color in the unfilled region
  edb = 25;
  progress_wait = 140;

  
  for (int icount = 0; icount < 3; icount++) { // initial user response loops (3)
    if (sigc = lerpWheel(rd, gd, bd, 0, 0, 0, 0, 11, 85)) return sigc;
  }
  
   if (sigc = Wait(wait)) return sigc;
  for (int ib = 0; ib < 12; ib ++) setRing(ib, ebr, ebg, ebb); 
  // set everything to the empty background
  
   
   //if (sigc = lerpWheel(p
 for(int chase = 1; chase <= 10; chase++) { // increment lerpwheel functions
      if (sigc = lerpWheel(pdr, pdg, pdb, pbr, pbg, pbb, 0, chase, progress_wait)) return sigc; // stop at chase
      setRing(chase, pbr, pbg, pbb);
      if (sigc = lerpWheel(edr, edg, edb, ebr, ebg, ebb, chase + 1, 12, progress_wait)) return sigc; // from chase (transition between two bars) to the end
      setRing(11, ebr, ebg, ebb);
      // dark dot on a dimmer background
 }
if (sigc = lerpWheel(pdr, pdg, pdb, pbr, pbg, pbb, 0, 11, progress_wait)) return sigc; // last wheel

  if (sigc = Wait(wait)) return sigc; //delay(wait);
 
  for ( ifade = pbr; ifade > 5; ifade-=5) { // pbr and pbg should be the same
        for (uint8_t ip = 0; ip <= 11; ip++) { // apply to all pixels
            setRing(ip, ifade, ifade, 0);
            strip.show();
        }
         if (sigc = Wait(45)) return sigc; //delay(level_delay);
   }
   
   if (sigc = Wait(400)) return sigc; 
   
   for (; ifade < pbr; ifade+=5) {
        for (uint8_t ip = 0; ip <= 11; ip++) { // apply to all pixels
            setRing(ip, ifade, ifade, 0);
            strip.show();
        }
         if (sigc = Wait(45)) return sigc; //delay(level_delay);
   }
   
  if (sigc = Wait(2000)) return sigc;
  
 strip.show();
 return sigc; //delay(wait);
} 

//-----------------lerpPixel----------------------//
int lerpPixels (int R_s, int G_s, int B_s, int R_e, int G_e, int B_e, int change_time, int led_if, int led_is) { // starting and ending colors, time to change color, first and second led
  // crossfades two pixels from one color to another
   int dt = 100; // preset timedelay for each light dim (from 20)
   int steps; // number of light changes in each transition
   int dr, dg, db; // individual changes for each color
   int sigc;
   
   if (change_time < dt) {
     steps = 1;
   } else {
     steps = change_time/dt;
   }
   
   dr = (R_e - R_s)/steps; // difference between two reds divided by the number of steps
   // add to background color to reach Rdot in calculated number of steps, subtract from Rdot to reach the background within the same interval
   dg = (G_e - G_s)/steps;
   db = (B_e - B_s)/steps;
   
   for (int i_s = 0; i_s < steps; i_s++) {
     R_s += dr;
     G_s += dg;
     B_s += db;
     setRing(led_if, R_s, G_s, B_s);
     setRing(led_is, R_s, G_s, B_s);
     strip.show();
     if (sigc = Wait(dt)) return sigc; //delay(dt); 
  }
   return sigc;
}

//-----------------lerpWheel----------------------//
int lerpWheel (int Rdot, int Gdot, int Bdot, int Rback, int Gback, int Bback, int istart, int iend, int change_time) {
  // smoothly slide a dot along a background, specifying the RGB of the dot and the background,
 // the starting and ending index, and the time to transition between pixels
   
   int dt = 100; // preset timedelay for each light dim (from 20)
   int steps; // number of light changes in each transition
   int dr, dg, db; // individual changes for each color
   int Ro, Go, Bo; // rgb value of current pixel
   int Rl, Gl, Bl; // rgb value of last pixel
   int di= abs(iend - istart)/(iend - istart);
   int sigc;

   if (change_time < dt) {
     steps = 1;
   } else {
     steps = change_time/dt;
   }
   
   dr = (Rdot - Rback)/steps; // difference between two reds divided by the number of steps
   // add to background color to reach Rdot in calculated number of steps, subtract from Rdot to reach the background within the same interval
   dg = (Gdot - Gback)/steps;
   db = (Bdot - Bback)/steps;

  for (int iled_b = 0; iled_b <= abs(iend - istart); iled_b++) { // di depends upon relative position of iend and istart
    setRing(istart + iled_b*di, Rback, Gback, Bback);
  } // problem here?
  setRing(istart, Rback, Gback, Bback);
  //strip.show();
 // if (sigc = Wait(dt)) return sigc; //delay(dt); // if there was a button press, not if equal.
  Ro = Rdot;
  Go = Gdot;
  Bo = Bdot;
  
  setRing(istart, Ro, Go, Bo);
  strip.show();
  if (sigc = Wait(dt)) return sigc; //delay(dt); 
  
  for (int iled = 0; iled < abs(iend - istart); iled++) { // di moves it forward or back depending on positioning
  // count through total number of leds in the range
 // changed from iled = 0
    Rl = Ro; 
    Gl = Go;
    Bl = Bo;
    Ro = Rback;
    Go = Gback;
    Bo = Bback;
    for (int i = 0; i < steps; i++) {
     Ro += dr;
     Go += dg;
     Bo += db;
     Rl -= dr;
     Gl -= dg;
     Bl -= db;
     setRing(istart + di + iled*di, Ro, Go, Bo); // find the leds based on the position in the range, iled
     setRing(istart + iled*di, Rl, Gl, Bl);
     strip.show();
     if (sigc = Wait(dt)) return sigc; //delay(dt);
   } // end dimming for loop
  } // end led for loop
  
  return sigc;
}// end function

//-------------------DECTECTICEINBUCKET---------------------------------//
void ICELEVELDETECT1(void) {
  static unsigned long previous = millis();
  unsigned long current = millis();
  double ilt = analogRead(ICELEVELTEMP);

  if ((current - previous) > 5000) {
    previous = current;
    Serial.print("Ice Level Temperature (F): ");
    Serial.println(0.0002*ilt*ilt+0.0935*ilt-31.835);
  }
} //END ICE DETECTION INSIDE OF THE BUCKET

//-----------------DETECTMOLDBODYTEMP----------------------------------//
void MOLDBODYTEMPERATURE(void) {
  static unsigned long previous = millis();
  unsigned long current = millis();
  double mbt = analogRead(MOLDBODYTEMP);

  if ((current - previous) > 5000) {
    previous = current;
    Serial.print("Mold Body Temp (F): ");
    Serial.println(0.0002*mbt*mbt+0.0935*mbt-31.835);
  }
} //END MOLDBODY TEMPERATURE DETECTION


//----------------------IRSENSOR---------------------------------------//

float getAverage() {
  float average;
 
  int count = averageQueue.count(); // number of items in queue before dequeing
  while (!averageQueue.isEmpty()) {
    average += averageQueue.dequeue(); // add all the members
  }
  average = average / count;
  
  return average;
}

//----------------------DAY/NIGHT----------------------------------------//

void Dim() {
  
  if (digitalRead(DAYNIGHT) == LOW) {
    analogWrite (ICE, 45);
  }
  else {
    analogWrite (ICE, 255);
}
}

void setRing(int i, int r, int g, int b) {
  // replicates strip.setPixelColor with the dimming switch taken into account
  float scale = 1.0;
  
  if (!digitalRead(DAYNIGHT)) {
    // dim by more than a sixth if the switch is off
    scale = 0.15; // dim factor
  }

  strip.setPixelColor(i, strip.Color(r*scale, g*scale, b*scale));
}
  
