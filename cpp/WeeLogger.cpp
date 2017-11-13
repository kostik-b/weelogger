// Copyright QUB 2017

#include "WeeLogger.h"
#include <stdlib.h>

// we really use this as a storage
struct WeeLogger::WeeLoggerImpl
{
  WeeLoggerImpl ()
    : m_core (NULL)
  {}

  wl_core   m_core;
}; // struct WeeLogger::WeeLoggerImpl

WeeLogger::WeeLogger ()
  : m_impl (*(new WeeLoggerImpl))
{
  // get the logger level from env
  wl_log_levels wlLogLevel  = WL_INFO;
  const char*   logLevelStr = getenv ("WL_LOG_LEVEL");

  if (logLevelStr)
  {
    int intValue = atoi (logLevelStr);
    if ((intValue >= WL_DEBUG) && (intValue <= WL_CRITICAL))
    {
      wlLogLevel = static_cast<wl_log_levels>(intValue);
    }
  }
  // create wl_core and set the logging level
  m_impl.m_core = wl_core_create (wlLogLevel, true);
  // create and set attributes and sinks
  wl_attribute time_date    = wl_create_date_time_attribute ();
  wl_core_add_attribute (m_impl.m_core, time_date);
  wl_core_add_attribute (m_impl.m_core, wl_create_pid_attribute ());
  // add the std out and file sinks
  wl_sink      stdout_sink  = wl_create_stdout_sink ();
  wl_core_add_sink (m_impl.m_core, stdout_sink);
}

WeeLogger::~WeeLogger ()
{
  // destroy wl_core
  wl_core_destroy (m_impl.m_core, true);
  // delete m_impl
  delete &m_impl;
}

WeeLogger& WeeLogger::getInstance ()
{
  static WeeLogger* logger = new WeeLogger;

  return *logger;
}

void WeeLogger::destroy (WeeLogger& singleton)
{
  delete &singleton;
}

void WeeLogger::log (const wl_log_levels level, const char* msg, ...)
{
  va_list args;
  va_start(args, msg);

  wl_core_vlog (m_impl.m_core, level, msg, args);

  va_end(args);
}
