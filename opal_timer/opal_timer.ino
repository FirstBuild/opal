//opal_timer
// This program has never actually been run. I was trying to replicate the linear opal program with timers
// and arrived at this sketch mostly through calculations rather than testing. The different functions
// describe the neopixel settings as a function of time, so the program only has to update the state 
#include <TimerOne.h>
//#include <TimerOne.h>
#include <Adafruit_NeoPixel.h>
//#include <TimerOne.h> // allows for callbacks to the timer
// error compiling this header

#define BUTTON_PIN   2    // Digital IO pin connected to the button.  This will be
                          // driven with a pull-up resistor so the switch should
                          // pull the pin to ground momentarily.  On a high -> low
                          // transition the button press logic will execute.
#define DEFAULT_PIN 4 // default button signal
#define PIXEL_PIN    6    // Digital IO pin connected to the NeoPixels.
#define TRANS_PIN    8 // send a digital write signal to the transistor    
#define PIXEL_COUNT 12
//const int led_pin = 6;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

#define TIMER_US   50000                //every 50 microseconds timer will go off this is the call back
#define TICK_COUNTS_1S         20       //10     // 2S worth of timer ticks (each tick 50 microseconds)
#define TICK_COUNTS_DISPLAY_TIMER  1
#define TICK_COUNTS_DEBOUNCE   3 // healthy 200 microsecond debounce

volatile long tick_count_1_seconds = TICK_COUNTS_1S;          // Counter for intermediate things, fires every second
volatile long tick_display_update_timer = TICK_COUNTS_DISPLAY_TIMER;
volatile long tick_debounce = TICK_COUNTS_DEBOUNCE;
volatile boolean Flag_1S_Timer_Expired = false;
volatile boolean Display_Update_Timer_Expired = false;
volatile boolean pressed = false; // button held down

enum FSM_NuggetDisplayLevel
{
   eDISPLAY_PowerOn = 0,
   eDISPLAY_MakingIce,
   eDISPLAY_FullIce,
   eDISPLAY_NoIce,
   eDISPLAY_NeedWater,
   eDISPLAY_CleanCycleStart,
   eDISPLAY_CleanCycle,
   eDISPLAY_Error
};
 
enum FSM_NuggetDisplayLevel_PowerOn
{
  ePowerOn_Init = 0,
  ePowerOn_Flash,
  ePowerOn_End
};

enum FSM_NuggetDisplayLevel_MakingIce
{
  eMakingIce_Init = 0,
  eMakingIce_Flash,
  eMakingIce_End
};

enum FSM_NuggetDisplayLevel_FullIce
{
  eFullIce_Init = 0,
  eFullIce_Flash,
  eFullIce_End
};

enum FSM_NuggetDisplayLevel_NoIce
{
  eNoIce_Init =0,
  eNoIce_Flash,
  eNoIce_End
};

enum FSM_NuggetDisplayLevel_NeedsWater
{
  eNeedsWater_Init =0,
  eNeedsWater_Flash,
  eNeedsWater_End
};

enum FSM_NuggetDisplayLevel_CleanCycleStart
{
  eCleanCycleStart_Init =0,
  eCleanCycleStart_Flash,
  eCleanCycleStart_End
};

enum FSM_NuggetDisplayLevel_CleanCycle
{
  eCleanCycle_Init =0,
  eCleanCycle_Flash,
  eCleanCycle_End
};

enum FSM_NuggetDisplayLevel_CleanCyclePulse
{
  eCleanCyclePulse_Init =0,
  eCleanCyclePulse_Flash,
  eCleanCyclePulse_End
};

enum FSM_NuggetDisplayLevel_Error
{
  eError_Init =0,
  eError_Flash,
  eError_End
};

