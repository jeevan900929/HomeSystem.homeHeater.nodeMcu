#ifndef watchDogTimer.h
#define watchDogTimer.h

#ifdef WATCHDOG

#include <Ticker.h>

Ticker secondTick;
volatile int watchdogCount = 0;

void ISRwatchdog()
{
  watchdogCount++;
  if (watchdogCount == 9)
  {
    Serial.println("the watchdog bit");
    ESP.reset();
  }
}
#endif // WATCHDOG

void initWatchDogTimer()
{

#ifdef WATCHDOG
  ESP.wdtDisable();
  secondTick.attach(1, ISRwatchdog);
  ESP.wdtEnable(WDTO_8S);
#endif // WATCHDOG

}

void feedWdt()
{
#ifdef WATCHDOG
  ESP.wdtFeed();
  Serial.print("Watchdog pat at ");
  Serial.println(watchdogCount);
  watchdogCount = 0;
  yield();
#endif // WATCHDOG
}

#endif // watchDogTimer.h
