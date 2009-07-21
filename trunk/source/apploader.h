#ifndef _APPLOADER_H_
#define _APPLOADER_H_

/* Entry point */
typedef void (*entry_point)(void);

/* Prototypes */
s32 Apploader_Run(entry_point *); 
void maindolpatches(void *dst, int len);
void PatchCountryStrings(void *Address, int Size);
void Anti_002_fix(void *Address, int Size);
bool Remove_001_Protection(void *Address, int Size);
u32 Load_Dol_from_sd(void);

/* Constants */
#define APPLDR_OFFSET	0x2440

#endif
