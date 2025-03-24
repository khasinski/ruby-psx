/*! @file
  @brief
  Hardware abstraction layer
        for PlayStation 1

  <pre>
  Copyright (C) 2025 - Chris Hasiński

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

#ifndef MRBC_SRC_HAL_H_
#define MRBC_SRC_HAL_H_

#include <unistd.h>

#define MRBC_OUT_OF_MEMORY() mrbc_out_of_memory_ps1()

#ifdef __cplusplus
extern "C" {
#endif

void mrbc_tick(void);

#define hal_init()	  ((void)0)
#define hal_enable_irq()  ((void)0)
#define hal_disable_irq() ((void)0)
#define hal_idle_cpu()    (usleep(MRBC_TICK_UNIT * 1000), mrbc_tick())

void hal_abort(const char *s);
void mrbc_out_of_memory_posix( void );

inline static int hal_write(int fd, const void *buf, int nbytes)
{
  return (int)write(1, buf, nbytes);
}

inline static int hal_flush(int fd)
{
  return fsync(1);
}


#ifdef __cplusplus
}
#endif
#endif
