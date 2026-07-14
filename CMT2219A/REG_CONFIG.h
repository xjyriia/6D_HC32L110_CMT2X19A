#ifndef __REG_CONFIG_H
#define __REG_CONFIG_H

#include <stdint.h>

/* The CMT2219A config registers length, dont't chang it */
#define CMT2219_FTP8_LENGTH			(uint16_t)(0x3E)			// The byte lendth
#define CMT2219_FTP16_LENGTH		(uint16_t)(0x3E/2)		// The word length


void Cmt2219a_UpdateConfigs(void);
void Cmt2219a_UpdateRegisters(const uint16_t *buf16);

#endif

