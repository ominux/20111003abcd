/*
 *
 *   Module      : $Source: /homes/gus/research/misc/src/RCS/random.C,v $
 *   LastCheckin : $Date: 1995/12/26 18:49:24 $
 *   Description : Random number generator class
 *   Author      : $Author: gus $
 *   LastModBy   : $Locker:  $
 *   Copyright   : Northwestern University
 *
 *   $Log: random.C,v $
 * Revision 1.5  1995/12/26  18:49:24  gus
 * Added state saving code
 *
 * Revision 1.4  1995/03/20  17:30:45  gus
 * Added mystring.h for filein.h
 *
 * Revision 1.3  1995/02/10  17:30:57  gus
 * Added new random number generator. Code is more
 * portable now.
 *
 * Revision 1.2  1994/12/02  17:53:59  gus
 * Latest
 *
 * Revision 1.1  1994/09/16  20:09:38  gus
 * Initial revision
 *
 *
 */

static char rcsid[] = "$Header: /homes/gus/research/misc/src/RCS/random.C,v 1.5 1995/12/26 18:49:24 gus Exp $";

 /* Marsaglia generator */
 /*
  * C This random number generator originally appeared in "Toward a Universal
  * C Random Number Generator" by George Marsaglia and Arif Zaman. C Florida
  * State University Report: FSU-SCRI-87-50 (1987)
  * C
  * C It was later modified by F. James and published in "A Review of Pseudo-
  * C random Number Generators"
  * C
  * C THIS IS THE BEST KNOWN RANDOM NUMBER GENERATOR AVAILABLE.
  * C       (However, a newly discovered technique can yield
  * C         a period of 10^600. But that is still in the development stage.)
  * C
  * C It passes ALL of the tests for random number generators and has a period
  * C of 2^144, is completely portable (gives bit identical results on all
  * C machines with at least 24-bit mantissas in the floating point
  * C representation).
  * C
  * C The algorithm is a combination of a Fibonacci sequence (with lags of 97
  * C   and 33, and operation "subtraction plus one, modulo one") and an
  * C   "arithmetic sequence" (using subtraction).
  * C========================================================================
  * This version is based on a C version by Jim Butler, which is itself based
  * on a FORTRAN program posted by David LaSalle of Florida State University.
  *
  */

/* NO_NUMBERS is upper bound of lrand48, 2 to the 24th power */
const long NO_NUMBERS = 0x1000000;
const double CONV_REAL = (double) NO_NUMBERS;
const long CD = 7654321;
const long CM = NO_NUMBERS-3;

const long FIRSTDIM = 64;
const long LASTDIM = 97;

static long x[LASTDIM], *j_ptr, *k_ptr, c;

void my_srand48(unsigned long seed)
{
   int ij, kl;                  /* seeds for Marsaglia generator */

   ij = (int) (seed % 31328);
   kl = (int) ((seed >> 16) % 30081);

   while (ij<0) ij+=31328;
   while (kl<0) kl+=30081;

   int i, j, k, l, m, ii, jj;
   long ss, tt;
/*
 *        compute i, j, k and l from ij and kl
 *       2 <= i < 179, 2 <= j < 179, 1 <= k < 179, 0 <= l < 169
 */
   i = ij / 177 % 177 + 2;
   j = ij % 177 + 2;
   k = kl / 169 % 178 + 1;
   l = kl % 169;

/* ii cycles through all elements of x, the random look-up table */

   for (ii = LASTDIM; ii--; ) {

/*
 *       initialize ss (the bit accumulator)
 *       initialize tt (the single-bit mask)
 *       jj cycles through 24 bits
 */
       for (ss = 0, tt = NO_NUMBERS, jj = 1; jj <= 24; jj++) {

               m = i * j % 179 * k % 179;  /* 0 <= m < 179 */

/* shift j, k and m into i, j and k */

               i = j;
               j = k;
               k = m;

               l = (53 * l + 1) % 169; /* 0 <= l < 169 */

               tt >>= 1; /* tt shifts to next bit */

/* accumulate tt into ss, about half the time */

               if (l * m & 32) ss |= tt;
       }

/* put ss into look-up table */

       x[ii] = ss;
//       printf(" %i. %li \n", ii, ss);
   }

/* assign remaining globals: j_ptr, k_ptr and c */

   j_ptr = x;
   k_ptr = x+FIRSTDIM;
   c = 362436;

}

long my_lrand48(void)
{
        long uni;

/* uni is difference between two table values */

        uni = *j_ptr - *k_ptr;

/* make sure 0<=uni<NO_NUMBERS */

        if (uni < 0) uni += NO_NUMBERS;

/* assign uni into look-up table */
/* old *j_ptr = (new *j_ptr + *k_ptr) modulus NO_NUMBERS */

        *j_ptr = uni;

/* point j_ptr and k_ptr at new look-up table values */

        if ( ++j_ptr >= x + LASTDIM) j_ptr = x;
        if ( ++k_ptr >= x + LASTDIM) k_ptr = x;

/* modify c; 0<=c<NO_NUMBERS-CD or CM-CD<=c<CM */
/* there are three overlap positions between CM-CD and NO_NUMBERS-CD,
         and three unavailable positions between CM and NO_NUMBERS */

        if ( (c -= CD) < 0) c += CM;

/* modify uni, make sure 0<=uni<NO_NUMBERS */

        if ( (uni -= c) < 0) uni += NO_NUMBERS;

        return uni;
}