static FSM_NuggetDisplayLevel           fsm_nuggetDisplayLevel          = eDISPLAY_PowerOn; // local instances of the enumertions
static FSM_NuggetDisplayLevel_PowerOn   fsm_nuggetDisplayLevel_PowerOn  = ePowerOn_Init;
static FSM_NuggetDisplayLevel_MakingIce fsm_nuggetDisplayLevel_MakingIce = eMakingIce_Init;
static FSM_NuggetDisplayLevel_FullIce   fsm_nuggetDisplayLevel_FullIce = eFullIce_Init;
static FSM_NuggetDisplayLevel_NoIce     fsm_nuggetDisplayLevel_NoIce = eNoIce_Init;
static FSM_NuggetDisplayLevel_NeedsWater fsm_nuggetDisplayLevel_NeedsWater = eNeedsWater_Init;
static FSM_NuggetDisplayLevel_CleanCycleStart fsm_nuggetDisplayLevel_CleanCycleStart = eCleanCycleStart_Init;
static FSM_NuggetDisplayLevel_CleanCycle fsm_nuggetDisplayLevel_CleanCycle = eCleanCycle_Init;
static FSM_NuggetDisplayLevel_CleanCyclePulse fsm_nuggetDisplayLevel_CleanCyclePulse = eCleanCyclePulse_Init;
 
void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);                             // Set the pin as in input
  pinMode(TRANS_PIN, OUTPUT);             // set up pin to send signal to transistor
  pinMode(DEFAULT_PIN, INPUT_PULLUP); // just 
  strip.begin();                                                 // This initializes the NeoPixel library.
  strip.show(); 
  
  // Initialise the IO and ISR
  // Initialize all pixels to 'off'  
  //lerpWheel(240, 30, 50, 10, 0, 70, 4, 9, 400);
  digitalWrite(TRANS_PIN, HIGH); // just set leds on for now, might use interrupt to change when motor powers on
  Serial.begin(9600);
  // put your setup code here, to run once:
  Timer1.initialize(TIMER_US);                  // Initialise timer 1
  //Timer1.pwm(9, 512);
  Timer1.attachInterrupt( timerIsr );           // attach the ISR routine here
 
}
 
void loop() {
  // put your main code here, to run repeatedly:
 
  if (Flag_1S_Timer_Expired)
  { // executes once a second
    Flag_1S_Timer_Expired = false;
    tick_count_1_seconds = TICK_COUNTS_1S; 
  }
 
  if (Display_Update_Timer_Expired)
  {
     //reset the ISR counter
     Display_Update_Timer_Expired = false;
     tick_display_update_timer = TICK_COUNTS_DISPLAY_TIMER;
    
     fsm_Display();
  }
}
 
void timerIsr()
{
  volatile static boolean first_press = false;
  //volatile static boolean old_state = LOW;
  Serial.println(first_press); // I hope it resets, might have to make yet another global
  if (!(--tick_display_update_timer))
  {
    Display_Update_Timer_Expired = true;
  }
 
  if (!(--tick_count_1_seconds))
  {
    Flag_1S_Timer_Expired = true;
  }
  
  if (!(--tick_debounce)) { // reached debounce for button
    if (!first_press) { // has not been pressed
      first_press = digitalRead(BUTTON_PIN); // will move to else next debounce if the button is high
    } else {
      if (digitalRead(BUTTON_PIN)) {
        pressed = true;
      } else {
        pressed = false; // pressed resets when it is no longer held down, this is a global flag
      }
      first_press = false; // first_press has passed, will update pressed immediately
    }
      tick_debounce = TICK_COUNTS_DEBOUNCE; // reset counter
      
  }
}
 
void fsm_Display()
{
  //static volatile unsigned long ticks_in; // ticks_in each state, resets during a change
  switch(fsm_nuggetDisplayLevel)
  {
      case eDISPLAY_PowerOn:
         fsm_PowerOn();
      break;
  
      case eDISPLAY_MakingIce:
       //  fsm_MakingIce();
      break;
     
      case eDISPLAY_NeedWater:
      break;
     
      case eDISPLAY_CleanCycle:
      break;
     
      case eDISPLAY_Error:
      break;
  }
 
}
  
