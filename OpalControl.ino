#include <TimerOne.h>                           // Header file for TimerOne library
//#include <avr/pgmspace.h>
 
#define TIMER_US               100000 // 100mS set timer duration in microseconds 
#define TICK_COUNTS_1S         1      //10     // 2S worth of timer ticks
#define TICK_COUNTS_2_5S       25    //2.5 seconds
#define TICK_COUNTS_5S         50     // 2S worth of timer ticks
#define TICK_COUNTS_60S        600
#define MAX_OVERFILL_TIME      80     // max time to fill the reservoir is 8 seconds, error if takes longer
#define MAX_EMPTY_TIME         80
#define MAX_DELAEYSTART_TIME   300    // 30 seconds to wait for delay start
#define MAX_RECOVERYWAIT_TIME  3600000    //  1 hourr to wait for recovery wait, if we wait more than this something went wrong
#define RECOVERY_TEMP          5000
#define RUN_AUGER_TEMP         4000  //temp at which the auger will turn on.  no need turning it on before the mold body is pulled down
#define NUGGET_RECOERY_RPM     200
#define NUGGET_LOW_RPM         1000
#define MAX_TIME_OFF_LOW_RPM   450    //turns off cooling system for 45 seconds
#define ICE_SENSOR_HIGH_LIMIT  250
#define ICE_SENSOR_LOW_LIMIT   750


