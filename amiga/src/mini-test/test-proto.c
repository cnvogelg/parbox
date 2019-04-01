#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>

#include "autoconf.h"
#include "compiler.h"
#include "debug.h"
#include "pario.h"
#include "timer.h"
#include "proto.h"

int dosmain(void)
{
    struct pario_handle *ph;
    struct timer_handle *th;
    int error;

    PutStr("test-proto\n");
    ph = pario_init((struct Library *)SysBase);
    if(ph != NULL) {
        struct pario_port *port = pario_get_port(ph);
        PutStr("timer_init\n");
        th = timer_init((struct Library *)SysBase);
        if(th != NULL) {
            /* setup proto low */
            PutStr("proto_init\n");
            struct proto_handle *ph = proto_init(port, th, (struct Library *)SysBase);
            if(ph != NULL) {

                // actions
                PutStr("bootloader");
                error = proto_bootloader(ph);
                Printf("-> %ld\n", (LONG)error);

                PutStr("reset");
                error = proto_reset(ph);
                Printf("-> %ld\n", (LONG)error);

                PutStr("ping");
                error = proto_ping(ph);
                Printf("-> %ld\n", (LONG)error);

                PutStr("action 15");
                error = proto_action(ph, 15);
                Printf("-> %ld\n", (LONG)error);

                // functions
                PutStr("wfunc_read");
                UWORD wdata = 0;
                error = proto_function_read_word(ph, 0, &wdata);
                Printf("-> %ld : %lx\n", (LONG)error, (ULONG)wdata);

                PutStr("wfunc_write");
                error = proto_function_write_word(ph, 0, 0xdead);
                Printf("-> %ld\n", (LONG)error);

                PutStr("lfunc_read");
                ULONG ldata = 0;
                error = proto_function_read_long(ph, 0, &ldata);
                Printf("-> %ld : %lx\n", (LONG)error, ldata);

                PutStr("lfunc_write");
                error = proto_function_write_long(ph, 0, 0xcafebabe);
                Printf("-> %ld\n", (LONG)error);

                // message

                PutStr("msg read");
                UBYTE buf[512];
                UWORD words = 256;
                UWORD crc = 0;
                error = proto_msg_read_single(ph, 0, buf, &words, &crc);
                Printf("-> %ld : #%ld  crc=%lx\n", (LONG)error, (ULONG)words, (ULONG)crc);

                PutStr("msg write");
                error = proto_msg_write_single(ph, 0, buf, 256, 0xfeed);
                Printf("-> %ld\n", (LONG)error);

                proto_exit(ph);
            } else {
                PutStr("error setting up proto!\n");
            }
            timer_exit(th);
        } else {
            PutStr("error setting up timer!\n");
        }
        pario_exit(ph);
    } else {
        PutStr("error setting up pario!\n");
    }
    return 0;
}
