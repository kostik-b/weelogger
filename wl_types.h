// Copyright QUB 2017

#ifndef wl_types_h_random_text
#define wl_types_h_random_text

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_LOG_BUFFER_LEN 256

typedef enum wl_log_levels
{
  WL_DEBUG    = 1,
  WL_VERBOSE  = 2,
  WL_INFO     = 3,
  WL_ERROR    = 4,
  WL_CRITICAL = 5
} wl_log_levels;

// return values
typedef enum wl_rc
{
  WL_SUCCESS        = 0,
  WL_MALLOC_ERROR   = -1,
  WL_BAD_ARGUMENTS  = -2,
  WL_PRINTF_FAILED  = -3,
  WL_TIME_ERROR     = -4
} wl_rc;

#ifdef __cplusplus
}
#endif

#endif
