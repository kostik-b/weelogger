// Copyright QUB 2017

#include <cpp/WeeLogger.h>

int main (int argc, char** argv)
{
  WeeLogger& logger = WeeLogger::getInstance ();

  logger.log (WL_INFO, "WeeLogger test");

  LOG_LOC_ERROR ("SnuclLogger location error");

  WeeLogger::destroy (logger);

  return 0;
}
