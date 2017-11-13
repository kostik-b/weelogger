// Copyright 2017 QUB

#ifndef wee_logger_sink_h
#define wee_logger_sink_h

#ifdef __cplusplus
extern "C" {
#endif

/*
  The purpose of the sink is to print the given string to some output
  device (screen, network, stdout etc)
*/

typedef void* wl_sink_impl;

typedef struct _wl_sink
{
  int  (*write)   (wl_sink_impl          impl, const char* str, const int len);

  void (*destroy) (struct _wl_sink*      self);

  // our approach to polymorphism
  wl_sink_impl  m_impl;
} _wl_sink;

typedef _wl_sink* wl_sink;

#ifdef __cplusplus
}
#endif

#endif