//PROGMEM 
static short tempLookUp[] = { -6983, -6953, -6922, -6892, -6862, -6831, -6801, -6770, -6740, -6709, -6679, -6648, -6617, -6586, -6555, -6524, -6493, -6462, -6431, -6400, -6369, -6338, -6306, -6275, -6244, -6212, -6181, -6150, -6118, -6087, -6055, -6024, -5992, -5961, -5929, -5898, -5866, -5835, -5803, -5772, -5740, -5708, -5677, -5645, -5614, -5582, -5550, -5519, -5487, -5456, -5424, -5393, -5361, -5330, -5298, -5267, -5235, -5204, -5172, -5141, -5110, -5078, -5047, -5016, -4984, -4953, -4922, -4891, -4859, -4828, -4797, -4766, -4735, -4704, -4673, -4642, -4611, -4581, -4550, -4519, -4488, -4458, -4427, -4396, -4366, -4335, -4305, -4274, -4244, -4214, -4184, -4153, -4123, -4093, -4063, -4033, -4003, -3973, -3943, -3914, -3884, -3854, -3825, -3795, -3766, -3736, -3707, -3677, -3648, -3619, -3590, -3561, -3532, -3503, -3474, -3445, -3417, -3388, -3359, -3331, -3302, -3274, -3245, -3217, -3189, -3161, -3133, -3105, -3077, -3049, -3021, -2993, -2966, -2938, -2911, -2883, -2856, -2829, -2801, -2774, -2747, -2720, -2693, -2666, -2640, -2613, -2586, -2560, -2533, -2507, -2480, -2454, -2428, -2402, -2376, -2350, -2324, -2298, -2272, -2246, -2221, -2195, -2170, -2144, -2119, -2094, -2069, -2044, -2019, -1994, -1969, -1944, -1919, -1895, -1870, -1845, -1821, -1797, -1772, -1748, -1724, -1700, -1676, -1652, -1628, -1604, -1581, -1557, -1534, -1510, -1487, -1463, -1440, -1417, -1394, -1371, -1348, -1325, -1302, -1279, -1256, -1234, -1211, -1189, -1166, -1144, -1122, -1099, -1077, -1055, -1033, -1011, -989, -968, -946, -924, -903, -881, -859, -838, -817, -795, -774, -753, -732, -711, -690, -669, -648, -627, -607, -586, -565, -545, -524, -504, -484, -463, -443, -423, -403, -383, -363, -343, -323, -303, -283, -264, -244, -224, -205, -185, -166, -147, -127, -108, -89, -69, -50, -31, -12, 7, 26, 44, 63, 82, 101, 119, 138, 157, 175, 194, 212, 231, 249, 267, 285, 304, 322, 340, 358, 376, 394, 412, 430, 448, 466, 483, 501, 519, 537, 554, 572, 589, 607, 624, 642, 659, 677, 694, 711, 728, 746, 763, 780, 797, 814, 831, 848, 865, 882, 899, 916, 933, 950, 967, 984, 1000, 1017, 1034, 1050, 1067, 1084, 1100, 1117, 1134, 1150, 1167, 1183, 1200, 1216, 1232, 1249, 1265, 1281, 1298, 1314, 1330, 1347, 1363, 1379, 1395, 1412, 1428, 1444, 1460, 1476, 1492, 1508, 1524, 1541, 1557, 1573, 1589, 1605, 1621, 1637, 1653, 1669, 1684, 1700, 1716, 1732, 1748, 1764, 1780, 1796, 1812, 1827, 1843, 1859, 1875, 1891, 1907, 1922, 1938, 1954, 1970, 1985, 2001, 2017, 2033, 2049, 2064, 2080, 2096, 2111, 2127, 2143, 2159, 2174, 2190, 2206, 2222, 2237, 2253, 2269, 2284, 2300, 2316, 2332, 2347, 2363, 2379, 2395, 2410, 2426, 2442, 2457, 2473, 2489, 2505, 2520, 2536, 2552, 2568, 2584, 2599, 2615, 2631, 2647, 2662, 2678, 2694, 2710, 2726, 2742, 2757, 2773, 2789, 2805, 2821, 2837, 2853, 2868, 2884, 2900, 2916, 2932, 2948, 2964, 2980, 2996, 3012, 3028, 3044, 3060, 3076, 3092, 3108, 3124, 3140, 3156, 3172, 3188, 3204, 3220, 3237, 3253, 3269, 3285, 3301, 3317, 3334, 3350, 3366, 3382, 3399, 3415, 3431, 3448, 3464, 3480, 3496, 3513, 3529, 3546, 3562, 3578, 3595, 3611, 3628, 3644, 3661, 3677, 3694, 3710, 3727, 3743, 3760, 3777, 3793, 3810, 3827, 3843, 3860, 3877, 3893, 3910, 3927, 3943, 3960, 3977, 3994, 4011, 4028, 4044, 4061, 4078, 4095, 4112, 4129, 4146, 4163, 4180, 4197, 4214, 4231, 4248, 4265, 4282, 4299, 4316, 4334, 4351, 4368, 4385, 4402, 4420, 4437, 4454, 4471, 4489, 4506, 4523, 4541, 4558, 4575, 4593, 4610, 4628, 4645, 4663, 4680, 4698, 4715, 4733, 4750, 4768, 4785, 4803, 4821, 4838, 4856, 4874, 4891, 4909, 4927, 4945, 4962, 4980, 4998, 5016, 5034, 5051, 5069, 5087, 5105, 5123, 5141, 5159, 5177, 5195, 5213, 5231, 5249, 5267, 5285, 5303, 5321, 5340, 5358, 5376, 5394, 5412, 5430, 5449, 5467, 5485, 5504, 5522, 5540, 5559, 5577, 5595, 5614, 5632, 5651, 5669, 5687, 5706, 5724, 5743, 5762, 5780, 5799, 5817, 5836, 5855, 5873, 5892, 5911, 5929, 5948, 5967, 5986, 6004, 6023, 6042, 6061, 6080, 6099, 6117, 6136, 6155, 6174, 6193, 6212, 6231, 6250, 6269, 6288, 6308, 6327, 6346, 6365, 6384, 6403, 6423, 6442, 6461, 6480, 6500, 6519, 6538, 6558, 6577, 6597, 6616, 6635, 6655, 6674, 6694, 6714, 6733, 6753, 6772, 6792, 6812, 6831, 6851, 6871, 6891, 6910, 6930, 6950, 6970, 6990, 7010, 7030, 7050, 7070, 7090, 7110, 7130, 7150, 7170, 7191, 7211, 7231, 7251, 7272, 7292, 7312, 7333, 7353, 7374, 7394, 7415, 7435, 7456, 7477, 7497, 7518, 7539, 7560, 7581, 7601, 7622, 7643, 7664, 7685, 7706, 7727, 7749, 7770, 7791, 7812, 7834, 7855, 7876, 7898, 7919, 7941, 7963, 7984, 8006, 8028, 8049, 8071, 8093, 8115, 8137, 8159, 8181, 8203, 8226, 8248, 8270, 8292, 8315, 8337, 8360, 8383, 8405, 8428, 8451, 8474, 8497, 8520, 8543, 8566, 8589, 8612, 8635, 8659, 8682, 8706, 8729, 8753, 8777, 8801, 8825, 8849, 8873, 8897, 8921, 8945, 8970, 8994, 9019, 9043, 9068, 9093, 9118, 9143, 9168, 9193, 9218, 9244, 9269, 9295, 9321, 9346, 9372, 9398, 9424, 9450, 9477, 9503, 9530, 9556, 9583, 9610, 9637, 9664, 9691, 9718, 9746, 9773, 9801, 9829, 9857, 9885, 9913, 9942, 9970, 9999, 10027, 10056, 10085, 10114, 10144, 10173, 10203, 10232, 10262, 10292, 10323, 10353, 10384, 10414, 10445, 10476, 10507, 10539, 10570, 10602, 10634, 10666, 10698, 10730, 10763, 10795, 10828, 10861, 10895, 10928, 10962, 10996, 11030, 11064, 11099, 11133, 11168, 11203, 11239, 11274, 11310, 11346, 11382, 11419, 11455, 11492, 11529, 11566, 11604, 11642, 11680, 11718, 11757, 11796, 11835, 11874, 11913, 11953, 11993, 12034, 12074, 12115, 12156, 12198, 12239, 12281, 12324, 12366, 12409, 12452, 12496, 12540, 12584, 12628, 12673, 12718, 12763, 12809, 12855, 12901, 12947, 12994, 13042, 13089, 13137, 13185, 13234, 13283, 13333, 13382, 13432, 13483, 13534, 13585, 13636, 13688, 13741, 13793, 13847, 13900, 13954, 14008, 14063, 14118, 14174, 14230, 14286, 14343, 14400, 14458, 14516, 14575, 14634, 14693, 14753, 14814, 14874, 14936, 14998, 15060, 15123, 15186, 15250, 15314, 15379, 15444, 15510, 15576, 15643, 15710, 15778, 15846, 15915, 15985, 16055, 16125, 16196, 16268, 16340, 16413, 16486, 16560, 16635, 16710, 16786, 16862, 16939, 17017, 17095, 17174, 17253, 17333, 17414, 17495, 17577, 17660, 17743, 17827, 17912, 17997, 18083, 18170, 18257, 18346, 18434, 18524, 18614, 18705, 18797, 18889, 18983, 19076, 19171, 19267, 19363, 19460, 19558, 19656, 19756, 19856, 19957, 20058, 20161, 20264, 20369, 20474, 20580, 20687, 20794, 20903, 21012, 21122, 21233, 21346, 21458, 21572, 21687, 21803, 21919, 22037, 22155, 22275, 22395, 22517, 22639, 22762, 22887, 23012, 23138, 23265, 23394, 23523, 23654, 23785, 23917, 24051, 24186, 24321, 24458, 24596, 24735, 24875, 25016, 25158, 25302 };