void fsm_PowerOn()
{
   const volatile unsigned long flash_length = 300; // ticks to go through flash state 
   volatile static unsigned long flash_counter;
   const volatile unsigned long end_length = 20;
   volatile static unsigned long end_counter; // not doing anything yet
   
   const volatile unsigned int start_l = 60; // greenness level 
   const volatile unsigned int end_l = 255;
   volatile unsigned int light_level;
   
   
   switch(fsm_nuggetDisplayLevel_PowerOn)
   {

   case ePowerOn_Init:
   flash_counter = flash_length; // start out the counter for the subsequent light changes
   end_counter = end_length;
   fsm_nuggetDisplayLevel_PowerOn = ePowerOn_Flash;
   break;
 
   case ePowerOn_Flash:
   light_level = parabola_level(start_l, end_l, flash_length, flash_counter); // send start and end light levels, the length of time and current timer
   //((start_l-end_l)/time)*(flash_counter*flash_counter) + ((end_l - start_l)/time)*(flash_counter) + start_l; what was time?
   for (int i = 0; i < PIXEL_COUNT; i++) {
     // snazzy new parabolic formula!
    
     strip.setPixelColor(i, strip.Color(0, light_level, 0));
     //0, start_light + ((flash_counter--)/flashLength)*(250 - start_light), 0));
   }
   break;
  
   case ePowerOn_End: // this one will be an actual state
   fsm_nuggetDisplayLevel = eDISPLAY_MakingIce;
   fsm_nuggetDisplayLevel_PowerOn = ePowerOn_Init;
   break;
   }
   flash_counter--; // decrement counter during the flash
   if (!flash_counter) {
     fsm_nuggetDisplayLevel_PowerOn = ePowerOn_End; // this one just terminates after predifined time
   } 
}

void fsm_NoIce() {
  const volatile unsigned long cycle_length = 600; // 600 pulses to turn back and forth (this may need to be much longer
  volatile static unsigned long cycle_counter;
  const volatile unsigned int dot_lR = 0;
  const volatile unsigned int dot_lG = 0;
  const volatile unsigned int dot_lB = 255;
  const volatile unsigned int back_lR = 5;
  const volatile unsigned int back_lG = 3;
  const volatile unsigned int back_lB = 25;
  
  switch(fsm_nuggetDisplayLevel_NoIce) {

     case eNoIce_Init:
     cycle_counter = cycle_length;
     for (uint16_t iw= 0; iw <= 11; iw++) { 
                strip.setPixelColor(iw, strip.Color(100, 100, 100)); // set top half color, white
     }
     fsm_nuggetDisplayLevel_NoIce = eNoIce_Flash;
     break;
     
     case eNoIce_Flash:
     if (cycle_counter < cycle_length/2) { // second half of count down
       lerpWheel(dot_lR, dot_lG, dot_lB, back_lR, back_lG, back_lB, 8, 3, cycle_counter, cycle_length/2); // slosh fro, half a cycle
     } else { // first half of count down
       lerpWheel(dot_lR, dot_lG, dot_lB, back_lR, back_lG, back_lB, 3, 8, cycle_counter-cycle_length/2, cycle_length/2);
     }
     break;
     
     case eNoIce_End:
     fsm_nuggetDisplayLevel_NoIce = eNoIce_Init; // need some way to actually break out of this
     break;     
     
     cycle_counter--;
     if (cycle_counter == 0) fsm_nuggetDisplayLevel_NoIce = eNoIce_End;
  } // end switch case
}

