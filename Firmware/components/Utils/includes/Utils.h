/**
 * @file Utils.h
 * @author Gustice
 * @brief Generic helper and defines
 * @version 0.1
 * @date 2021-03-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#define SET_BIT(PORT, BIT)  ((PORT) |= (1<<BIT))	// Set bit on word
#define CLR_BIT(PORT, BIT)  ((PORT) &= ~(1<<BIT))	// Clear bit on word
#define TGL_BIT(PORT, BIT)  ((PORT) ^= (1<<BIT))  // Toggle bit on word
#define CHK_BIT(PORT, BIT)  ((PORT>>BIT) & 1u)  // sample bit from port

 #define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

 #define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })


#define member_size(type, member) sizeof(((type *)0)->member) // sizeof structure-member