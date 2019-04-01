#include "types.h"
#include "autoconf.h"

#define DEBUG CONFIG_DEBUG_PROTO

#include "proto_low.h"
#include "proto.h"
#include "debug.h"
#include "system.h"

void proto_init(void)
{
  proto_low_init();
  DS("pinit"); DNL;
}

void proto_first(void)
{
  // expect reset command
  u08 cmd = proto_low_get_cmd();
  if(cmd != PROTO_CMD_ACTION_RESET && cmd != PROTO_CMD_ACTION_BOOTLOADER) {
    DS("wrong first:"); DB(cmd); DNL;
    system_sys_reset();
  }
  // ack action
  DS("first:"); DB(cmd); DNL;
  proto_low_action();
  proto_low_end();
  DS("done"); DNL;
}

static void handle_action(u08 num)
{
  DS("a:"); DB(num); DNL;

  // some actions need special handling:
  // immediate reset
  if(num == PROTO_ACTION_RESET || num == PROTO_ACTION_BOOTLOADER) {
    DS("a:RESET!"); DNL;
    system_sys_reset();
  }

  proto_low_action();
  proto_low_end();

  if(num != PROTO_ACTION_PING) {
    proto_api_action(num);
  }
}

static void handle_wfunc_read(u08 num)
{
  DS("wfr:"); DB(num); DNL;
  u16 val = proto_api_wfunc_read(num);
  proto_low_read_word(val);
  proto_low_end();
}

static void handle_wfunc_write(u08 num)
{
  DS("wfw:"); DB(num); DNL;
  u16 val = proto_low_write_word();
  proto_low_end();
  proto_api_wfunc_write(num, val);
}

static void handle_lfunc_read(u08 num)
{
  DS("lfr:"); DB(num); DNL;
  u32 val = proto_api_lfunc_read(num);
  proto_low_read_long(val);
  proto_low_end();
}

static void handle_lfunc_write(u08 num)
{
  DS("lfw:"); DB(num); DNL;
  u32 val = proto_low_write_long();
  proto_low_end();
  proto_api_lfunc_write(num, val);
}

static void handle_msg_read(u08 chan)
{
  DS("mr:#"); DB(chan); DC(':');

  // get size and crc
  u16 size_words = 0;
  u16 crc = 0;
  u08 *buf = proto_api_read_msg_prepare(chan, &size_words, &crc);

  DC('+'); DW(size_words); DC('%'); DW(crc); DNL;
  proto_low_read_block(size_words, buf, crc);
  proto_api_read_msg_done(chan);

  proto_low_end();
}

static void handle_msg_write(u08 chan)
{
  DS("mw:#"); DB(chan); DC(':');
  
  u16 max_words = 0;
  u08 *buf = proto_api_write_msg_prepare(chan, &max_words);
  DW(max_words); DC(':');
  
  u16 crc = 0;
  u16 size_words = proto_low_write_block(max_words, buf, &crc);
  
  DC('+'); DW(size_words); DC('%'); DW(crc); DNL;
  proto_api_write_msg_done(chan, size_words, crc);

  proto_low_end();
}

void proto_handle(void)
{
  // read command from bits 0..4 in idle byte
  u08 cmd = proto_low_get_cmd();
  if(cmd == 0xff) {
    // no clock lined pulled -> idle
    return;
  }

  DS("cmd:"); DB(cmd); DNL;

  // extract command group
  u08 grp = cmd & PROTO_CMD_MASK;
  u08 chn = cmd & PROTO_CMD_ARG;
  switch(grp) {
    case PROTO_CMD_ACTION:
      handle_action(chn);
      break;
    case PROTO_CMD_WFUNC_READ:
      handle_wfunc_read(chn);
      break;
    case PROTO_CMD_WFUNC_WRITE:
      handle_wfunc_write(chn);
      break;
    case PROTO_CMD_LFUNC_READ:
      handle_lfunc_read(chn);
      break;
    case PROTO_CMD_LFUNC_WRITE:
      handle_lfunc_write(chn);
      break;
    case PROTO_CMD_MSG_READ:
      proto_api_read_msg(chn);
      break;
    case PROTO_CMD_MSG_WRITE:
      proto_api_write_msg(chn);
      break;
    default:
      DS("invalid!"); DNL;
      break;
  }

  DS("cmd_end:"); DB(cmd); DNL;
}

// default binding vor message read/write
void proto_api_read_msg(u08 chn) __attribute__ ((weak, alias("handle_msg_read")));
void proto_api_write_msg(u08 chn) __attribute__ ((weak, alias("handle_msg_write")));