void fsm_NeedsWater() {
  const volatile unsigned long cycle_length = 600; // 600 pulses to turn back and forth (this may need to be much longer
  volatile static unsigned long cycle_counter;
  const volatile unsigned int dot_lR = 0;
  const volatile unsigned int dot_lG = 0;
  const volatile unsigned int dot_lB = 255;
  const volatile unsigned int back_lR = 5;
  const volatile unsigned int back_lG = 3;
  const volatile unsigned int back_lB = 25;
  
  switch(fsm_nuggetDisplayLevel_NeedsWater) {

     case eNeedsWater_Init:
     cycle_counter = cycle_length;
     for (uint16_t iw= 0; iw <= 11; iw++) { 
                strip.setPixelColor(iw, strip.Color(0, 0, 0)); // set top half color, off
     }
     fsm_nuggetDisplayLevel_NeedsWater = eNeedsWater_Flash;
     break;
     
     case eNeedsWater_Flash:
     if (cycle_counter < cycle_length/2) { // second half of count down
       lerpWheel(dot_lR, dot_lG, dot_lB, back_lR, back_lG, back_lB, 8, 3, cycle_counter, cycle_length/2); // slosh fro, half a cycle
     } else { // first half of count down
       lerpWheel(dot_lR, dot_lG, dot_lB, back_lR, back_lG, back_lB, 3, 8, cycle_counter-cycle_length/2, cycle_length/2);
     }
     if (cycle_counter == 0) fsm_nuggetDisplayLevel_NeedsWater = eNeedsWater_End;
     break;
     
     case eNeedsWater_End:
       fsm_nuggetDisplayLevel_NeedsWater = eNeedsWater_Init; // need some way to actually break out of this
     break;     
     
     cycle_counter--;
  } // end switch case
}

void fsm_FullIce() {
    const volatile unsigned int lR_start = 20;
    const volatile unsigned int lG_start = 20;
    const volatile unsigned int lB_start = 20;
    const volatile unsigned int lR_end = 115;
    const volatile unsigned int lG_end = 115;
    const volatile unsigned int lB_end = 115; // pulses low to mid white
    const volatile unsigned long pulse_length = 500;
    volatile unsigned int lR, lG, lB;

    static volatile unsigned long pulse_counter;
    
    switch (fsm_nuggetDisplayLevel_FullIce) {
      
      case eFullIce_Init:
        pulse_counter = pulse_length;
        fsm_nuggetDisplayLevel_FullIce = eFullIce_Flash;
      break;
      
      case eFullIce_Flash:
        lR = parabola_level(lR_start, lR_end, pulse_length, pulse_counter);
        lG = parabola_level(lG_start, lG_end, pulse_length, pulse_counter);
        lB = parabola_level(lB_start, lB_end, pulse_length, pulse_counter);
        for (int iled = 0; iled < PIXEL_COUNT; iled++) {
           strip.setPixelColor(iled, strip.Color(lR, lG, lB));
        }
        pulse_counter--;
        if (pulse_counter == 0) fsm_nuggetDisplayLevel_FullIce = eFullIce_End;
      break;
      
      case eFullIce_End:
        fsm_nuggetDisplayLevel_FullIce = eFullIce_Init; // end state not needed here, perhaps
      break;  
    }
}

