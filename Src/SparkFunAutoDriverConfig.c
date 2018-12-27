#include "SparkFunAutoDriver.h"

// Setup the SYNC/BUSY pin to be either SYNC or BUSY, and to a desired
//  ticks per step level.
void L6470_configSyncPin(uint8_t pinFunc, uint8_t syncSteps)
{
  // Only some of the bits in this register are of interest to us; we need to
  //  clear those bits. It happens that they are the upper four.
  uint8_t syncPinConfig = (uint8_t)getParam(STEP_MODE);
  syncPinConfig &= 0x0F;
  
  // Now, let's OR in the arguments. We're going to mask the incoming
  //  data to avoid touching any bits that aren't appropriate. See datasheet
  //  for more info about which bits we're touching.
  syncPinConfig |= ((pinFunc & 0x80) | (syncSteps & 0x70));
  
  // Now we should be able to send that uint8_t right back to the dSPIN- it
  //  won't corrupt the other bits, and the changes are made.
  setParam(STEP_MODE, (uint32_t)syncPinConfig);
}

// The dSPIN chip supports microstepping for a smoother ride. This function
//  provides an easy front end for changing the microstepping mode.
void L6470_configStepMode(uint8_t stepMode)
{

  // Only some of these bits are useful (the lower three). We'll extract the
  //  current contents, clear those three bits, then set them accordingly.
  uint8_t stepModeConfig = (uint8_t)getParam(STEP_MODE);
  stepModeConfig &= 0xF8;
  
  // Now we can OR in the new bit settings. Mask the argument so we don't
  //  accidentally the other bits, if the user sends us a non-legit value.
  stepModeConfig |= (stepMode&0x07);
  
  // Now push the change to the chip.
  setParam(STEP_MODE, (uint32_t)stepModeConfig);
}

uint8_t L6470_getStepMode() {
  return (uint8_t)(getParam(STEP_MODE) & 0x07);
}

// This is the maximum speed the dSPIN will attempt to produce.
void L6470_setMaxSpeed(float stepsPerSecond)
{
  // We need to convert the floating point stepsPerSecond into a value that
  //  the dSPIN can understand. Fortunately, we have a function to do that.
  uint32_t integerSpeed = L6470_maxSpdCalc(stepsPerSecond);
  
  // Now, we can set that paramter.
  setParam(MAX_SPEED, integerSpeed);
}


float L6470_getMaxSpeed()
{
  return L6470_maxSpdParse(getParam(MAX_SPEED));
}

// Set the minimum speed allowable in the system. This is the speed a motion
//  starts with; it will then ramp up to the designated speed or the max
//  speed, using the acceleration profile.
void L6470_setMinSpeed(float stepsPerSecond)
{
  // We need to convert the floating point stepsPerSecond into a value that
  //  the dSPIN can understand. Fortunately, we have a function to do that.
  uint32_t integerSpeed = L6470_minSpdCalc(stepsPerSecond);
  
  // MIN_SPEED also contains the LSPD_OPT flag, so we need to protect that.
  uint32_t temp = getParam(MIN_SPEED) & 0x00001000;
  
  // Now, we can set that paramter.
  setParam(MIN_SPEED, integerSpeed | temp);
}

float L6470_getMinSpeed()
{
  return L6470_minSpdParse(getParam(MIN_SPEED));
}

// Above this threshold, the dSPIN will cease microstepping and go to full-step
//  mode. 
void L6470_setFullSpeed(float stepsPerSecond)
{
  uint32_t integerSpeed = L6470_FSCalc(stepsPerSecond);
  setParam(FS_SPD, integerSpeed);
}

float L6470_getFullSpeed()
{
  return L6470_FSParse(getParam(FS_SPD));
}

// Set the acceleration rate, in steps per second per second. This value is
//  converted to a dSPIN friendly value. Any value larger than 29802 will
//  disable acceleration, putting the chip in "infinite" acceleration mode.
void L6470_setAcc(float stepsPerSecondPerSecond)
{
  uint32_t integerAcc = L6470_accCalc(stepsPerSecondPerSecond);
  setParam(ACC, integerAcc);
}

float L6470_getAcc()
{
  return L6470_accParse(getParam(ACC));
}

// Same rules as setAcc().
void L6470_setDec(float stepsPerSecondPerSecond)
{
  uint32_t integerDec = L6470_decCalc(stepsPerSecondPerSecond);
  setParam(DECEL, integerDec);
}

float L6470_getDec()
{
  return L6470_accParse(getParam(DECEL));
}

void L6470_setOCThreshold(uint8_t threshold)
{
  setParam(OCD_TH, 0x0F & threshold);
}

uint8_t L6470_getOCThreshold()
{
  return (uint8_t) (getParam(OCD_TH) & 0xF);
}

// The next few functions are all breakouts for individual options within the
//  single register CONFIG. We'll read CONFIG, blank some bits, then OR in the
//  new value.

// This is a multiplier/divider setup for the PWM frequency when microstepping.
//  Divisors of 1-7 are available; multipliers of .625-2 are available. See
//  datasheet for more details; it's not clear what the frequency being
//  multiplied/divided here is, but it is clearly *not* the actual clock freq.
void L6470_setPWMFreq(int16_t divisor, int16_t multiplier)
{
  uint32_t configVal = getParam(CONFIG);
  
  // The divisor is set by config 15:13, so mask 0xE000 to clear them.
  configVal &= ~(0xE000);
  // The multiplier is set by config 12:10; mask is 0x1C00
  configVal &= ~(0x1C00);
  // Now we can OR in the masked-out versions of the values passed in.
  configVal |= ((0xE000&divisor)|(0x1C00&multiplier));
  setParam(CONFIG, configVal);
}

