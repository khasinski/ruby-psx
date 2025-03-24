/*! @file
  @brief
  Hardware abstraction layer
        for PlayStation 1

  <pre>
  Copyright (C) 2025 - Chris Hasiński

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "hal.h"

int hal_write(int fd, const void *buf, int nbytes)
{
  // Only handle stdout (fd 1) and stderr (fd 2)
  if (fd != 1 && fd != 2) {
    return -1;
  }

  const char *data = (const char *)buf;
  for (int i = 0; i < nbytes; i++) {
    printf("%c", data[i]);
  }

  return nbytes;
}

int hal_flush(int fd)
{
  return fsync(1);
}

void hal_abort( const char *s )
{
  if( s ) {
    hal_write(1, s, strlen(s));
  }
  exit( 1 );
}
