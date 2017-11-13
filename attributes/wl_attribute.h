// Copyright 2017 QUB

#ifndef wee_logger_attribute_h
#define wee_logger_attribute_h

#include <wl_types.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
  The purpose of an attribute is to print date, time, etc to the buffer preceding the actual message
*/
typedef void* wl_attribute_impl;

typedef struct _wl_attribute
{
  /* returns the num of chars written, or a negative error code (similar to snprintf) 
     the num of chars written will always be less than or equal to max_len
  */
  int   (*write_attribute) (wl_attribute_impl impl, char* buffer, const size_t max_len);

  void  (*destroy)         (struct _wl_attribute* self);

  wl_attribute_impl m_impl;
} _wl_attribute;

typedef _wl_attribute* wl_attribute;

#ifdef __cplusplus
}
#endif

#endif
