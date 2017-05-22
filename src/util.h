#ifndef _ZHSH_UTIL_H_
#define _ZHSH_UTIL_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define unless(exp) if (!(exp))

/* function: str_is_num :: check that string contains only number
 *     args:        str :: string to check
 *   return: 1 if true, otherwise 0
 */
int str_is_num(char* str);


/* function: int_to_string :: converts 32-bit integer to string
 *     args:             i :: integer to convert
 *                  buffer :: char array to place string 
 *   return: length of final string
 * */
uint32_t int_to_string(int32_t i, char buffer[]);

/* function: strdup :: duplicate a string
 *     args: string :: pointer to a string to duplicate
 *   return: pointer to to a new string which is a duplicate of 
 *           the spicified string
 * */
char* strdup(const char* string);

#endif
