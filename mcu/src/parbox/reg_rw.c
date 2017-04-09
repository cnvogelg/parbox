#include "types.h"
#include "arch.h"
#include "reg_rw.h"

u16 reg_rw_get(u08 num)
{
  u08 max = read_rom_char(&reg_rw_table_size);
  if(num >= max) {
    /* invalid reg */
    return 0;
  } else {
    u08 flags = read_rom_char(&reg_rw_table[num].flags);
    rom_pchar ptr = read_rom_rom_ptr(&reg_rw_table[num].ptr);
    if(flags & REG_RW_FLAG_BYTE) {
        u08 *cptr = (u08 *)ptr;
        return *cptr;
    } else {
        u16 *wptr = (u16 *)ptr;
        return *wptr;
    }
    return 0;
  }
}

u16 proto_api_get_rw_reg(u08 num) __attribute__ ((weak, alias("reg_rw_get")));

void reg_rw_set(u08 num, u16 val)
{
  u08 max = read_rom_char(&reg_rw_table_size);
  if(num >= max) {
    /* invalid reg */
    return;
  } else {
    u08 flags = read_rom_char(&reg_rw_table[num].flags);
    if(flags & REG_RW_FLAG_FUNC) {
        /* validator function */
        rom_pchar ptr = read_rom_rom_ptr(&reg_rw_table[num].func);
        reg_rw_func_t func = (reg_rw_func_t)ptr;
        val = func(val);
    }
    /* set value */
    rom_pchar ptr = read_rom_rom_ptr(&reg_rw_table[num].ptr);
    if(flags & REG_RW_FLAG_BYTE) {
        u08 *cptr = (u08 *)ptr;
        *cptr = (u08)val;
    } else {
        u16 *wptr = (u16 *)ptr;
        *wptr = val;
    }
  }
}

void proto_api_set_rw_reg(u08 num, u16 val) __attribute__ ((weak, alias("reg_rw_set")));