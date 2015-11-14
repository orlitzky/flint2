/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Authored 2015 by Daniel S. Roche; US Government work in the public domain. 

******************************************************************************/

#include "flint.h"
#include "fmpz.h"

int main(void)
{
    int i;
    mp_bitcnt_t bits;
    FLINT_TEST_INIT(state);

    flint_printf("randprime....");
    fflush(stdout);

    for (bits=2; bits < 40; ++bits)
    {
        for (i=0; i < 1 * flint_test_multiplier(); ++i)
        {
            int res;
            fmpz_t p;

            fmpz_init(p);

            res = fmpz_randprime(p, state, bits, 1);
            
            if (res != 1)
            {
                flint_printf("FAIL: %wu bits, returned %d\n", bits, res);
                abort();
            }

            if (fmpz_bits(p) < bits)
            {
                flint_printf("FAIL: fewer than %wu bits\n", bits);
                fmpz_print(p); flint_printf("\n");
                abort();
            }

            if (fmpz_bits(p) > bits+1)
            {
                flint_printf("FAIL: more than %wu bits\n", bits);
                fmpz_print(p); flint_printf("\n");
                abort();
            }

            if (fmpz_is_prime(p) != 1)
            {
                flint_printf("FAIL: not prime, %wu bits\n", bits);
                fmpz_print(p); flint_printf("\n");
                abort();
            }

            fmpz_clear(p);
        }
    }

    for (; bits < 130; ++bits)
    {
        /* at this point, the chances of a collision are less than
         * one in a billion. So it should never happen.
         */
        
        for (i=0; i < 1 * flint_test_multiplier(); ++i)
        {
            int j, res;
            fmpz p[2];

            fmpz_init(p+0);
            fmpz_init(p+1);

            for (j=0; j<2; ++j)
            {
                res = fmpz_randprime(p+j, state, bits, 0);
                
                if (res != 1)
                {
                    flint_printf("FAIL: %wu bits, returned %d\n", bits, res);
                    abort();
                }

                if (fmpz_bits(p+j) < bits)
                {
                    flint_printf("FAIL: fewer than %wu bits\n", bits);
                    fmpz_print(p+j); flint_printf("\n");
                    abort();
                }

                if (fmpz_bits(p+j) > bits+1)
                {
                    flint_printf("FAIL: more than %wu bits\n", bits);
                    fmpz_print(p+j); flint_printf("\n");
                    abort();
                }

                if (fmpz_is_prime(p+j) == 0)
                {
                    flint_printf("FAIL: not prime, %wu bits\n", bits);
                    fmpz_print(p+j); flint_printf("\n");
                    abort();
                }
            }

            if (fmpz_equal(p+0, p+1))
            {
                flint_printf("FAIL: returned a duplicate\n");
                fmpz_print(p+0); flint_printf("\n");
                fmpz_print(p+1); flint_printf("\n");
                abort();
            }

            fmpz_clear(p+0);
            fmpz_clear(p+1);
        }
    }

    FLINT_TEST_CLEANUP(state);

    flint_printf("PASS\n");
    return 0;
}