void fsm_MakingIce() {
  volatile const unsigned long make_length = 600; // probably will not be a timer later on
  volatile static unsigned long make_counter;
  volatile const unsigned int led_interval = make_length/6; // time interval between each pixel
  volatile const unsigned int frostR = 100;
  volatile const unsigned int frostG = 100;
  volatile const unsigned int frostB = 100; // the white color that frosts over the background
  volatile const unsigned int backR = 0;
  volatile const unsigned int backG = 0;
  volatile const unsigned int backB = 255;
  
   switch (fsm_nuggetDisplayLevel_MakingIce) {
     
     case eMakingIce_Init:
     make_counter = make_length;
     fsm_nuggetDisplayLevel_MakingIce = eMakingIce_Flash;
     break;
     
     case eMakingIce_Flash:
     for (int ir = 0; ir <= 5; ir++) { // loops through right side
       if (make_counter < (5 - ir)*led_interval) {// past the current pixel, in the blue
          strip.setPixelColor(ir, strip.Color(backR, backG, backB));
       } else if (make_counter > (5 - ir + 1)*led_interval) {// before the current pixel, in white // need to check this on paper
          strip.setPixelColor(ir, strip.Color(frostR, frostG, frostB));
       } else {
          lerpPixel(backR, backG, backB, frostR, frostG, frostB, ir, led_interval, (make_counter%led_interval)); // lerp space between intervals the two colors
       }
     }
     for (int ir = 11; ir >= 6; ir++) { // loops through right side
       if (make_counter > (ir - 6)*led_interval) {// index is past the current pixel, in the blue
          strip.setPixelColor(ir, strip.Color(backR, backG, backB));
       } else if (make_counter < (ir - 6 + 1)*led_interval) {// index before the current pixel, in white 
          strip.setPixelColor(ir, strip.Color(frostR, frostG, frostB));
       } else {
          lerpPixel(backR, backG, backB, frostR, frostG, frostB, ir, led_interval, (make_counter%led_interval)); // lerp space between intervals the two colors
       }
     }
     if (make_counter == 0) fsm_nuggetDisplayLevel_MakingIce = eMakingIce_End;
     break;
     case eMakingIce_End:
       fsm_nuggetDisplayLevel_MakingIce = eMakingIce_Init;
     break;
   } 
}

void fsmCleaningCycleStart() {
  volatile const unsigned long start_length = 20; // precisely 1 second
  volatile static unsigned long start_counter;
  static unsigned int loop_count = 0; // count number of loops
  volatile const int dotR = 125;
  volatile const int dotG = 125; 
  volatile const int dotB = 0; 
  
  switch(fsm_nuggetDisplayLevel_CleanCycleStart) {
   
    case eCleanCycleStart_Init:
    start_counter = start_length;
    break;
    
    case eCleanCycleStart_Flash:
      lerpWheel(dotR, dotG, dotB, 0, 0, 0, 11, 0, start_length, start_counter);
      if (start_counter == 0) fsm_nuggetDisplayLevel_CleanCycleStart = eCleanCycleStart_End;
    break;
    
    case eCleanCycleStart_End:
      loop_count++;
      if (loop_count < 3)  {
        fsm_nuggetDisplayLevel_CleanCycleStart = eCleanCycleStart_Init;
      } else {
        fsm_nuggetDisplayLevel = eDISPLAY_CleanCycle;
      }
    break;
  }
}

// TO DO: 
void fsmCleaningCycle() {
}

void fsmCleaningCyclePulse() { // pulses after completion
  volatile unsigned int ystart = 40; // initial yellowness
  volatile unsigned int yend = 210; // end level
  volatile unsigned int yellow;
  volatile const unsigned long pulseTime = 80;
  volatile static unsigned long pulseCounter;
  switch (fsm_nuggetDisplayLevel_CleanCyclePulse) {
    
    case eCleanCyclePulse_Init:
      pulseCounter = pulseTime;
      fsm_nuggetDisplayLevel_CleanCyclePulse = eCleanCyclePulse_Flash;
    break;
    
    case eCleanCyclePulse_Flash:
      yellow = parabola_level(ystart, yend, pulseCounter--, pulseTime);
      for (int iled = 0; iled < PIXEL_COUNT; iled++) {
        strip.setPixelColor(iled, strip.Color(yellow, yellow, 0));
      }
      if (pulseCounter == 0) fsm_nuggetDisplayLevel_CleanCyclePulse = eCleanCyclePulse_End;
    break;
    
    case eCleanCyclePulse_End:
      fsm_nuggetDisplayLevel_CleanCyclePulse = eCleanCyclePulse_Init;
      fsm_nuggetDisplayLevel = eDISPLAY_MakingIce; // go back to normal function?
    break;

  } // end switch
}

