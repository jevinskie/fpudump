/* fpu_dump.h -- x87 FPU state dumper
version 0.1, September 12th, 2007

Copyright (C) 2007 Jevin Sweval

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

Jevin Sweval (jevinsweval @t gmail d0t com)

*/

/*                   HOWTO USE
 *
 * Simply include this file and call FPUDUMP from anywhere in your program.
 * e.g.:
 *
 * #include "fpu_dump.h"
 * // some code
 * FPUDUMP;
 * // more code
 *
 * For more detailed output, uncomment the FPU_DUMP_VERBOSE
 *  define or define it in the compile line using '-DFPU_DUMP_VERBOSE'
 *
 * Credit goes to the excellent OllyDbg for the
 *  consise and readable output format
 */

#ifndef FPU_DUMP_H_FILE
#define FPU_DUMP_H_FILE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*#define FPU_DUMP_VERBOSE*/

/* Call this from anywhere in your code (e.g. 'FPU_DUMP;') */
/* Copies the FPU state into the struct, prints it, and restores it */
#define FPU_DUMP                                         \
   __asm__ volatile ("fsave %[fpu]\n\t"                  \
                     : [fpu] "=m"(fpu.orig)              \
                     : );                                \
   fpu_dump(__FILE__, __LINE__);                         \
   __asm__ volatile ("frstor %[fpu]\n\t"                 \
                     :                                   \
                     : [fpu] "m"(fpu.orig));

/* struct for the FPU environment dumped by fsave */
typedef struct {
   unsigned short cont;
   unsigned short padding1;
   unsigned short stat;
   unsigned short padding2;
   unsigned short tag;
   unsigned short padding3;
   unsigned int   instr_off;
   unsigned short instr_sel;
   unsigned short opcode;
   unsigned int   op_off;
   unsigned short op_sel;
   unsigned short padding4;
   unsigned short regs[8][5];
} FPU_ENV;

/* struct for the parsed control word */
typedef struct {
   char           inv_mask;
   char           denorm_mask;
   char           zero_mask;
   char           over_mask;
   char           under_mask;
   char           prec_mask;
   short          prec;
   char           rnd[5];
   char           inf;
} FPU_CONTROL;

/* struct for the parsed status word */
typedef struct {
   char           inv;
   char           denorm;
   char           zero;
   char           over;
   char           under;
   char           prec;
   char           sf;
   char           error;
   char           c0;
   char           c1;
   unsigned short top;
   char           c2;
   char           c3;
   char           busy;
} FPU_STATUS;

/* struct that contains the environment and parsed environment */
typedef struct {
   FPU_ENV        env;
   FPU_ENV        orig;
   FPU_CONTROL    cont;
   FPU_STATUS     stat;
   char           tags[8][7];
   long double    regs[8];
} FPU;

FPU fpu;
int fpu_i;

void fpu_dump(char*, int);