//set up the input and output pins
//digital pins


//rpm feedback is interrrupt 0 which goes to pin 2
int pinRecoveryHeater  = 4;
int pinCompressor      = 5;
int pinDiverter        = 6;
int pinCondensorFan    = 9;  //verified
int pinWaterPump       = 8;
int pinSumpPump        = 7;//verified
int pinAugerMotor      = 10;
int pinWaterSwitchLo   = 12;  //verified
int pinWaterSwitchHi   = 11;








int pinSumpFullSwitch  = 14;

int pinSumpEmptySwitch = 13;



//analog pins
int pinMoldBodyTemp     = 0;
int pinIceSensor        = 1;

enum TEIceMakerOperationLevel
{
  eICEMAKER_NuggetDelayStart = 0,
  eICEMAKER_NuggetIceProduction,
  eICEMAKER_NuggetFillReservoir,
  eICEMAKER_NuggetRecovery,
  eICEMAKER_NuggetRecoveryWait,
  eICEMAKER_NuggetFault,
  eICEMAKER_NuggetStorage,
  eICEMAKER_NuggetEmptySump,
  eICEMAKER_NuggetOff
};

enum TEIceMakerFillOperationLevel 
{
  eICEMAKER_ReservoirInit,
  eICEMAKER_ReservoirWaitForEmpty,
  eICEMAKER_ReservoirFill,
  eICEMAKER_ReservoirWaitForFull,
  eICEMAKER_ReservoirFull,
  eICEMAKER_ReservoirFault
};

enum TEIceMakerEmptyOperationLevel 
{
  eICEMAKER_SumpInit,
  eICEMAKER_SumpWaitForFull,
  eICEMAKER_SumpFull,
  eICEMAKER_SumpWaitForEmpty,
  eICEMAKER_SumpEmpty,
  eICEMAKER_SumpFault,
};

enum TEIceMakerSumpFillOperationalLevel     
{
  eICEMAKER_SumpFillInit,
  eICEMAKER_SumpFillWaitForEmpty,
  eICEMAKER_SumpFillStartFilling,
  eICEMAKER_SumpFillWaitForFull,
  eICEMAKER_SumpFillFull,
  eICEMAKER_SumpFillFault
};

enum TEIceProductionLevel
{
  eICEMAKER_IceProductionInit = 0,
  eICEMAKER_Pulldown_Complete,  
  eICEMAKER_IceProductionMonitorRPMs,
  eICEMAKER_IceProductionRPMLow
};

enum TEDelayStartLevel
{
  eICEMAKER_DelayStartInit = 0,
  eICEMAKER_DelayStartCheckForTime
};

enum TERecoveryWaitLevel
{
  eICEMAKER_RecoveryWaitInit = 0,
  eICEMAKER_RecoveryWaitCheckForTemp
};

enum TEStorageLevel
{
  eICEMAKER_StorageInit = 0,
  eICEMAKER_StorageWait
};


static TEIceMakerOperationLevel   nuggetFSMLevel = eICEMAKER_NuggetDelayStart;   //tells us what state the icemaker is currently in
static TEIceMakerFillOperationLevel fillReservoirFSMLevel = eICEMAKER_ReservoirInit;
static TEIceMakerEmptyOperationLevel emptySumpFSMLevel = eICEMAKER_SumpInit;
static TEIceMakerSumpFillOperationalLevel  fillSumpFSMLevel = eICEMAKER_SumpFillInit;
static TEIceMakerOperationLevel   nuggetFSMPreviousLevel = eICEMAKER_NuggetDelayStart;  //this is the previous state.  if we have to go fill or empty sump we know what to return to
static TEDelayStartLevel          nuggetDelayStartLevel = eICEMAKER_DelayStartInit; //tells us what state the delaystart FSM is in
static TERecoveryWaitLevel        nuggetRecoveryWaitLevel = eICEMAKER_RecoveryWaitInit; //recovery wait FSM
static TEStorageLevel             nuggetStorageLevel = eICEMAKER_StorageInit; // storage FSM
static TEIceProductionLevel       nuggetProductionLevel = eICEMAKER_IceProductionInit; // ice production fsm


