#include <LedControl.h>

LedControl lc=LedControl(12,11,10,1); // lc is our object
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219

int hackLightPin = 13;
int hackButtonPin = A0;

unsigned long CYCLE_TIME = 1000*60*60*4;
unsigned long BASE_COOLDOWN_TIME = 1000*60*5;
int BASE_N_HACKS = 4;

unsigned long cooldown_time = BASE_COOLDOWN_TIME;
int nHacksPerCycle = BASE_N_HACKS;

int cycleNHacks = 0;
unsigned long cycleStartTime = 0;
unsigned long lastHackTime = 0;


boolean hackButtonState = false;

void setup()
{
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0,false);// turn off power saving, enables display
  lc.setIntensity(0,8);// sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen'
  
  pinMode(hackLightPin, OUTPUT);
  
  setHackButton(true);
  // TODO set info.
  
  // debug
  Serial.begin(9600);
}

void showTime(unsigned long timeMillis)//int h, int m, int s)
{
  unsigned long fixedTime = timeMillis / 1000;
  int s = fixedTime % 60;
  fixedTime /= 60;
  int m = fixedTime % 60;
  fixedTime /= 60;
  int h = fixedTime % 60;
  
  if (h > 0)
  {
    lc.setDigit(0,0,m%10,s%2==0);
    lc.setDigit(0,1,m/10,false);
    lc.setDigit(0,2,h%10,true);
    lc.setDigit(0,3,h/10,false);
  }
  else
  {
    lc.setDigit(0,0,s%10,false);
    lc.setDigit(0,1,s/10,false);
    lc.setDigit(0,2,m%10,true);
    lc.setDigit(0,3,m/10,false);
  }
}

void setHackButton(boolean enabled)
{
  digitalWrite(hackLightPin, enabled ? HIGH : LOW);
}

void updateCycle()
{
  if (cycleNHacks >= nHacksPerCycle)
  {
    int timeLeft = CYCLE_TIME - (millis() - cycleStartTime);
    if (timeLeft <= 0)
    {
      // Cycle over!
      cycleStartTime = 0;
      lc.clearDisplay(0);
      setHackButton(true);
    }
    else
    {
      showTime(timeLeft);
    }
  }
  else
  {
    int timeLeft = cooldown_time - (millis() - lastHackTime);
    if (timeLeft <= 0)
    {
      lc.clearDisplay(0);
      lastHackTime = 0;
      setHackButton(true);
    }
    else
    {
      showTime(timeLeft);
    }
  }
}

void loop()
{
  if (cycleStartTime > 0)
  {
    updateCycle();
  }
  
  int hackButtonReading = analogRead(hackButtonPin);
  if (hackButtonReading < 512)
  {
    hackButtonState = false;
  }
  else
  {
    if (!hackButtonState)
    {
      hackButtonState = true;
      if (lastHackTime > 0)
      {
        // Can't hack yet, ignore.
      }
      else if (cycleNHacks >= nHacksPerCycle)
      {
        // Can't hack, ignore.
      }
      else
      {
        // Hacked!
        if (cycleStartTime == 0)
        {
          cycleStartTime = millis();
        }
        cycleNHacks++;
        setHackButton(false);
        if (cycleNHacks < nHacksPerCycle)
        {
          lastHackTime = millis();
        }
      }
    }
  }
  
  delay(200);
}
