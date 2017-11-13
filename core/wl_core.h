// Copyright 2017 QUB

#ifndef wee_logger_core_h
#define wee_logger_core_h

#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include <sinks/wl_sink.h>
#include <attributes/wl_attribute.h>

#include <wl_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
  The core has to be as minimal as possible.
*/

typedef struct _wl_core
{
  // sinks and attributes
  wl_sink*        m_sinks;
  size_t          m_sinks_num;

  wl_attribute*   m_attributes;
  size_t          m_attributes_num;

  // we want to be thread-safe if necessary
  pthread_mutex_t m_mutex;
  bool            m_thread_safe;

  // buffer to print to
  char*           m_buffer;
  size_t          m_buffer_len;

  // current logging level - set once and for all at
  // the startup
  wl_log_levels   m_current_level;  

} _wl_core;

typedef _wl_core* wl_core;

wl_core   wl_core_create         (const wl_log_levels logging_level, const bool thread_safe);

wl_rc     wl_core_log            (wl_core self, const int    level, const char* msg, ...);

wl_rc     wl_core_vlog           (wl_core self, const int    level, const char* msg, va_list args);

wl_rc     wl_core_add_sink       (wl_core self, wl_sink      new_sink);

wl_rc     wl_core_add_attribute  (wl_core self, wl_attribute attribute);

/* the user can either keep hold of sinks and attributes
   and destroy those oneself, or it can be delegated to 
   the wl_core_destroy */
void      wl_core_destroy        (wl_core self, const bool    destroy_attributes_sinks);

#ifdef __cplusplus
}
#endif

#endif
