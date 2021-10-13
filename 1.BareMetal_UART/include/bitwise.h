/** 
  ******************************************************************************
  * @file    bitwise.h
  * @author  Camilo Vera
  * @brief   Bitwise operations macros.
*/

#ifndef BITWISE_H
#define BITWISE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Modifiers -----------------------------------------------------------------*/
#define SET_BIT(x, pos)  (x |= (1U << pos))           // Set pos bit
#define CLEAR_BIT(x, pos) (x &= (~(1U<< pos)))        // Clear pos bit
#define TOGGLE_BIT(x, pos) x ^= (1U<< pos)            // Toggle pos bit

/* Readers -------------------------------------------------------------------*/

#define MASK_DATA(x, mask) (x & mask)                 // Get masked data

#define VF_L(pos) (pos < 32U) ? pos : 31U             // verify length
#define GET_MASK(pos) 1U << VF_L(pos)                 

#define GET_BIT(x, pos) ((x & GET_MASK(pos)) >> pos)  // Get pos bit
#define GET_BITS(x, pos, mask) ((x & mask) >> pos)    // Get pos bits 

/* Casters -------------------------------------------------------------------*/


#define FIRST_BYTE(x)  ((uint8_t)x)
#define SECOND_BYTE(x) (uint8_t)(((uint16_t)x) >> 8)
#define THIRD_BYTE(x)  (uint8_t)(((uint32_t)x) >> 16)
#define FOURTH_BYTE(x) (uint8_t)(((uint32_t)x) >> 24)

#define LOW_BYTE(x) ((uint8_t)x)
#define HIGH_BYTE(x) ((uint8_t) (((uint16_t)x) >> 8)) 


#ifdef __cplusplus
}
#endif

#endif /* BITWISE_H */

/************************ Camilo Vera **************************END OF FILE****/