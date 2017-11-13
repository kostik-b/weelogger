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
  wl_sink      stdout_sink  = wl_create_stdout_sink ();
  wl_core_add_sink (core, stdout_sink);

  // print some messages
  wl_core_log (core, WL_ERROR, "Some error encountered, error code is %d", 23);

  wl_core_log (core, WL_INFO, "Some unimportant info");

  wl_core_log (core, WL_DEBUG, "Some debug message: %s.", "Ground control to major Tom:vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv   zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz");

  wl_core_log (core, 10,       "Unknown logging level");

  // destroy
  wl_core_destroy (core, true);

  return 0;
}
