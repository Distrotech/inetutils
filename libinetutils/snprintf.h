/* 
Alain Magloire: alainm@rcsm.ee.mcgill.ca

    IMPORTANT: this is just a SIMPLE version of snprintf for
    for the systems that do not provide one
    it understands:
      Integer:
        %lu %lu %u
        %hd %ld %d     decimal
        %ho %lo %o     octal
        %hx %lx %x %X  hexa
      Floating points:
        %g %G %e %E %f  double
      Strings:
        %s %c  string
      %%   %

    Formating conversion flags:
      - justify left
      + Justify right or put a plus if number
      # prefix 0x, 0X for hexa and 0 for octal
      * precision/witdth is specify as an (int) in the arguments
    ' ' leave a blank for number with no sign
      l the later should be a long
      h the later should be a short

format:
  snprintf(holder, sizeof_holder, format, ...)

Return values:
  (sizeof_holder - 1)

bugs:
 - not complying fully to ANSI std :-)
 - %#g or %#G will not cut the trailing zeros
*/

#if defined(HAVE_STDARG_H) && defined(__STDC__) && __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <stdlib.h>    /* for atoi() */
#include <ctype.h>


/* 
 * For the FLOATING POINT FORMAT :
 *  the challenge was finding a way to
 *  manipulate the Real numbers without having
 *  to resort to mathematical function(it
 *  would require to link with -lm) and not
 *  going down to the bit pattern(not portable)
 *
 *  so a number, a real is:

      real = integral + fraction

      integral = ... + a(2)*10^2 + a(1)*10^1 + a(0)*10^0
      fraction = b(1)*10^-1 + b(2)*10^-2 + ...

      where:
       0 <= a(i) => 9 
       0 <= b(i) => 9 
 
    from then it was simple math
 */

/*
 * size of the buffer for the integral part
 * and the fraction part 
 */
#define MAX_INT  99 + 1 /* 1 for the null */
#define MAX_FRACT 29 + 1

/* 
 * numtoa() uses PRIVATE buffers to store the results,
 * you've been warn
 */
#define itoa(n) numtoa(n, 10, (char **)0) 
#define otoa(n) numtoa(n, 8, (char **)0)
#define htoa(n) numtoa(n, 16, (char **)0)
#define dtoa(n, f) numtoa(n, 10, f)

#define SWAP_INT(a,b) {int t; t = (a); (a) = (b); (b) = t;}

#define RIGHT 1
#define LEFT  0
#define NOT_FOUND -1
#define FOUND 1
#define MAX_FIELD 15

/* the conversion flags */
#define isflag(c) ((c) == '#' || (c) == ' ' || \
                   (c) == '*' || (c) == '+' || \
                   (c) == '-' || (c) == '.' || \
                   isdigit(c))

/* round off to the precision */
#define ROUND(d, p) \
            (d < 0.) ? \
             d - pow_10(-(p)->precision) * 0.5 : \
             d + pow_10(-(p)->precision) * 0.5

/* set default precision */
#define DEF_PREC(p) \
            if ((p)->precision == NOT_FOUND) \
              (p)->precision = 6

/* put a char */
#define PUT_CHAR(c, p) \
            if ((p)->counter < (p)->length) { \
              *(p)->holder++ = (c); \
              (p)->counter++; \
            }

#define PUT_PLUS(d, p) \
            if ((d) > 0. && (p)->justify == RIGHT) \
              PUT_CHAR('+', p)

#define PUT_SPACE(d, p) \
            if ((p)->space == FOUND && (d) > 0.) \
              PUT_CHAR(' ', p)

/* pad right */ 
#define PAD_RIGHT(p) \
            if ((p)->width > 0 && (p)->justify != LEFT) \
              for (; (p)->width > 0; (p)->width--) \
                 PUT_CHAR((p)->pad, p)

/* pad left */
#define PAD_LEFT(p) \
            if ((p)->width > 0 && (p)->justify == LEFT) \
              for (; (p)->width > 0; (p)->width--) \
                 PUT_CHAR((p)->pad, p)

/* if width and prec. in the args */
#define STAR_ARGS(p) \
            if ((p)->star_w == FOUND) \
              (p)->width = va_arg(args, int); \
            if ((p)->star_p == FOUND) \
              (p)->precision = va_arg(args, int)

/* this struct holds everything we need */
struct DATA {
  int length;
  char *holder;
  int counter;
  const char *pf;
/* FLAGS */
  int width, precision;
  int justify; char pad;
  int square, space, star_w, star_p, a_long ;
};

#define PRIVATE static
#define PUBLIC
/* signature of the functions */
#ifdef __STDC__
/* the floating point stuff */
  PRIVATE double pow_10(int);
  PRIVATE int log_10(double);
  PRIVATE double integral(double, double *);
  PRIVATE char * numtoa(double, int, char **);

/* for the format */
  PRIVATE void conv_flag(char *, struct DATA *);
  PRIVATE void floating(struct DATA *, double);
  PRIVATE void exponent(struct DATA *, double);
  PRIVATE void decimal(struct DATA *, double);
  PRIVATE void octal(struct DATA *, double);
  PRIVATE void hexa(struct DATA *, double);
  PRIVATE void strings(struct DATA *, char *);

#else
/* the floating point stuff */
  PRIVATE double pow_10();
  PRIVATE int log_10();
  PRIVATE double integral();
  PRIVATE char * numtoa();

/* for the format */
  PRIVATE void conv_flag();
  PRIVATE void floating();
  PRIVATE void exponent();
  PRIVATE void decimal();
  PRIVATE void octal();
  PRIVATE void hexa();
  PRIVATE void strings();
#endif

