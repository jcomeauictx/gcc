/* Provide a version of sprintf that uses an array of longs/pointers rather
   than a va_list, and using a buffer of fixed size. Based on _doprnt.c,
   Copyright (C) 1998-2023 Free Software Foundation, Inc.
   Contributed by Kaveh Ghazi  (ghazi@caip.rutgers.edu)  3/29/98
   _arraysnprintf.c edits by John Comeau <jc@unternet.net> 2023-03-25

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#include "config.h"
#include "ansidecl.h"
#include "safe-ctype.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

int errprintf(const char *format, ...);
char * memdump(char *buffer, void *location, int count);
int checkit(const char * format, void **args);

#define BUFFERSIZE 1024

#define COPY_INT \
  do { \
	 const int value = (int)(long)(*args++); \
	 char buf[32]; \
	 /*errprintf("COPY_INT called with value %d\n", value);*/ \
	 ptr++; /* Go past the asterisk.  */ \
	 *sptr = '\0'; /* NULL terminate sptr.  */ \
	 sprintf(buf, "%d", value); \
	 strcat(sptr, buf); \
         /*errprintf("COPY_INT result: %s\n", sptr);*/ \
	 while (*sptr) sptr++; \
     } while (0)

#define PRINT_CHAR(CHAR) \
  do { \
	 if (total_printed < maxlength) \
           *(formatted + total_printed++) = *ptr++; \
     } while (0)