void fpu_dump(char* file, int line) {
   /* Be extra careful to not modify the status */
   fpu.env = fpu.orig;
   
   /* Parse the control word */
   fpu.cont.inv_mask =     (fpu.env.cont >> 0) & 0x1;
   fpu.cont.denorm_mask =  (fpu.env.cont >> 1) & 0x1;
   fpu.cont.zero_mask =    (fpu.env.cont >> 2) & 0x1;
   fpu.cont.over_mask =    (fpu.env.cont >> 3) & 0x1;
   fpu.cont.under_mask =   (fpu.env.cont >> 4) & 0x1;
   fpu.cont.prec_mask =    (fpu.env.cont >> 5) & 0x1;
   /* Parse the precision bits */
   switch ((fpu.env.cont >> 8) & 0x3) {
      case 0x0:
         fpu.cont.prec = 24;
         break;
      case 0x1: /* reserved */
         fpu.cont.prec = -1;
         break;
      case 0x2:
         fpu.cont.prec = 53;
         break;
      case 0x3:
         fpu.cont.prec = 64;
         break;
   }
   /* Parse the rounding bits */
   switch ((fpu.env.cont >> 10) & 0x3) {
      case 0x0:
         strcpy(fpu.cont.rnd, "NEAR");
         break;
      case 0x1:
         strcpy(fpu.cont.rnd, "DOWN");
         break;
      case 0x2:
         strcpy(fpu.cont.rnd, "UP");
         break;
      case 0x3:
         strcpy(fpu.cont.rnd, "ZERO");
         break;
   }
   fpu.cont.inf =          (fpu.env.cont >> 12) & 0x1;
   
   /* Parse the status word */
   fpu.stat.inv =    (fpu.env.stat >> 0) & 0x1;
   fpu.stat.denorm = (fpu.env.stat >> 1) & 0x1;
   fpu.stat.zero =   (fpu.env.stat >> 2) & 0x1;
   fpu.stat.over =   (fpu.env.stat >> 3) & 0x1;
   fpu.stat.under =  (fpu.env.stat >> 4) & 0x1;
   fpu.stat.prec =   (fpu.env.stat >> 5) & 0x1;
   fpu.stat.sf =     (fpu.env.stat >> 6) & 0x1;
   fpu.stat.error =  (fpu.env.stat >> 7) & 0x1;
   fpu.stat.c0 =     (fpu.env.stat >> 8) & 0x1;
   fpu.stat.c1 =     (fpu.env.stat >> 9) & 0x1;
   fpu.stat.c2 =     (fpu.env.stat >> 10) & 0x1;
   fpu.stat.top =    (fpu.env.stat >> 11) & 0x7;
   fpu.stat.c3 =     (fpu.env.stat >> 14) & 0x1;
   fpu.stat.busy =   (fpu.env.stat >> 15) & 0x1;
   
   /* Parse the tags for each data register */
   for (fpu_i = 0; fpu_i < 8; fpu_i++) {
      switch ((fpu.env.tag >> (2 * fpu_i)) & 0x3) {
         case 0x0:
            strcpy(fpu.tags[fpu_i], "VALID");
            break;
         case 0x1:
            strcpy(fpu.tags[fpu_i], "ZERO");
            break;
         case 0x2:
            strcpy(fpu.tags[fpu_i], "SPEC");
            break;
         case 0x3:
            strcpy(fpu.tags[fpu_i], "EMPTY");
            break;
      }
   }
   
   /* Convert from FPU's 10 byte long doubles to GCC's 12 bytes */
   for(fpu_i = 0; fpu_i < 8; fpu_i++) {
      *((short*)&fpu.regs[fpu_i] + 0) = fpu.env.regs[fpu_i][0];
      *((short*)&fpu.regs[fpu_i] + 1) = fpu.env.regs[fpu_i][1];
      *((short*)&fpu.regs[fpu_i] + 2) = fpu.env.regs[fpu_i][2];
      *((short*)&fpu.regs[fpu_i] + 3) = fpu.env.regs[fpu_i][3];
      *((short*)&fpu.regs[fpu_i] + 4) = fpu.env.regs[fpu_i][4];
      *((short*)&fpu.regs[fpu_i] + 5) = 0x0000;
   }
   
   printf("\n\n================== MAGICAL FPU INSPECTOR ==================\n");
   printf("From %s line %d:\n", file, line);
   
#ifdef FPU_DUMP_VERBOSE
   /* Detailed status register */
   printf("\nStatus: 0x%04X\n", fpu.env.stat);
   printf("   Invalid: %d, Denorm: %d, Zero Div: %d\n", fpu.stat.inv, fpu.stat.denorm, fpu.stat.zero);
   printf("   Overflow: %d, Underflow: %d, Precision: %d\n", fpu.stat.over, fpu.stat.under, fpu.stat.prec);
   printf("   Stack fault: %d, Error summary: %d, TOP: %d\n", fpu.stat.sf, fpu.stat.error, fpu.stat.top);
   printf("   C3: %d, C2: %d, C1: %d, C0: %d\n", fpu.stat.c3, fpu.stat.c2, fpu.stat.c1, fpu.stat.c0);
   
   /* Detailed control register */
   printf("\nControl: 0x%04X\n", fpu.env.cont);
   printf("   Invalid Mask: %d, Denorm Mask: %d, Zero Div Mask: %d\n", fpu.cont.inv_mask, fpu.cont.denorm_mask, fpu.cont.zero_mask);
   printf("   Overflow Mask: %d, Underflow Mask: %d, Precision Mask: %d\n", fpu.cont.over_mask, fpu.cont.under_mask, fpu.cont.prec_mask);
   printf("   Rounding: %4s, Precision: %d, Infinity: %d\n", fpu.cont.rnd, fpu.cont.prec, fpu.cont.inf);
#endif
   
   /* A compact summary */
   printf("\n                  3 2 1 0      E S P U O Z D I\n");
   /* Compact status word */
   printf("   FST %04X  Cond %d %d %d %d  Err %d %d %d %d %d %d %d %d  ",
          fpu.env.stat, fpu.stat.c3, fpu.stat.c2, fpu.stat.c1, fpu.stat.c0, fpu.stat.error, fpu.stat.sf,
          fpu.stat.prec, fpu.stat.under, fpu.stat.over, fpu.stat.zero, fpu.stat.denorm, fpu.stat.inv);
   if (fpu.stat.c3 && fpu.stat.c2 && fpu.stat.c0) { /* print the result of FCOM(P)(P) */
      printf("(Unordered)\n");
   } else if (!fpu.stat.c3 && !fpu.stat.c2 && !fpu.stat.c0) {
      printf("(GT)\n");
   } else if (!fpu.stat.c3 && !fpu.stat.c2 && fpu.stat.c0) {
      printf("(LT)\n");
   } else if (fpu.stat.c3 && !fpu.stat.c2 && !fpu.stat.c0) {
      printf("(EQ)\n");
   } else {
      printf("\n");
   }
   
   /* Compact control word */
   printf("   FCW %04X  Prec %s,%d  Mask    %d %d %d %d %d %d\n\n",
          fpu.env.cont, fpu.cont.rnd, fpu.cont.prec, fpu.cont.prec_mask, fpu.cont.under_mask,
          fpu.cont.over_mask, fpu.cont.zero_mask, fpu.cont.denorm_mask, fpu.cont.inv_mask);
   
   /* Print out all the data registers in stack order */
   for(fpu_i = 0; fpu_i < 8; fpu_i++) {
      /* Stack, register, value, tag */
      printf("ST%d %6s %26.20G   REG%d\n", fpu_i, fpu.tags[(fpu_i + fpu.stat.top) % 8],
             (double)fpu.regs[fpu_i], (fpu_i + fpu.stat.top) % 8);
      /* Hex dump, left->right MSB->LSB */
      /* Format is: exponent mantissa-hi mantissa-low */ 
      printf("               %04hX %08X %08X\n\n", *((short*)&fpu.env.regs[fpu_i] + 4),
             *((int*)(((short*)&fpu.env.regs[fpu_i] + 2))),
             *((int*)(((short*)&fpu.env.regs[fpu_i] + 0))));
   }
   
   printf("========================= GOODBYE =========================\n\n\n");
   
   return;
}

#endif