//static variables
static long mlongOverFillTimer = 0;                         // Counter to time for overfill of water reservoir
static long mlongEmptyTimer = 0;                            // Counter to time the sump pump runs
static long mlongDelayStartTimer = 0;                       // Counter to time the 30 seconds needed for delay start
static long mlongRecoveryWaitTimer = 0;                     // Recovery wait timer
static long mlongLowRPMTimer = 0;                           // Times turning off the cooling system so that temp can recover
static long coolingWaitTimer = 0;
static long mlongIceLevel = 0;
static boolean Motor_Is_On           = false;
static boolean Cooling_Is_On         = false;
static boolean Recovery_Heater_Is_On = false;
static boolean Chute_Heater_Is_On    = false;
static boolean Cooling_Available     = true;
static boolean Sump_Is_On            = false;

//volatile variables
volatile static int  lastRPMReading = 0;
volatile long seconds = 0;
volatile boolean Flag_1S_Timer_Expired = false;
volatile boolean Flag_2_5S_Timer_Expired = false;
volatile boolean Flag_5S_Timer_Expired = false;
volatile long tick_count_1_seconds = TICK_COUNTS_1S;          // Counter for intermediate things, fires every second
volatile long tick_count_2_5_seconds = TICK_COUNTS_2_5S;       // Counter for 2.5 seconds, for the RPM feedback
volatile long tick_count_5_seconds = TICK_COUNTS_5S;        // Counter for intermediate things, fires every second
volatile long tick_count_60_seconds = TICK_COUNTS_60S;        // Counter for long lived objects, every 60 seconds
volatile long  rpmCount = 250;                                 // counts the RPMs for feedback control

