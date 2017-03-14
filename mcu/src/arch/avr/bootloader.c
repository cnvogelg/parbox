#include <avr/wdt.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>

#include "autoconf.h"
#include "types.h"
#include "arch.h"

#include "bootloader.h"
#include "uart.h"
#include "proto.h"
#include "proto_low.h"
#include "flash.h"

static u08 status;
static u16 page_addr;
static u08 page_buf[SPM_PAGESIZE];

// ro registers
const u16 reg_ro_table[] ROM_ATTR = {
  0x8000 | VERSION_TAG,                           /* 0: bootloader version */
  MACH_TAG,                                       /* 1: mach tag */
  SPM_PAGESIZE,                                   /* 2: page size */
  CONFIG_MAX_ROM                                  /* 3: rom size */
};

u08 reg_ro_size(void)
{
  return sizeof(reg_ro_table)/2;
}

// from optiboot
static void run_app(u08 rstFlags) __attribute__ ((naked));
static void run_app(u08 rstFlags)
{
  // store reset reason
  __asm__ __volatile__ ("mov r2, %0\n" :: "r" (rstFlags));

  // disable watchdog
  MCUSR = 0;
  wdt_disable();

  // jump to app
  __asm__ __volatile__ (
    "clr r30\n"
    "clr r31\n"
    "ijmp\n"::
  );
}

// remove irq vector table
int main(void) __attribute__ ((OS_main)) __attribute__ ((section (".vectors")));
int main(void)
{
  // raw init
  __asm__ __volatile__ ("clr __zero_reg__");
#if defined(__AVR_ATmega8__) || defined (__AVR_ATmega32__) || defined (__AVR_ATmega16__)
  SP=RAMEND;  // This is done by hardware reset
#endif

  // get reset reason
  u08 rst_flag;
#ifdef MCUSR
  rst_flag = MCUSR;
  MCUSR = 0;
#elif MCUCSR
  rst_flag = MCUCSR;
  MCUCSR = 0;
#else
#error unknown avr
#endif

  // watchdog init
  wdt_enable(WDTO_500MS);

  // say hello
  uart_init();
  uart_send('P');

  // setup proto
  proto_init();
  uart_send('A');

  // check if bootloader command is set - if not enter app
  u08 cmd = proto_low_get_cmd();
  if(cmd != CMD_BOOTLOADER) {
    // check crc
    uart_send('B');
    u16 crc = flash_check_crc();
    if(crc == 0) {
      uart_send('L');
      // ensure that mach_tag matches in pablo footer
      u16 rom_mach_tag = pgm_read_word(CONFIG_MAX_ROM-4);
      if(rom_mach_tag == MACH_TAG) {
        uart_send('O');
        // run app if valid
        run_app(rst_flag);
      }
    }
  }
  else {
    // reply to bootloader command
    uart_send('-');
    proto_low_no_value();
  }

  // enter main loop
  uart_send(':');
  while(1) {
    wdt_reset();
    proto_handle();
  }
}

// registers

void proto_api_set_rw_reg(u08 reg, u16 val)
{
  if(reg == 0) {
    page_addr = val;
  }
}

u16  proto_api_get_rw_reg(u08 reg)
{
  if(reg == 0) {
    return page_addr;
  } else {
    return 0;
  }
}

// msg i/o is used to transfer page data - channel is ignored in bootloader

u08 *proto_api_prepare_read_msg(u08 chan, u16 *size)
{
  *size = SPM_PAGESIZE;
  uart_send('r');
  flash_read_page(page_addr, page_buf);
  return page_buf;
}

void proto_api_done_read_msg(u08 chan)
{
  uart_send('.');
}

u08 *proto_api_prepare_write_msg(u08 chan, u16 *max_size)
{
  *max_size = SPM_PAGESIZE;
  uart_send('w');
  return page_buf;
}

void proto_api_done_write_msg(u08 chan, u16 size)
{
  if(size == SPM_PAGESIZE) {
    uart_send('(');
    flash_program_page(page_addr, page_buf);
    uart_send(')');
    status = BOOT_STATUS_OK;
  } else {
    uart_send('?');
    status = BOOT_STATUS_INVALID_PAGE_SIZE;
  }
}
