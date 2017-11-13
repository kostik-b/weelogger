// Copyright QUB 2017

#include <wl_logger.h>

int main (int argc, char** argv)
{
  // create the logger
  wl_core core = wl_core_create (WL_INFO, true);
  // add the date and time attributes
  wl_attribute time_date    = wl_create_date_time_attribute ();
  wl_core_add_attribute (core, time_date);
  // add the std out and file sinks
  wl_sink      file_sink    = wl_create_file_sink   (3, 1, "wlog");
  wl_core_add_sink (core, file_sink);

  // print some messages
  for (int i = 0; i < 100000; ++i)
  {
    wl_core_log (core, WL_INFO, "Some unimportant info #%d", i);
  }

  // destroy
  wl_core_destroy (core, true);

  return 0;
}
