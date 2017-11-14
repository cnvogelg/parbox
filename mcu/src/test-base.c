#include "autoconf.h"
#include "types.h"
#include "arch.h"

#define DEBUG 1

#include "debug.h"

#include "uart.h"
#include "uartutil.h"
#include "rominfo.h"
#include "system.h"
#include "led.h"
#include "timer.h"

#include "proto_low.h"

int main(void)
{
  system_init();
  led_init();

  uart_init();
  uart_send_pstring(PSTR("parbox: test-base!"));
  uart_send_crlf();

  rom_info();

  proto_low_init(0);

  u08 on = 1;
  while(1) {
    system_wdt_reset();

    timer_delay(100);

    on = !on;
    led_set(on);
  }

  return 0;
}
