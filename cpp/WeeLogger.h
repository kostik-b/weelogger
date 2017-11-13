// Copyright QUB 2017

#ifndef wl_logger_cpp_h
#define wl_logger_cpp_h

#include <wl_logger.h>

// a few macros for logging. The idea is that in the future 
// we could turn them off for an optimized build


#define LOG_DEBUG(msg, ...)   WeeLogger::getInstance().log (WL_DEBUG,   msg, ##__VA_ARGS__)
#define LOG_INFO(msg, ...)    WeeLogger::getInstance().log (WL_INFO,    msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...)   WeeLogger::getInstance().log (WL_ERROR,   msg, ##__VA_ARGS__)
#define LOG_VERBOSE(msg, ...) WeeLogger::getInstance().log (WL_VERBOSE, msg, ##__VA_ARGS__)

// these include the file name (__FILE_NAME_ONLY__) in the output, feel free
// to add more of these
#define LOG_LOC_ERROR(msg, ...)   WeeLogger::getInstance().log (WL_ERROR,  \
                                         __FILE_NAME_ONLY__ msg, ##__VA_ARGS__)

#define LOG_LOC_DEBUG(msg, ...)   WeeLogger::getInstance().log (WL_DEBUG,  \
                                         __FILE_NAME_ONLY__ msg, ##__VA_ARGS__)


class WeeLogger
{
public:
  static WeeLogger& getInstance ();

  static void         destroy     (WeeLogger& singleton);

  void log (const wl_log_levels level, const char* msg, ...);

private:
  WeeLogger (); // it's a singleton
  ~WeeLogger();

  // we want to hide details as much as possible
  struct WeeLoggerImpl; // forward declaration
  WeeLoggerImpl& m_impl;
}; // class WeeLogger

#endif