// ----------------------------------
//  setup() routine called first.
// ----------------------------------
void setup() 
{
  Serial.begin(9600);
  
  pinMode(pinRecoveryHeater,OUTPUT);
  pinMode(pinCompressor,OUTPUT);
  pinMode(pinDiverter,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(pinWaterPump,OUTPUT);
  pinMode(pinCondensorFan,OUTPUT);
  pinMode(pinAugerMotor,OUTPUT);
  pinMode(pinWaterSwitchLo,INPUT);
  pinMode(pinWaterSwitchLo,INPUT);
  pinMode(pinSumpEmptySwitch,INPUT);
 



  nuggetFSMLevel = eICEMAKER_NuggetDelayStart;
  nuggetDelayStartLevel = eICEMAKER_DelayStartInit;
  
  turnAugerMotorOff();
  Timer1.initialize(TIMER_US);                  // Initialise timer 1
  Timer1.attachInterrupt( timerIsr );           // attach the ISR routine here
  
  attachInterrupt(0, countRPM, RISING);               // interrupt 0 is actually pin 2
  
}
 
// ----------------------------------
//  loop() Runs continuously in a loop.
// ----------------------------------
void loop()
{   
 
 if (Flag_1S_Timer_Expired)
   {
   
     tick_count_1_seconds = TICK_COUNTS_1S;
     Flag_1S_Timer_Expired = false;
   }

   if (Flag_1S_Timer_Expired)
   {
     coolingWaitTimer++;
     if (coolingWaitTimer > 360)
       Cooling_Available = true;
       
     getIceLevel();
     runNuggetFSM();
     runFillReservoirFSM();
     //fillSumpFSM();
     //runEmptySumpFSM();
     tick_count_1_seconds = TICK_COUNTS_1S;
     Flag_1S_Timer_Expired = false;
   }
   
   if (Flag_2_5S_Timer_Expired)
   {
     tick_count_2_5_seconds = TICK_COUNTS_2_5S;
     Flag_2_5S_Timer_Expired = false;
     calculateRPM();
   }
   
   if (Flag_5S_Timer_Expired)
   {
     
     tick_count_5_seconds = TICK_COUNTS_5S;
     Flag_5S_Timer_Expired = false;
     
   }
   
}

void countRPM()
{
  rpmCount++;
}
 
// --------------------------
// timerIsr() 100 milli second interrupt ISR()
// Called every time the hardware timer 1 times out.
// --------------------------
void timerIsr()
{   
    if (!(--tick_count_1_seconds))                  // Timer fires every 1 seconds                     
    {
      //tick_count_1_seconds = TICK_COUNTS_1S;        // Reload
      Flag_1S_Timer_Expired = true;
    }
    
    
    if (!(--tick_count_2_5_seconds))
    {
      //tick_count_2_5_seconds = TICK_COUNTS_2_5S;
      Flag_2_5S_Timer_Expired = true;
    }
    
    if (!(--tick_count_5_seconds))                  // Timer fires every 5 seconds                     
    {
      tick_count_5_seconds = TICK_COUNTS_5S;        // Reload
      Flag_5S_Timer_Expired = true;
    }
}

void fillSumpFSM()
{
  switch(fillSumpFSMLevel)
 {
    case eICEMAKER_SumpFillInit:
      fillSumpFSMLevel = eICEMAKER_SumpFillWaitForEmpty;
    break;
    
    case eICEMAKER_SumpFillWaitForEmpty:
      if (isSumpEmpty())
        fillSumpFSMLevel = eICEMAKER_SumpFillStartFilling;
    break;
    
    case eICEMAKER_SumpFillStartFilling:
      turnWaterOn();
      fillSumpFSMLevel = eICEMAKER_SumpFillWaitForFull;
    break;
  
    case eICEMAKER_SumpFillWaitForFull:
      if (isSumpFull())
        fillSumpFSMLevel = eICEMAKER_SumpFillFull;
    break;
  
    case eICEMAKER_SumpFillFull:
      turnWaterOff();
      fillSumpFSMLevel = eICEMAKER_SumpFillWaitForEmpty;
    break;
  
    case eICEMAKER_SumpFillFault:
    break;
  } 
}

void runEmptySumpFSM()
{
  switch (emptySumpFSMLevel)
  {
    case eICEMAKER_SumpInit:
      emptySumpFSMLevel = eICEMAKER_SumpWaitForFull;
    break;
    
    case eICEMAKER_SumpWaitForFull:
      if (isSumpFull())
        emptySumpFSMLevel = eICEMAKER_SumpFull;
    break;
    
    case eICEMAKER_SumpFull:
      turnSumpOn();
      mlongEmptyTimer = 0;
      emptySumpFSMLevel = eICEMAKER_SumpWaitForEmpty;
      break;
     
    case eICEMAKER_SumpWaitForEmpty:
      if (isSumpEmpty())
      {
        emptySumpFSMLevel = eICEMAKER_SumpEmpty;
      }
      else if (mlongEmptyTimer >= MAX_EMPTY_TIME)  //time has exceeded, turn the sump pump and set error
      {
        //set the nugget FSM to Error
        change_state_NuggetFault();
        emptySumpFSMLevel = eICEMAKER_SumpFault;
      }
      else
      {
        mlongEmptyTimer++; 
      }
    break;
    
    case eICEMAKER_SumpEmpty:
      turnSumpOff();
      emptySumpFSMLevel = eICEMAKER_SumpWaitForFull;
    break;
    
    case eICEMAKER_SumpFault:
      turnSumpOff();
    break;
  }
}

void runFillReservoirFSM()
{
  
  //Serial.print("FSM ");Serial.print(fillReservoirFSMLevel);Serial.print(" Low Switch ");Serial.print(isReservoirEmpty());Serial.print(" Hi Switch ");Serial.println(isReservoirFull());
  switch (fillReservoirFSMLevel)
  {
    case eICEMAKER_ReservoirInit:
      fillReservoirFSMLevel = eICEMAKER_ReservoirWaitForEmpty;
    break;
    
    case eICEMAKER_ReservoirWaitForEmpty:
      if (isReservoirEmpty())
        fillReservoirFSMLevel = eICEMAKER_ReservoirFill;
    break;
    
    case eICEMAKER_ReservoirFill:
      turnWaterOn();
      //turnWaterOn();
      mlongOverFillTimer = 0;
      fillReservoirFSMLevel = eICEMAKER_ReservoirWaitForFull;
      break;
     
    case eICEMAKER_ReservoirWaitForFull:
      if (isReservoirFull())
      {
        fillReservoirFSMLevel = eICEMAKER_ReservoirFull;
      }
      else if (mlongOverFillTimer >= MAX_OVERFILL_TIME)  //time has exceeded, turn off water and set error
      {
        //set the nugget FSM to Error
        change_state_NuggetFault();
        fillReservoirFSMLevel = eICEMAKER_ReservoirFault;
      }
      else
      {
        mlongOverFillTimer++; 
      }
    break;
    
    case eICEMAKER_ReservoirFull:
      turnWaterOff();
      //turnSumpOff();
      fillReservoirFSMLevel = eICEMAKER_ReservoirWaitForEmpty;
    break;
    
    case eICEMAKER_ReservoirFault:
      turnWaterOff();
      //turnSumpOff();
    break;
  }
}

/***********************************************************************
*  runNuggetFSM
***********************************************************************/
void runNuggetFSM()
{
  
/*  if (isSumpFull())
    turnSumpOn();
    
  else if (isSumpEmpty())
    turnSumpOff();*/
  
  isIceLevelFull();
  nuggetFSMLevel = eICEMAKER_NuggetDelayStart;
  
  switch(nuggetFSMLevel)
  {
    case eICEMAKER_NuggetDelayStart:
      fsm_NuggetDelayStart();
    break;
    
    case eICEMAKER_NuggetIceProduction:
      fsm_NuggetIceProduction();
    break;
    
    case eICEMAKER_NuggetRecovery:
      fsm_NuggetRecovery();
    break;
    
    case eICEMAKER_NuggetRecoveryWait:
      fsm_NuggetRecoveryWait();
    break;
    
    case eICEMAKER_NuggetFault:
      fsm_NuggetFault();
    break;
    
    case eICEMAKER_NuggetStorage:
      fsm_NuggetStorage();
    break;
    
    case eICEMAKER_NuggetOff:
      fsm_NuggetOff();
    break;    
  }
}

/************************************************************************************************************************************************************************
*                                                                                                                                                                       *
*                Following functions change the state of the FSM at the next execution cycle                                                                            *            
*                                                                                                                                                                       *
************************************************************************************************************************************************************************/
/********************************************************************
*  change_state_DelayStart()
*  This function changes the state of the nugget FSM
********************************************************************/
void change_state_DelayStart()
{
   nuggetFSMLevel = eICEMAKER_NuggetDelayStart;
}

/********************************************************************
*  change_state_NuggetIceProduction()
*  This function changes the state of the nugget FSM
********************************************************************/
void change_state_NuggetIceProduction()
{
   nuggetFSMLevel = eICEMAKER_NuggetIceProduction;
}

/********************************************************************
*  change_state_NuggetFillReservoir()
*  This function changes the state of the nugget FSM
********************************************************************/
void change_state_NuggetFillReservoir()
{
   nuggetFSMLevel = eICEMAKER_NuggetFillReservoir;
}

/********************************************************************
*  change_state_NuggetRecovery()
*  This function changes the state of the nugget FSM
********************************************************************/
void change_state_NuggetRecovery()
{
   nuggetFSMLevel = eICEMAKER_NuggetRecovery;
}

/********************************************************************
*  change_state_NuggetRecoveryWait()
*  This function changes the state of the nugget FSM
********************************************************************/
void change_state_NuggetRecoveryWait()
{
   nuggetFSMLevel = eICEMAKER_NuggetRecoveryWait;
}

/********************************************************************
*  change_state_NuggetFault()
*  This function changes the state of the nugget FSM
********************************************************************/
void change_state_NuggetFault()
{
   nuggetFSMLevel = eICEMAKER_NuggetFault;
}

/********************************************************************
*  change_state_NuggetEmptySump()
*  This function changes the state of the nugget FSM
********************************************************************/
void change_state_NuggetEmptySump()
{
  nuggetFSMLevel = eICEMAKER_NuggetEmptySump;
}

/********************************************************************
*  change_state_NuggetStorage()
*  This function changes the state of the nugget FSM
********************************************************************/
void change_state_NuggetStorage()
{
   nuggetFSMLevel = eICEMAKER_NuggetStorage;
}
/********************************************************************
*  change_state_NuggetOff()
*  This function changes the state of the nugget FSM
********************************************************************/
void change_state_NuggetOff()
{
   nuggetFSMLevel = eICEMAKER_NuggetOff;
}
/************************************************************************************************************************************************************************
*                                                                                                                                                                       *
*               end of the functions that change the state at the next execution cycle                                                                                  *            
*                                                                                                                                                                       *
************************************************************************************************************************************************************************/


/************************************************************************************************************************************************************************
*                                                                                                                                                                       *
*                Following functions are the actual FSM state execution                                                                                                 *            
*                                                                                                                                                                       *
************************************************************************************************************************************************************************/

/********************************************************************
*  fsm_NuggetDelayStart()
*  Executes the Delay Start state
********************************************************************/
void fsm_NuggetDelayStart()
{
 
  switch(nuggetDelayStartLevel)
  {
    case eICEMAKER_DelayStartInit:
      //set the delay start timer
      mlongDelayStartTimer = 0;
      nuggetDelayStartLevel = eICEMAKER_DelayStartCheckForTime;
    break;
    
    case eICEMAKER_DelayStartCheckForTime:
      if (mlongDelayStartTimer >= MAX_DELAEYSTART_TIME)
      {
        nuggetProductionLevel = eICEMAKER_IceProductionInit;
        change_state_NuggetIceProduction();
      }
      else
      {
        mlongDelayStartTimer++;
      }
    break;
  }
}

/********************************************************************
*  ()
*  Executes the Delay Start state
********************************************************************/
void fsm_NuggetIceProduction()
{
  
  //if ice bucket is full, turn ice making off
  if (isIceLevelFull())
  {
    change_state_NuggetStorage();
  }
  //else run through the monitor RPM settings
  else
  {
    switch(nuggetProductionLevel)
    {
      case eICEMAKER_IceProductionInit:
        //turnChuteHeaterOn();
        if (Cooling_Available)  //we won't cycle the cooling on/off shorter than 5 minutes
        {
          turnCoolingOn();
          turnRecoveryHeaterOff();
          nuggetProductionLevel = eICEMAKER_Pulldown_Complete;
        }
      break;
      
      case eICEMAKER_Pulldown_Complete:
        if (getMoldBodyTemp() < RUN_AUGER_TEMP)
        {
          turnAugerMotorOn();
          nuggetProductionLevel = eICEMAKER_IceProductionMonitorRPMs;
        }
      break;     
      
      case eICEMAKER_IceProductionMonitorRPMs:

      break;
      
      case eICEMAKER_IceProductionRPMLow:

        
      break;
    } 
  }
}

/********************************************************************
*  fsm_NuggetRecovery()
*  This function executes the nugget recovery state fsm.  In this 
*   state the unit turns on the recovery heater, turns off the cooling
*  system and changes to recovery wait
********************************************************************/
void fsm_NuggetRecovery()
{

}

/********************************************************************
*  fsm_NuggetRecoveryWait()
*  Executes the reovery wait FSM.  Here we wait until the mold body
*  has reached its recovery temperature. If this does not occur
*  before a max timer than we set an error
********************************************************************/
void fsm_NuggetRecoveryWait()
{

}

/********************************************************************
*  fsm_NuggetFault()
*  Executes the Fault state.  Turn everything off.  No exit from this 
*  state, except to unplug/plug the unit
********************************************************************/
void fsm_NuggetFault()
{
  turnWaterOff();
  turnAugerMotorOff();
  turnAllHeatersOff();
  turnCoolingOff();
}

/********************************************************************
*  fsm_NuggetOff()
*  Executes the off state.  Turn everything off.  Exactly the same as
* the nugget fault FSM, except this one can be exited with a nugget on
********************************************************************/
void fsm_NuggetOff()
{
  turnWaterOff();
  turnAugerMotorOff();
  turnAllHeatersOff();
  turnCoolingOff();
}

/********************************************************************
*  fsm_NuggetStorage()
*  Executes the storage state.  Turn everything off and check to see 
*  if anything occurs that requires ice to be made again
********************************************************************/
void fsm_NuggetStorage()
{
  switch(nuggetStorageLevel)
  {
    case eICEMAKER_StorageInit:
      //turn ice making off
      turnAugerMotorOff();
      turnAllHeatersOff();
      turnCoolingOff();
      nuggetStorageLevel = eICEMAKER_StorageWait;
    break;
    
    case eICEMAKER_StorageWait:
      //check to see if the ice level is low, if so turn everything back on and make ice
      if (isIceLevelLow())
      {
        change_state_DelayStart();
        nuggetDelayStartLevel = eICEMAKER_DelayStartInit;
      }
    break;
  }
}
/************************************************************************************************************************************************************************
*                                                                                                                                                                       *
*               end of the FSM state execution function                                                                                                                 *
*                                                                                                                                                                       *
************************************************************************************************************************************************************************/

/************************************************************************************************************************************************************************
*                                                                                                                                                                       *
*                Following check the status of something or return a value                                                                                              *
*                                                                                                                                                                       *
************************************************************************************************************************************************************************/

/********************************************************************
*  isIceLevelLow()
*  This function checks to see if the ice level in the reservoir is 
*  below our limit and needs to be turned on
*  returns true if ice is low and false if ice is not low
********************************************************************/
bool isIceLevelLow()
{
  bool returnVal = false;
  
  if (mlongIceLevel < ICE_SENSOR_LOW_LIMIT)
  {
    returnVal = true;
  }
  
  //returnVal = true;
  return(returnVal);
}

/********************************************************************
*  isIceLevelFull()
*  This function checks to see if the ice level in the reservoir is 
*  below above the limit.  If so turn off the icemaker
********************************************************************/
bool isIceLevelFull()
{
  bool returnVal = false;
  
  if (mlongIceLevel > ICE_SENSOR_HIGH_LIMIT)
  {
    returnVal = true;
  }
  else{
    returnVal = false;
  }
  
  return(returnVal); 
}

/********************************************************************
*  isReservoirFull
*  checks the water level switch
*  TRUE   - if water reservoir is full
*  FALSE  - if water reservoir is not full
********************************************************************/
bool isReservoirFull()
{
   bool returnVal = false;
   
   if (digitalRead(pinWaterSwitchHi) == 0)
   {
     returnVal = true;
   }
   return(returnVal);
} 

/********************************************************************
*  isReservoirEmpty
*  checks the water level switch
*  TRUE   - if water reservoir is empty
*  FALSE  - if water reservoir is not empty
********************************************************************/
bool isReservoirEmpty()
{
  bool returnVal = false;
   
   if (digitalRead(pinWaterSwitchLo) == 0)
   {
     returnVal = true;
   }
   return(returnVal);
}

/***********************************************************************
*  every time through the loop it updates the ice level
*  it uses a waited average so that falling cubes do not 
*  trip the sensor to think it is full
***********************************************************************/
void getIceLevel()
{
  mlongIceLevel = 0.9*mlongIceLevel + 0.1*analogRead(pinIceSensor); 
}

int getMoldBodyTemp()
{
  int anaVal = analogRead(pinMoldBodyTemp);
  //Serial.print("analog thermistor reading: ");
  //Serial.println(anaVal);
  
  if (anaVal == 0)
    return -30000;
    
  else 
    return(tempLookUp[anaVal]);
    //return( pgm_read_word_near(tempLookUp + anaVal));*/
    
    
}

int getNuggetMotorRPM()
{
  return ((int)lastRPMReading);
}

bool isSumpFull()
{
  bool returnVal = false;
   
  if (digitalRead(pinSumpFullSwitch) == HIGH)
   {
     returnVal = true;
   }
   return(returnVal);
}

bool isSumpEmpty()
{
  bool returnVal = false;
   
  if (digitalRead(pinSumpEmptySwitch) == HIGH)
   {
     returnVal = true;
   }
   return(returnVal);
}
/************************************************************************************************************************************************************************
*                                                                                                                                                                       *
*                end of functions that check the state of something                                                                                                     *
*                                                                                                                                                                       *
************************************************************************************************************************************************************************/

/************************************************************************************************************************************************************************
*                                                                                                                                                                       *
*                start of functions that control something (eg, motor, heater, ...)                                                                                     *
*                                                                                                                                                                       *
************************************************************************************************************************************************************************/
/*******************************************************************
*  turnChuteHeaterOn()
*  turns the auger motor on
********************************************************************/
void turnChuteHeaterOn()
{
   //digitalWrite(pinChuteHeater, HIGH);
  
}
/*******************************************************************
*  turnChuteHeaterOff()
*  turns the auger motor off
********************************************************************/
void turnChuteHeaterOff()
{
  //digitalWrite(pinChuteHeater, LOW);
 
}

/********************************************************************
*  turnAugerMotorOn()
*  turns the auger motor on
********************************************************************/
void turnAugerMotorOn()
{
  lastRPMReading = 4000;
  digitalWrite(pinAugerMotor,LOW);
  Motor_Is_On = true;

}

/********************************************************************
*  turnAugerMotorOff()
*  turns the auger motor off
********************************************************************/
void turnAugerMotorOff()
{
  digitalWrite(pinAugerMotor,HIGH);
  Motor_Is_On = false;


}

/********************************************************************
*  turnAllHeatersOff()
*  turns both the recovery and the chute heater off
********************************************************************/
void turnAllHeatersOff()
{
  turnChuteHeaterOff();
  turnRecoveryHeaterOff();
  Recovery_Heater_Is_On  = false;
  Chute_Heater_Is_On = false;
  

}

/********************************************************************
*  turnCoolingOn()
*  turns the cooling system on
********************************************************************/
void turnCoolingOn()
{
  digitalWrite(pinCondensorFan, HIGH);
  digitalWrite(pinCompressor, HIGH);
  Cooling_Is_On  = true;

}

/********************************************************************
*  turnCoolingOff()
*  turns the cooling system off
********************************************************************/
void turnCoolingOff()
{
  digitalWrite(pinCondensorFan, LOW);
  digitalWrite(pinCompressor, LOW);
  
  startCoolingWait();  //wait 5 minutes before we will turn the cooling back on
  
  Cooling_Is_On = false;
  

}

/********************************************************************
*  turnRecoveryHeaterOff()
*  turns the recovery heater on
********************************************************************/
void turnRecoveryHeaterOn()
{
  digitalWrite(pinRecoveryHeater,HIGH);
  Recovery_Heater_Is_On  = true;
  
}

/********************************************************************
*  turnRecoveryHeaterOff()
*  turns the recovery heater off
********************************************************************/
void turnRecoveryHeaterOff()
{
  digitalWrite(pinRecoveryHeater,LOW);
  Recovery_Heater_Is_On  = false;

}

/********************************************************************
*  turnWaterOn()
*  turns on the water to fill the reservoir
********************************************************************/
void turnWaterOn()
{
   digitalWrite(pinSumpPump,HIGH);

    delay(3000);
    turnWaterOff();

}   

/********************************************************************
*  turnWaterOff()
*  turns on the water to fill the reservoir
********************************************************************/
void turnWaterOff()
{     
  
   digitalWrite(pinSumpPump,LOW);


}  

/********************************************************************
*  turnSumpOn()
*  turns on the sump pump on
********************************************************************/
void turnSumpOn()
{
  Sump_Is_On = true;
  digitalWrite(pinSumpPump,HIGH);

}

/********************************************************************
*  turnSumpOff()
*  turns on the sump pump off
********************************************************************/
void turnSumpOff()
{
   Sump_Is_On = false;
  digitalWrite(pinSumpPump,LOW);

}

/********************************************************************
*  calculateRPM()
*  every 5 seconds we will calcuate the new average motor RPM
********************************************************************/
void calculateRPM()
{
  
  lastRPMReading = (rpmCount*60.0/6.0)/(2.5);  //sampe every 2.5 secondes
  rpmCount = 0;

}

void startCoolingWait()
{
  coolingWaitTimer = 0;
  Cooling_Available = false;
}

/************************************************************************************************************************************************************************
*                                                                                                                                                                       *
*                end of functions that control something (eg, motor, heater, ...)                                                                                       *
*                                                                                                                                                                       *
************************************************************************************************************************************************************************/
