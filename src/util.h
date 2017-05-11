#ifndef _ZHYSH_UTIL_H_
#define _ZHYSH_UTIL_H_

#include <stdint.h>

/* function: int_to_string :: converts 32-bit integer to string
 *     args:             i :: integer to convert
 *                  buffer :: char array to place string 
 *   return: length of final string
 * */
uint32_t int_to_string(int32_t i, char buffer[]);

#endif
