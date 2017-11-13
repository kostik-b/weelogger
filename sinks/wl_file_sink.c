// Copyright QUB 2017

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio_ext.h>
#include "wl_basic_sinks.h"
#include <wl_types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_NAME_LENGTH 128
#define O_NEW ("w")
#define O_APPEND ("a")

typedef struct wl_file_container
{
  char* m_file_name;
  float m_max_size;
  float m_current_size;
  int   m_max_files;
  FILE* m_fd;
  int   m_file_name_num;
} wl_file_container;

// ------------- helper functions ----------------
static char* wl_generate_next_file_name (int* file_name_num, const int max_files, const char* file_name_base)
{
  static char fname [FILE_NAME_LENGTH];

  sprintf(fname, "%s_%d%s", file_name_base, (*file_name_num)%max_files, ".log");

  ++(*file_name_num);

  return fname;
}

static FILE* wl_open_file (const char* file_name, const char* attr)
{
  FILE* fd = fopen (file_name, "w" /*attr*/);
  if (!fd)
  {
    printf ("Could not create file.\n");
    exit (EXIT_FAILURE);
  }

  return fd;
}

static void wl_close_file(FILE* fd)
{
  if (fd)
  {
    fclose(fd);
  }
}

static wl_rc wl_write_file (FILE* fd, const char* msg)
{
  //int bufsize;
  //bufsize = __fbufsize(fd);
  if(fprintf(fd, "%s\n" , msg) < 0)
  {
    printf ("[CRITICAL ERROR] Failed writing to file.\n");
    return WL_PRINTF_FAILED;
  }
  fflush(fd);

  return WL_SUCCESS;
}

// -------------- "exported" functions ---------------
wl_rc wl_fsink_write (wl_sink_impl  impl,
                     const char*  str,
                     const int    len)
{
  wl_file_container* fc   = (wl_file_container*)impl;
  fc->m_current_size     += (len*1.0) / (1024*1024*1.0);

  // first write, then check the size, it's OK, the files' size should be over
  // the threshold
  if (wl_write_file (fc->m_fd, str) != WL_SUCCESS)
  {
    return WL_PRINTF_FAILED;
  }

  if (fc->m_current_size > fc->m_max_size) // need to roll over the file
  {
    wl_close_file (fc->m_fd);

    const char* fname   = strdup (wl_generate_next_file_name(&(fc->m_file_name_num), fc->m_max_files, fc->m_file_name));
    fc->m_fd            = wl_open_file (fname, O_NEW);

    fc->m_current_size  = 0.0f;
  }

  return WL_SUCCESS;
}

void wl_fsink_destroy (struct _wl_sink* self)
{
  if (self)
  {
    if (self->m_impl)
    {
      wl_file_container* fc = (wl_file_container*) self->m_impl;

      if (fc->m_file_name)
      {
        free (fc->m_file_name);
      }
      if (fc->m_fd)
      {
        wl_close_file (fc->m_fd);
      }
      free (fc);
    }
    free (self);
  }
}



wl_sink wl_create_file_sink   (const int max_files, const float max_size, const char* file_name_base)
{
  wl_sink sink = malloc (sizeof (_wl_sink));
  if (!sink)
  {
    printf ("[CRITICAL - SINK: could not allocate file sink]\n");
    return NULL;
  }

  sink->write   = wl_fsink_write;
  sink->destroy = wl_fsink_destroy;

// impl part

  wl_file_container* fc = malloc(sizeof(wl_file_container));
  if (!fc)
  {
    printf ("[CRITICAL - FILE CONTAINER: could not allocate file container]\n");
    return NULL;
  }

  // here we keep track of the number of the open file
  fc->m_file_name_num = 0;
  fc->m_file_name     = strdup (file_name_base);
  fc->m_fd            = NULL;
  fc->m_current_size  = 0.0f;
  fc->m_max_files     = max_files;
  fc->m_max_size      = max_size;

  // start generating file names and keep on "fstating" them until we
  // get an error, i.e. a file does not exist - that is our file
  for (int c = 0 ; c < max_files; ++c)
  {
    const char* fname = wl_generate_next_file_name(&(fc->m_file_name_num), fc->m_max_files, fc->m_file_name);

    struct stat file_stats;
    if (stat (fname, &file_stats) < 0)
    {
      // we found the "empty file!"
      fc->m_fd        = wl_open_file (fname, O_NEW);
      break;
    }
  }

  if (fc->m_fd == NULL) // have we exhausted our max_files set?
  {
    // we just roll over and start a new file
    const char* fname = strdup (wl_generate_next_file_name(&(fc->m_file_name_num), fc->m_max_files, fc->m_file_name));
    fc->m_fd          = wl_open_file (fname, O_NEW);
  }

  sink->m_impl  = fc;
  return sink;
}

