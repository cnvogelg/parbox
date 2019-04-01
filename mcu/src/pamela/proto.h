#ifndef PROTO_H

#include "proto_shared.h"

extern void proto_init(void);
extern void proto_first(void);
extern void proto_handle(void);

// ----- proto_api callbacks are defined in your code -----

// action handler
extern void proto_api_action(u08 num);

// func read/write
extern u16  proto_api_wfunc_read(u08 num);
extern void proto_api_wfunc_write(u08 num, u16 val);
extern u32  proto_api_lfunc_read(u08 num);
extern void proto_api_lfunc_write(u08 num, u32 val);

// high level message i/o api
extern u08 *proto_api_read_msg_prepare(u08 chan,u16 *words, u16 *crc);
extern void proto_api_read_msg_done(u08 chan);
extern u08 *proto_api_write_msg_prepare(u08 chan,u16 *max_words);
extern void proto_api_write_msg_done(u08 chan,u16 words, u16 crc);

// low level message i/o api
extern void proto_api_read_msg(u08 chan);
extern void proto_api_write_msg(u08 chan);

extern u08  proto_api_get_end_status(void);

#endif