int16_t L6470_getPWMFreqDivisor()
{
  return (int16_t) (getParam(CONFIG) & 0xE000);
}

int16_t L6470_getPWMFreqMultiplier()
{
  return (int16_t) (getParam(CONFIG) & 0x1C00);
}

// Slew rate of the output in V/us. Can be 180, 290, or 530.
void L6470_setSlewRate(int16_t slewRate)
{
  uint32_t configVal = getParam(CONFIG);
  
  // These bits live in CONFIG 9:8, so the mask is 0x0300.
  configVal &= ~(0x0300);
  //Now, OR in the masked incoming value.
  configVal |= (0x0300&slewRate);
  setParam(CONFIG, configVal);
}

int16_t L6470_getSlewRate()
{
  return (int16_t) (getParam(CONFIG) & 0x0300);
}

// Single bit- do we shutdown the drivers on overcurrent or not?
void L6470_setOCShutdown(int16_t OCShutdown)
{
  uint32_t configVal = getParam(CONFIG);
  // This bit is CONFIG 7, mask is 0x0080
  configVal &= ~(0x0080);
  //Now, OR in the masked incoming value.
  configVal |= (0x0080&OCShutdown);
  setParam(CONFIG, configVal);
}

int16_t L6470_getOCShutdown()
{
  return (int16_t) (getParam(CONFIG) & 0x0080);
}

// Enable motor voltage compensation? Not at all straightforward- check out
//  p34 of the datasheet.
void L6470_setVoltageComp(int16_t vsCompMode)
{
  uint32_t configVal = getParam(CONFIG);
  // This bit is CONFIG 5, mask is 0x0020
  configVal &= ~(0x0020);
  //Now, OR in the masked incoming value.
  configVal |= (0x0020&vsCompMode);
  setParam(CONFIG, configVal);
}

int16_t L6470_getVoltageComp()
{
  return (int16_t) (getParam(CONFIG) & 0x0020);
}

// The switch input can either hard-stop the driver _or_ activate an interrupt.
//  This bit allows you to select what it does.
void L6470_setSwitchMode(int16_t switchMode)
{
  uint32_t configVal = getParam(CONFIG);
  // This bit is CONFIG 4, mask is 0x0010
  configVal &= ~(0x0010);
  //Now, OR in the masked incoming value.
  configVal |= (0x0010 & switchMode);
  setParam(CONFIG, configVal);
}

int16_t L6470_getSwitchMode()
{
  return (int16_t) (getParam(CONFIG) & 0x0010);
}

// There are a number of clock options for this chip- it can be configured to
//  accept a clock, drive a crystal or resonator, and pass or not pass the
//  clock signal downstream. Theoretically, you can use pretty much any
//  frequency you want to drive it; practically, this library assumes it's
//  being driven at 16MHz. Also, the device will use these bits to set the
//  math used to figure out steps per second and stuff like that.
void L6470_setOscMode(int16_t oscillatorMode)
{
  uint32_t configVal = getParam(CONFIG);
  // These bits are CONFIG 3:0, mask is 0x000F
  configVal &= ~(0x000F);
  //Now, OR in the masked incoming value.
  configVal |= (0x000F&oscillatorMode);
  setParam(CONFIG, configVal);
}

int16_t L6470_getOscMode()
{
  return (int16_t) (getParam(CONFIG) & 0x000F);
}

// The KVAL registers are...weird. I don't entirely understand how they differ
//  from the microstepping, but if you have trouble getting the motor to run,
//  tweaking KVAL has proven effective in the past. There's a separate register
//  for each case: running, static, accelerating, and decelerating.

void L6470_setAccKVAL(uint8_t kvalInput)
{
  setParam(KVAL_ACC, kvalInput);
}

uint8_t L6470_getAccKVAL()
{
  return (uint8_t) getParam(KVAL_ACC);
}

void L6470_setDecKVAL(uint8_t kvalInput)
{
  setParam(KVAL_DEC, kvalInput);
}

uint8_t L6470_getDecKVAL()
{
  return (uint8_t) getParam(KVAL_DEC);
}

void L6470_setRunKVAL(uint8_t kvalInput)
{
  setParam(KVAL_RUN, kvalInput);
}

uint8_t L6470_getRunKVAL()
{
  return (uint8_t) getParam(KVAL_RUN);
}

void L6470_setHoldKVAL(uint8_t kvalInput)
{
  setParam(KVAL_HOLD, kvalInput);
}

uint8_t L6470_getHoldKVAL()
{
  return (uint8_t) getParam(KVAL_HOLD);
}

// Enable or disable the low-speed optimization option. With LSPD_OPT enabled,
//  motion starts from 0 instead of MIN_SPEED and low-speed optimization keeps
//  the driving sine wave prettier than normal until MIN_SPEED is reached.
void L6470_setLoSpdOpt(bool enable)
{
  uint32_t temp = getParam(MIN_SPEED);
  if (enable) temp |= 0x00001000; // Set the LSPD_OPT bit
  else        temp &= 0xffffefff; // Clear the LSPD_OPT bit
  setParam(MIN_SPEED, temp);
}

bool L6470_getLoSpdOpt()
{
  return (bool) ((getParam(MIN_SPEED) & 0x00001000) != 0);
}

