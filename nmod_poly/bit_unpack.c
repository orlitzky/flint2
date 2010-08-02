/*============================================================================

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

===============================================================================*/
/****************************************************************************

   Copyright (C) 2007 David Howden
   Copyright (C) 2010 William Hart
   
*****************************************************************************/

#include <stdlib.h>
#include <mpir.h>
#include "flint.h"
#include "nmod_vec.h"
#include "nmod_poly.h"

// assumes length > 0, bits > 0
void _nmod_poly_bit_unpack(mp_ptr res, mp_srcptr mpn, long length, mp_bitcnt_t bits, nmod_t mod)
{
   long i;
   ulong current_limb = 0;
   ulong current_bit = 0;

   mp_limb_t temp_lower;
   mp_limb_t temp_upper;
   mp_limb_t temp_upper2;
      
   mp_limb_t mask;

   if (bits < FLINT_BITS)
   {
      ulong boundary_limit_bit = FLINT_BITS - bits;

	  mask = (1L<<bits) - 1L;
      
      for (i = 0; i < length; i++)
      {
          if (current_bit > boundary_limit_bit)
          {
             temp_lower = mpn[current_limb++] >> current_bit;
             temp_upper = mpn[current_limb] << (FLINT_BITS - current_bit);
             
			 temp_upper |= temp_lower;
             temp_upper &= mask;
             
			 NMOD_RED(res[i], temp_upper, mod);
            
             current_bit += bits - FLINT_BITS;
          }
          else
          {
             /* the coeff will fit in the current limb */
             temp_upper = (mpn[current_limb] >> current_bit) & mask;
             
             NMOD_RED(res[i], temp_upper, mod);
            
             current_bit += bits;

			 if (current_bit == FLINT_BITS)
             {
                current_bit = 0;
                current_limb++;
             }
          }    
      }
   } else if (bits == FLINT_BITS)
   {
      for (i = 0; i < length; i++)
         NMOD_RED(res[i], mpn[i], mod);
   } else if (bits == 2*FLINT_BITS)
   {
      for (i = 0; i < length; i++)
      {
         NMOD2_RED2(res[i], mpn[current_limb + 1], mpn[current_limb], mod);
         current_limb += 2;
      }
   } else if (bits < 2*FLINT_BITS) /* FLINT_BITS < bits < 2*FLINT_BITS */
   {
      ulong double_boundary_limit_bit = 2*FLINT_BITS - bits;

	  mask = (1L<<(bits - FLINT_BITS)) - 1L;

      for (i = 0; i < length; i++)
      {
         if (current_bit == 0)
		 {       
			temp_lower = mpn[current_limb++];
            temp_upper = mpn[current_limb] & mask;
            
			NMOD2_RED2(res[i], temp_upper, temp_lower, mod);
            
			current_bit = bits - FLINT_BITS;
 		 } else if (current_bit > double_boundary_limit_bit)
         {
            /* the coeff will be across two limb boundaries */
            temp_lower = mpn[current_limb++] >> current_bit;
            temp_lower |= (mpn[current_limb] << (FLINT_BITS - current_bit));
            
			temp_upper = mpn[current_limb++] >> current_bit; 
			temp_upper |= (mpn[current_limb] << (FLINT_BITS - current_bit));
            temp_upper &= mask;
            
			NMOD2_RED2(res[i], temp_upper, temp_lower, mod);
            
			current_bit += bits - 2*FLINT_BITS;    
        } else 
        {
            /* the coeff will be across one limb boundary */
            temp_lower = (mpn[current_limb] >> current_bit) | (mpn[current_limb + 1] << (FLINT_BITS - current_bit));
            current_limb++;
            
            temp_upper = mpn[current_limb] >> current_bit;
            temp_upper &= mask;

            NMOD2_RED2(res[i], temp_upper, temp_lower, mod);
            
			current_bit += bits - FLINT_BITS;           
			if (current_bit == FLINT_BITS)
            {
               current_bit = 0; 
               current_limb++;  
            } 
         }
      }
   } else /* 2*FLINT_BITS < bits < 3*FLINT_BITS */
   {
      ulong double_boundary_limit_bit = 3*FLINT_BITS - bits;

	  mask = (1L<<(bits - 2*FLINT_BITS)) - 1L;

      for (i = 0; i < length; i++)
      {
         if (current_bit == 0)
         {
            temp_lower = mpn[current_limb++];
            temp_upper = mpn[current_limb++];
            temp_upper2 = mpn[current_limb] & mask;
			
			NMOD_RED3(res[i], temp_upper2, temp_upper, temp_lower, mod);
			
			current_bit = bits - 2*FLINT_BITS;
         } else if (current_bit <= double_boundary_limit_bit)
         {
            /* the coeff will be across two limb boundaries */
            temp_lower = mpn[current_limb++] >> current_bit;
            temp_lower |= (mpn[current_limb] << (FLINT_BITS - current_bit));
            
            temp_upper = mpn[current_limb++] >> current_bit;
            temp_upper |= (mpn[current_limb] << (FLINT_BITS - current_bit));
            
			temp_upper2 = mpn[current_limb] >> current_bit;
            temp_upper2 &= mask;

			NMOD_RED3(res[i], temp_upper2, temp_upper, temp_lower, mod);

			current_bit += bits - 2*FLINT_BITS;
			if (current_bit == FLINT_BITS)
            {
               current_bit = 0; 
               current_limb++;  
            }     
         } else 
         {
            /* the coeff will be across three limb boundaries */
            temp_lower = mpn[current_limb++] >> current_bit;
            temp_lower |= (mpn[current_limb] << (FLINT_BITS - current_bit));
            
            temp_upper = mpn[current_limb++] >> current_bit;
            temp_upper |= (mpn[current_limb] << (FLINT_BITS - current_bit));
            
			temp_upper2 = mpn[current_limb++] >> current_bit;
            temp_upper2 |= (mpn[current_limb] << (FLINT_BITS - current_bit));
            
			temp_upper2 &= mask;
            
			NMOD_RED3(res[i], temp_upper2, temp_upper, temp_lower, mod);

			current_bit += bits - 3*FLINT_BITS;
        }
      }
   }
}
