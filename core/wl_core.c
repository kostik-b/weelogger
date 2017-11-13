// Copyright 2017 QUB

#include "wl_core.h"
#include <stdlib.h>
#include <string.h>

wl_core wl_core_create (const wl_log_levels logging_level,
                        const bool          thread_safe)
{
  wl_core core = malloc (sizeof (_wl_core));

  if (!core)
  {
    return NULL;
  }
  // initalize all the vars
  core->m_sinks           = NULL;
  core->m_sinks_num       = 0;

  core->m_attributes      = NULL;
  core->m_attributes_num  = 0;

  // we want to be thread-safe if necessary
  if (thread_safe)
  {
    int rc = pthread_mutex_init(&(core->m_mutex), NULL);

    if (rc == 0)
    {
      core->m_thread_safe = true;
    }
    else
    {
      core->m_thread_safe = false;
      printf ("CRITICAL: could not initialize mutex, continuing with initialization\n");
    }
  }

  // buffer to print to
  core->m_buffer          = malloc (MAX_LOG_BUFFER_LEN);
  if (!core->m_buffer)
  {
    printf ("FATAL: could not allocate the buffer for logging\n");
    return NULL;
  }
  core->m_buffer_len      = MAX_LOG_BUFFER_LEN;

  // current logging level - set once and for all at
  // the startup
  core->m_current_level   = logging_level;

  return core;
}

#define WL_CORE_ADD_ELEMENT(type, array, num_elements, new_element) \
  /* allocate m_sinks_num + 1 pointer array */                      \
  type* tmp = malloc (sizeof(type)*(num_elements + 1));             \
  if (!tmp)                                                         \
  {                                                                 \
    return WL_MALLOC_ERROR;                                         \
  }                                                                 \
                                                                    \
  /* copy all the previous pointers if any*/  \
  if (num_elements == 0)                      \
  {                                           \
    array         = tmp;                      \
    num_elements  = 1;                        \
                                              \
    array[0]      = new_element;              \
  }                                           \
  else                                        \
  {                                           \
    /* copy over all the old ones*/           \
    memcpy (tmp, array, sizeof(type)*(num_elements));  \
    /* add the new sink */                                    \
    tmp [num_elements] = new_element;                         \
    /* free the old buffer */                                 \
    free (array);                                             \
    /* assign the new one over */                             \
    array        = tmp;                                       \
    /* and update the num counter */                          \
    num_elements   += 1;                                      \
  }


int wl_core_add_sink (wl_core self, wl_sink new_sink)
{
  if (self == NULL)
  {
    return WL_BAD_ARGUMENTS;
  }

  WL_CORE_ADD_ELEMENT (wl_sink, self->m_sinks, self->m_sinks_num, new_sink)

  return WL_SUCCESS;
}

int wl_core_add_attribute (wl_core self, wl_attribute new_attribute)
{
  if (self == NULL)
  {
    return WL_BAD_ARGUMENTS;
  }

  WL_CORE_ADD_ELEMENT (wl_attribute, self->m_attributes, self->m_attributes_num, new_attribute)

  return WL_SUCCESS;
}

void wl_core_destroy (wl_core self, const bool destroy_attributes_sinks)
{
  if (destroy_attributes_sinks)
  {
    for (int i = 0; i < self->m_sinks_num; ++i)
    {
      wl_sink sink = self->m_sinks[i];
      sink->destroy (sink);
    }

    for (int i = 0; i < self->m_attributes_num; ++i)
    {
      wl_attribute attribute = self->m_attributes[i];
      attribute->destroy (attribute);
    }
  }

  self->m_sinks_num = 0;
  if (self->m_sinks)
  {
    free (self->m_sinks);
  }
  self->m_sinks = NULL;


  self->m_attributes_num = 0;
  if (self->m_attributes)
  {
    free (self->m_attributes);
  }
  self->m_attributes = NULL;

  free (self);
}

