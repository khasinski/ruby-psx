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

#include "hal.h"

void hal_abort( const char *s )
{
  if( s ) {
    hal_write(1, s, strlen(s));
  }
  exit( 1 );
}

void mrbc_out_of_memory_ps1( void )
{
  static const char msg[] = "Fatal error: Out of memory.\n";
  hal_write(2, msg, sizeof(msg)-1);
  hal_abort(0);
}
