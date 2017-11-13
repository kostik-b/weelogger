// Copyright QUB 2017

#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include "wl_basic_attributes.h"
#include <time.h>
#include <stdlib.h>

int wl_write_date_time (wl_attribute_impl impl, char* buffer, const size_t max_len)
{
  // get time of day
  // convert to tm with localtime
  // print to buffer adding the millisecond's part

  struct timeval time_now;
  int rc = gettimeofday (&time_now, NULL);
  if (rc < 0)
  {
    printf ("[CRITICAL] : could not get time of day\n");
    return WL_TIME_ERROR;
  }

  struct tm time_now_broken_down;
  struct tm* result = localtime_r (&(time_now.tv_sec), &(time_now_broken_down));
  if (result != &time_now_broken_down) // the pointers should be identical
  {
    printf ("[CRITICAL] : could not convert time of day\n");
    return WL_TIME_ERROR;
  }

  rc = snprintf (buffer, max_len, "%02d.%02d.%02d-%02d:%02d:%02d.%02ld ",
              time_now_broken_down.tm_year + 1900,
              time_now_broken_down.tm_mon  + 1,
              time_now_broken_down.tm_mday,

              time_now_broken_down.tm_hour,
              time_now_broken_down.tm_min,
              time_now_broken_down.tm_sec,

              time_now.tv_usec / 1000);

  if (rc < 0)
  {
    return WL_PRINTF_FAILED;
  }
  else if (rc > max_len)
  {
    rc = max_len;
  }

  return rc;
}

void wl_destroy_date_time (wl_attribute self)
{
  free (self);
}

wl_attribute wl_create_date_time_attribute ()
{
  // create the attribute
  wl_attribute att = malloc (sizeof (_wl_attribute));
  if (!att)
  {
    printf ("[CRITICAL - ATTRIBUTE : could not allocate date time attribute]\n");
    return NULL;
  }
  
  // set the function and data pointers
  att->m_impl           = NULL;
  att->write_attribute  = wl_write_date_time;
  att->destroy          = wl_destroy_date_time;

  return att;
}

// ---------------- P I D ----------------------------------------
// ------------ A T T R I B U T E --------------------------------

int wl_write_pid (wl_attribute_impl impl, char* buffer, const size_t max_len)
{
  int rc = snprintf (buffer, max_len, "[PID: %d]", *((int*)impl));

  if (rc < 0)
  {
    return WL_PRINTF_FAILED;
  }
  else if (rc > max_len)
  {
    rc = max_len;
  }

  return rc;
}

void wl_destroy_pid (wl_attribute self)
{
  if (self->m_impl)
  {
    free (self->m_impl);
  }
  free (self);
}

wl_attribute wl_create_pid_attribute ()
{
  // create the attribute
  wl_attribute att = malloc (sizeof (_wl_attribute));
  if (!att)
  {
    printf ("[CRITICAL - ATTRIBUTE : could not allocate PID attribute]\n");
    return NULL;
  }

  int* impl_int         = malloc (sizeof (int));
  *impl_int             = getpid ();

  // set the function and data pointers
  att->m_impl           = impl_int;
  att->write_attribute  = wl_write_pid;
  att->destroy          = wl_destroy_pid;

  return att;
}

