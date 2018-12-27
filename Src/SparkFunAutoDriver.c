#include "SparkFunAutoDriver.h"

int16_t busyCheck(void)
{
  if (getParam(STATUS) & 0x0002) return 0;
  else                           return 1;
}
