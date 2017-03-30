#ifndef ARCH_H
#define ARCH_H

#include <avr/pgmspace.h>

typedef PGM_P rom_pchar;
typedef char *ram_pchar;

#define read_rom_char(x)       pgm_read_byte_near(x)
#define read_rom_word(x)       pgm_read_word_near(x)

#define read_rom_rom_ptr(x)    ((rom_pchar)pgm_read_word_near(x))
#define read_rom_ram_ptr(x)    ((ram_pchar)pgm_read_word_near(x))

#define ROM_ATTR __ATTR_PROGMEM__

#endif
