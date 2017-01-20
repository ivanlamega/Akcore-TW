
#include <stdint.h>
#include "stdafx.h"
#include <iostream>
#include <stdint.h>
#include "dbo_random.h"

/***************************************************************/  
dbo_random::dbo_random()  
{  
   state = 0;  
   GenerateSeeds();  
}  
/***************************************************************/  
dbo_random::dbo_random(uint32_t seed)  
{  
   state = seed;  
   GenerateSeeds();  
}  
/***************************************************************/  
dbo_random::~dbo_random()  
{  
}  
/***************************************************************/  
uint32_t dbo_random::Generate()  
{  
    uint32_t retval = (table[index1] ^= table[index2]);   
  
    index1++;  
    if (index1 == TABLE_SIZE)  
       index1 = 0;  
  
    index2++;  
    if (index2 == TABLE_SIZE)  
       index2 = 0;  
  
    return retval;  
}  
/***************************************************************/  
void dbo_random::GenerateSeeds()  
{  
   int32_t n;  
   uint32_t msk, bit;  
     
   index1 = 0;  
   index2 = 103;  
   for (n = TABLE_SIZE - 1; n >= 0; n--)  
      table[n] = GenerateSimple();  
   for (n = 3, msk = 0xffffffff, bit = 0x80000000; bit; n += 7)   
   {  
      table[n] = (table[n] & msk) | bit;  
      msk >>= 1;  
      bit >>= 1;  
   }  
}  
/***************************************************************/  
uint32_t dbo_random::GenerateSimple()  
{  
   // IL_Shift32Random from Inner Loops book  
   uint32_t n, bit, temp;  
   temp = state;  
   for (n=0; n < 32; n++)  
   {  
      bit = ((temp >> 0) ^ (temp >> 1) ^ (temp >> 2) ^  
         (temp >> 3) ^ (temp >> 5) ^ (temp >> 7)) & 1;  
      temp = (((temp >> 1) | (temp << 31)) & ~1) | bit;  
   }  
   state = temp;  
   return state;  
}  


