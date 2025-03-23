/*
 * File:stdarg.h
 */
/*
 * $PSLibId: Run-time Library Release 4.7$
 */

#ifndef _STDARG_H
#define _STDARG_H


#define __va_rounded_size(TYPE)  \
(((sizeof(TYPE) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#define va_start(AP, LASTARG)  \
(AP = (va_list) ((char *) &(LASTARG) + __va_rounded_size(LASTARG)))

#define va_arg(AP, TYPE)  \
(AP = (va_list) ((char *) (AP) + __va_rounded_size(TYPE)),  \
*((TYPE *) (void *) ((char *) (AP) - __va_rounded_size(TYPE))))

#define va_end(AP)  \
(AP = (va_list) 0)


typedef void *va_list;

#endif /* _STDARG_H */