#define PRINT_TYPE(TYPE) \
  do { \
	int result; TYPE value; \
        if (strstr(#TYPE, "double") != NULL) value = *(TYPE *)*args++; \
        else value = *(TYPE *)args++; \
	*sptr++ = *ptr++; /* Copy the type specifier.  */ \
	*sptr = '\0'; /* NULL terminate sptr.  */ \
	result = snprintf(formatted + total_printed, \
	  maxlength - total_printed, specifier, value); \
	if (result == -1) \
	  return -1; \
	else \
	  { \
	    total_printed += result; \
	    continue; \
	  } \
      } while (0)

int
_arraysnprintf (char *formatted, size_t maxlength, const char *format,
	       	void **args)
  /* NOTE that `maxlength` should always be at least 1 less than the size
   * of the `formatted` buffer */
{
  const char * ptr = format;
  char specifier[128];
  int total_printed = 0;
  
  while (*ptr != '\0')
    {
      if (*ptr != '%') /* While we have regular characters, print them.  */
	PRINT_CHAR(*ptr);
      else /* We got a format specifier! */
	{
	  char * sptr = specifier;
	  int wide_width = 0, short_width = 0;
	  
	  *sptr++ = *ptr++; /* Copy the % and move forward.  */

	  while (strchr ("-+ #0", *ptr)) /* Move past flags.  */
	    *sptr++ = *ptr++;

	  if (*ptr == '*')
	    COPY_INT;
	  else
	    while (ISDIGIT(*ptr)) /* Handle explicit numeric value.  */
	      *sptr++ = *ptr++;
	  
	  if (*ptr == '.')
	    {
	      *sptr++ = *ptr++; /* Copy and go past the period.  */
	      if (*ptr == '*')
		COPY_INT;
	      else
		while (ISDIGIT(*ptr)) /* Handle explicit numeric value.  */
		  *sptr++ = *ptr++;
	    }
	  while (strchr ("hlL", *ptr))
	    {
	      switch (*ptr)
		{
		case 'h':
		  short_width = 1;
		  break;
		case 'l':
		  wide_width++;
		  break;
		case 'L':
		  wide_width = 2;
		  break;
		default:
		  abort();
		}
	      *sptr++ = *ptr++;
	    }

	  switch (*ptr)
	    {
	    case 'd':
	    case 'i':
	    case 'o':
	    case 'u':
	    case 'x':
	    case 'X':
	    case 'c':
	      {
		/* Short values are promoted to int, so just copy it
                   as an int and trust the C library printf to cast it
                   to the right width.  */
		if (short_width)
		  PRINT_TYPE(int);
		else
		  {
		    switch (wide_width)
		      {
		      case 0:
			PRINT_TYPE(int);
			break;
		      case 1:
			PRINT_TYPE(long);
			break;
		      case 2:
		      default:
#if defined(__GNUC__) || defined(HAVE_LONG_LONG)
			PRINT_TYPE(long long);
#else
			PRINT_TYPE(long); /* Fake it, hope for the best.  */
#endif
			break;
		      } /* End of switch (wide_width) */
		  } /* End of else statement */
	      } /* End of integer case */
	      break;
	    case 'f':
	    case 'e':
	    case 'E':
	    case 'g':
	    case 'G':
	      {
		if (wide_width == 0)
		  PRINT_TYPE(double);
		else
		  {
#if defined(__GNUC__) || defined(HAVE_LONG_DOUBLE)
		    PRINT_TYPE(long double);
#else
		    PRINT_TYPE(double); /* Hope for the best.  */
#endif
		  }
	      }
	      break;
	    case 's':
	      PRINT_TYPE(char *);
	      break;
	    case 'p':
	      PRINT_TYPE(void *);
	      break;
	    case '%':
	      PRINT_CHAR('%');
	      break;
	    default:
	      abort();
	    } /* End of switch (*ptr) */
	} /* End of else statement */
    }

  return total_printed;
}

#ifdef TEST

#include <math.h>
#ifndef M_PI
#define M_PI (3.1415926535897932385)
#endif

#define resultformat "printed %d characters\n"

#define RESULT(x) do \
{ \
    int i = (x); \
    if (strstr(#x, "checkit") != NULL) \
      checkit(resultformat, (void * []){(void *)(long)i}); \
    else printf(resultformat, i); \
    fflush(stdin); \
} while (0)

int checkit(const char* format, void **args)
{
  int result;
  char formatted[BUFFERSIZE] = "";
  /* allocate a safety zone in case program error causes buffer overrun */
  char safety[BUFFERSIZE];
  result = _arraysnprintf (formatted, BUFFERSIZE - 1, format, args);
  fprintf(stderr, "%s", formatted);  /* avoid double newline */
  return result;
}

int errprintf(const char *format, ...)
{
  int result;
  va_list args;
  va_start(args, format);
  result = vfprintf(stderr, format, args);
  va_end(args);
  return result;
}

char * memdump(char *buffer, void *location, int count) {
  unsigned char uchar, *ptr;
  char *saved = buffer;
  int i;
  buffer += sprintf(buffer, "%p: ", location);
  buffer--; /* so we can increment at start of each hex digit */
  ptr = (unsigned char *)location;
  for (i = 0; i < count; i++) {
    uchar = *ptr++;
    *++buffer = uchar >> 4; *buffer += *buffer > 9 ? 'a' - 10 : '0';
    *++buffer = uchar & 0xf; *buffer += *buffer > 9 ? 'a' - 10: '0';
  }
  *buffer = '\0';
  return saved;
}

int
main (void)
{
  /* constants needed for some tests below */
  const double PI = M_PI;
  unsigned char *pi = (unsigned char *)&PI;
  const double ONE = 1.0;
  unsigned char *one = (unsigned char *)&ONE;
  const double SEQ_SHORT = 1.23456;
  unsigned char *seq_short = (unsigned char *)&SEQ_SHORT;
  const long double SEQ_LONG = 1.234567890123456789L;
  unsigned char *seq_long = (unsigned char *)&SEQ_LONG;

  RESULT(checkit ("<%d>\n", (void * []) {(void *)0x12345678}));
  RESULT(printf ("<%d>\n", 0x12345678));

  RESULT(checkit ("<%200d>\n", (void * []) {(void *)5}));
  RESULT(printf ("<%200d>\n", 5));

  RESULT(checkit ("<%.300d>\n", (void * []) {(void *)6}));
  RESULT(printf ("<%.300d>\n", 6));

  RESULT(checkit ("<%100.150d>\n", (void * []) {(void *)7}));
  RESULT(printf ("<%100.150d>\n", 7));

  RESULT(checkit ("<%s>\n", (void * [])
		  {(void *)
		  "jjjjjjjjjiiiiiiiiiiiiiiioooooooooooooooooppppppppppppaa\n\
777777777777777777333333333333366666666666622222222222777777777777733333"}));
  RESULT(printf ("<%s>\n",
		 "jjjjjjjjjiiiiiiiiiiiiiiioooooooooooooooooppppppppppppaa\n\
777777777777777777333333333333366666666666622222222222777777777777733333"));

  RESULT(checkit ("<%f><%0+#f>%s%d%s>\n", (void * []) {
		  (void *)one, (void *)one, (void *)"foo", (void *)77,
		  (void *) "asdjffffffffffffffiiiiiiiiiiixxxxx"}));
  RESULT(printf ("<%f><%0+#f>%s%d%s>\n",
		 1.0, 1.0, "foo", 77, "asdjffffffffffffffiiiiiiiiiiixxxxx"));

  RESULT(checkit ("<%4f><%.4f><%%><%4.4f>\n",
		  (void * []) {(void *)pi, (void *)pi, (void *)pi}));
  RESULT(printf ("<%4f><%.4f><%%><%4.4f>\n", M_PI, M_PI, M_PI));

  RESULT(checkit ("<%*f><%.*f><%%><%*.*f>\n",
		  (void * []) {(void *)3, (void *)pi, (void *)3, (void *)pi, (void *)3, (void *)3, (void *)pi}));
  RESULT(printf ("<%*f><%.*f><%%><%*.*f>\n", 3, M_PI, 3, M_PI, 3, 3, M_PI));

  RESULT(checkit ("<%d><%i><%o><%u><%x><%X><%c>\n",
		  (void * []) {(void *)75, (void *)75, (void *)75, (void *)75, (void *)75, (void *)75, (void *)75}));
  RESULT(printf ("<%d><%i><%o><%u><%x><%X><%c>\n",
		 75, 75, 75, 75, 75, 75, 75));

  RESULT(checkit ("Testing (hd) short: <%d><%ld><%hd><%hd><%d>\n",
                  (void * []) {(void *)123, (void *)(long)234, (void *)345, (void *)123456789, (void *)456}));
  RESULT(printf ("Testing (hd) short: <%d><%ld><%hd><%hd><%d>\n", 123, (long)234, 345, 123456789, 456));

#if defined(__GNUC__) || defined (HAVE_LONG_LONG)
  RESULT(checkit ("Testing (lld) long long: <%d><%lld><%d>\n", (void * [])
        {(void *)123, (void *)234234234234234234LL, (void *)345}));
  RESULT(printf ("Testing (lld) long long: <%d><%lld><%d>\n", 123, 234234234234234234LL, 345));
  RESULT(checkit ("Testing (Ld) long long: <%d><%Ld><%d>\n", (void * [])
        {(void *)123, (void *)234234234234234234LL, (void *)345}));
  RESULT(printf ("Testing (Ld) long long: <%d><%Ld><%d>\n", 123, 234234234234234234LL, 345));
#endif  /* HAVE_LONG_LONG */

#if defined(__GNUC__) || defined (HAVE_LONG_DOUBLE)
  RESULT(checkit ("Testing (Lf) long double: <%.20f><%.20Lf><%0+#.20f>\n",
		 (void * []) {(void *)seq_short, (void *)seq_long, (void *)seq_short}));
  RESULT(printf ("Testing (Lf) long double: <%.20f><%.20Lf><%0+#.20f>\n",
		 1.23456, 1.234567890123456789L, 1.23456));
#endif  /* HAVE_LONG_DOUBLE */
#ifdef HIDE_FOR_NOW
#endif  /* HIDE_FOR_NOW */

  return 0;
}
#endif /* TEST */
/* vim: tabstop=8 shiftwidth=2 expandtab softtabstop=2
 */