static int wl_write_level (char* buffer, const int level, const size_t remaining_len)
{
  int bytes_written = 0;
  switch (level)
  {
    case WL_DEBUG:
      bytes_written = snprintf (buffer, remaining_len, "[DEBUG]: ");
      break;
    case WL_VERBOSE:
      bytes_written = snprintf (buffer, remaining_len, "[VERBOSE]: ");
      break;
    case WL_INFO:
      bytes_written = snprintf (buffer, remaining_len, "[INFO]: ");
      break;
    case WL_ERROR:
      bytes_written = snprintf (buffer, remaining_len, "[ERROR]: ");
      break;
    case WL_CRITICAL:
      bytes_written = snprintf (buffer, remaining_len, "[CRITICAL]: ");
      break;
    default: // do nothing
      break;
  }

  // did we write anything?
  if (bytes_written < 1)
  {
    return WL_PRINTF_FAILED;
  }
  else
  {
    // just making sure we don't return more chars than remaining_len
    if (bytes_written <= remaining_len)
    {
      return bytes_written;
    }
    else
    {
      return remaining_len;
    }
  }
}

static wl_rc wl_core_do_log (wl_core self, const int level, const char* msg, va_list args)
{
  if (level < self->m_current_level)
  {
    return WL_SUCCESS;
  }

  if (self->m_thread_safe)
  {
    pthread_mutex_lock (&(self->m_mutex));
  }
  // for each attribute pass the buffer and let the attribute write to the buffer
  // - need to keep track of the number of bytes written
  int remaining_len = self->m_buffer_len - 1; // to save space for null char
  int buffer_offset = 0;
  for (int i = 0; i < self->m_attributes_num; ++i)
  {
    wl_attribute  attribute     = self->m_attributes[i];

    int rc = attribute->write_attribute (attribute->m_impl,
                                         self->m_buffer + buffer_offset,
                                         remaining_len);
    // did we write anything?
    if (rc >= 0) // filer out the error code
    {
      if (rc <= remaining_len) // just being careful with checking for overflow
      {
        remaining_len -= rc; // update the remaining len
        buffer_offset += rc; // adjust the buffer offset
      }
      // if rc > remaining_len, we just ignore it, there is some error in the attribute
    }
  }

  int rc = wl_write_level (self->m_buffer + buffer_offset, level, remaining_len);
  if (rc > 0)
  {
    remaining_len -= rc; // update the remaining len
    buffer_offset += rc; // adjust the buffer offset
  }
  
  // then we print the msg to remaining part of the buffer using snprintf
  // then we pass the buffer to each one of the sinks
  // quite simple as it is designed to be. If we wish to log line num, file names,
  // etc, then we can simply create macros to do that, so that these are the part
  // of the msg

  rc = vsnprintf (self->m_buffer + buffer_offset, remaining_len, msg, args);

  if (rc > remaining_len)
  {
    rc = remaining_len;
  }
  if (rc > 0)
  {
    remaining_len -= rc; // update the remaining len
    buffer_offset += rc; // adjust the buffer offset
  }
  else
  {
    printf ("[CRITICAL - Wee Logger]: Could not write the message to the buffer\n");
    pthread_mutex_unlock (&(self->m_mutex));
    return WL_PRINTF_FAILED;
  }

  self->m_buffer[buffer_offset] = '\0';
  for (int i = 0; i < self->m_sinks_num; ++i)
  {
    wl_sink sink = self->m_sinks[i];
    sink->write (sink->m_impl, self->m_buffer, remaining_len);
  }

  pthread_mutex_unlock (&(self->m_mutex));
  return WL_SUCCESS; // no error reporting for now
}

wl_rc wl_core_log (wl_core self, const int level, const char* msg, ...)
{
  va_list args;
  va_start(args, msg);

  wl_rc rc = wl_core_do_log (self, level, msg, args);

  va_end(args);

  return rc;
}

wl_rc wl_core_vlog(wl_core self, const int    level, const char* msg, va_list args)
{
  return wl_core_do_log (self, level, msg, args);
}