void lerpPixel(int Ro, int Go, int Bo, int Re, int Ge, int Be, int iled, long time, long timer) { // initial / end colors, pixel index, timers for placement in range
  int Rled = Re + (Ro - Re) * (timer/time); // moves from Ro to Re, adding more of the negative interval between at the beginning then lowering that
  int Gled = Ge + (Go - Ge) * (timer/time);
  int Bled = Be + (Bo - Be) * (timer/time);
  strip.setPixelColor(iled, strip.Color(Rled, Gled, Bled));
}

void lerpWheel(int Rdot, int Gdot, int Bdot, int Rback, int Gback, int Bback, int istart, int iend, long time, long timer) {
  // time is the length of the whole clock cycle, time progresses down from the clock-cycle length. 
  unsigned const long int led_interval = time/(abs(iend-istart)); // no need to be constant really
  int di= abs(iend - istart)/(iend - istart); // determines in which direction indices proceed
  
  int current_pixel = istart + di*timer/led_interval; // index of current pixel (in range from 0 to iend - istart, will add istart to this), either forwards or backwards from istart
  //double led_progress = (timer - led_interval*current_pixel)/led_interval; // progress (from 0 to 1) through the current pixel (big data_type!!!, could be better)
  // this only works because current_pixel has been truncated, this essentially finds out how much it truncated. Is there a better way?
  double led_progress = (double)timer/(double)led_interval - current_pixel; // how far is it from the next current pixel value (0 to 1)
  int other_pixel; // index of the other lit pixel
  
  if (led_progress < 0.5) {
    other_pixel = current_pixel - di; // other pixel is the last one in this chain, still moving down
  } else {
    other_pixel = current_pixel + di; // halfway mark, next pixel going up
  }
    
  unsigned int Rc, Gc, Bc; // timer currently occupies this pixel's range
  unsigned int Ro, Go, Bo; // other pixel (in front or behind)
  //int dr = abs(Rdot - Rback)/(Rdot - Rback); // if change is negative or positive (not needed)
  
  for (int iled = 0; iled < abs(iend - istart); iled++) {
     if (iled != current_pixel && iled != other_pixel) {
         strip.setPixelColor(iled, strip.Color(Rback, Gback, Bback)); // all other pixels in the background
     }
  }
  if (led_progress < 0.5) {
      Rc = Rback + (Rdot - Rback)*(led_progress + .5); // .5 to 1 times the range between R values (could be negative, so dips down from background)
      Gc = Gback + (Gdot - Gback)*(led_progress + .5);
      Bc = Bback + (Bdot - Bback)*(led_progress + .5);
      Ro = Rback + (Rdot - Rback)*(.5 - led_progress); // background transitions from .5 to about 0
      Go = Gback + (Gdot - Gback)*(.5 - led_progress);
      Bo = Bback + (Bdot - Bback)*(.5 - led_progress);
  } else {
      Rc = Rback + (Rdot - Rback)*(1.5 - led_progress); // 1 to .5 level transition
      Gc = Gback + (Gdot - Gback)*(1.5 - led_progress);
      Bc = Bback + (Bdot - Bback)*(1.5 - led_progress);
      Ro = Rback + (Rdot - Rback)*(led_progress - .5); // 0 to .5 transition
      Go = Gback + (Gdot - Gback)*(led_progress - .5);
      Bo = Gback + (Bdot - Bback)*(led_progress - .5);
  }
  
  strip.setPixelColor(current_pixel, strip.Color(Rc, Gc, Bc));
  strip.setPixelColor(other_pixel, strip.Color(Ro, Go, Bo));
  
}

int parabola_level(int start_l, int end_l, long time, long timer) { // parabolic curve from start_l to end_l light levels, tim is full time it takes, 
  return ((start_l-end_l)/time)*(timer*timer) + ((end_l - start_l)/time)*(timer) + start_l;
}
    
