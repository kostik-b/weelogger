// Copyright 2017 QUB

#ifndef wee_logger_basic_sinks_h
#define wee_logger_basic_sinks_h

#include "wl_sink.h"

#ifdef __cplusplus
extern "C" {
#endif

// stdout sink
wl_sink wl_create_stdout_sink ();

// file sink
wl_sink wl_create_file_sink   (const int max_files, const float max_size, const char* file_name_base);

wl_sink wl_create_sink        ();
#ifdef __cplusplus
}
#endif

#endif
