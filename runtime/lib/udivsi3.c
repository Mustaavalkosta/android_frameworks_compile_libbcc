/* ===-- udivsi3.c - Implement __udivsi3 -----------------------------------===
 *
 *                     The LLVM Compiler Infrastructure
 *
 * This file is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 *
 * ===----------------------------------------------------------------------===
 *
 * This file implements __udivsi3 for the compiler_rt library.
 *
 * ===----------------------------------------------------------------------===
 */

#if !defined(__GNUC__) || __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 8) // gcc >= 4.8 implements this in libgcc
#include "int_lib.h"

/* Returns: a / b */

/* Translated from Figure 3-40 of The PowerPC Compiler Writer's Guide */

su_int
__udivsi3(su_int n, su_int d)
{
    const unsigned n_uword_bits = sizeof(su_int) * CHAR_BIT;
    su_int q;
    su_int r;
    unsigned sr;
    /* special cases */
    if (d == 0)
        return 0; /* ?! */
    if (n == 0)
        return 0;
    sr = __builtin_clz(d) - __builtin_clz(n);
    /* 0 <= sr <= n_uword_bits - 1 or sr large */
    if (sr > n_uword_bits - 1)  /* d > r */
        return 0;
    if (sr == n_uword_bits - 1)  /* d == 1 */
        return n;
    ++sr;
    /* 1 <= sr <= n_uword_bits - 1 */
    /* Not a special case */
    q = n << (n_uword_bits - sr);
    r = n >> sr;
    su_int carry = 0;
    for (; sr > 0; --sr)
    {
        /* r:q = ((r:q)  << 1) | carry */
        r = (r << 1) | (q >> (n_uword_bits - 1));
        q = (q << 1) | carry;
        /* carry = 0;
         * if (r.all >= d.all)
         * {
         *      r.all -= d.all;
         *      carry = 1;
         * }
         */
        const si_int s = (si_int)(d - r - 1) >> (n_uword_bits - 1);
        carry = s & 1;
        r -= d & s;
    }
    q = (q << 1) | carry;
    return q;
}
#endif
