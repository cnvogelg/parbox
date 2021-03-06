/*
 * uartutil.c - serial utility routines
 *
 * Written by
 *  Christian Vogelgsang <chris@vogelgsang.org>
 *
 * This file is part of plipbox.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "autoconf.h"
#include "types.h"
#include "arch.h"

#include "uart.h"
#include "uartutil.h"

void uart_send_pstring(rom_pchar data)
{
  while(1) {
    u08 c = read_rom_char(data);
    if(c == 0) {
      break;
    }
    uart_send(c);
    data++;
  }
}

void uart_send_string(const char *str)
{
  while(*str) {
    uart_send((u08)*str);
    str++;
  }
}

void uart_send_data(u08 *data,u08 len)
{
  for(u08 i=0;i<len;i++) {
    uart_send(data[i]);
  }
}

void uart_send_crlf(void)
{
  uart_send(13);
  uart_send(10);
}

void uart_send_spc(void)
{
  uart_send((u08)' ');
}

static u08 nybble_to_hex(u08 in)
{
  if(in<10)
    return '0' + in;
  else
    return 'A' + in - 10;
}

void uart_send_hex_byte(u08 in)
{
  uart_send(nybble_to_hex(in >> 4));
  uart_send(nybble_to_hex(in & 0xf));
}

void uart_send_hex_word(u16 in)
{
  uart_send_hex_byte((u08)(in>>8));
  uart_send_hex_byte((u08)(in&0xff));
}

void uart_send_hex_long(u32 in)
{
  uart_send_hex_word((u16)(in>>16));
  uart_send_hex_word((u16)(in&0xffff));
}
