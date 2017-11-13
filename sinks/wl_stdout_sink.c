// Copyright QUB 2017

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdio_ext.h>
#include <wl_types.h>
#include "wl_basic_sinks.h"


int wl_stdout_write (wl_sink_impl impl,
                     const char*  str,
                     const int len)
{
  printf ("%s\n", str);

  return WL_SUCCESS;
}

void wl_stdout_destroy (struct _wl_sink* self)
{
  if (self)
  {
    free (self);
  }
}

wl_sink wl_create_stdout_sink ()
{
  wl_sink sink = malloc (sizeof (_wl_sink));
  if (!sink)
  {
    printf ("[CRITICAL - SINK: could not allocate stdout sink]\n");
    return NULL;
  }

  sink->write   = wl_stdout_write;
  sink->destroy = wl_stdout_destroy;
  sink->m_impl  = NULL;

  return sink;
}

