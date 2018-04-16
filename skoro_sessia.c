#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef unsigned char byte;
typedef int word;
typedef word adr;

byte mem[56*1024];

#define LO(x) (x & 0xFF);
#define HI(x) (((x>>8) & 0xFF));

void b_write (adr a, byte val) {
    mem[a] = val;
}
byte b_read (adr a) {
    return mem[a];
}
word w_read (adr a) {
    word res;
    assert (a % 2 == 0);
    res = (word)(mem[a])|(word)(mem[a+1]<<8);
    return res;
}
void w_write (adr a, word val) {
    assert (a % 2 == 0);
    mem[a] = LO(val);
    mem[a+1] = HI(val);
}
void do_halt() {
    printf ("URA!\n");
    exit(0);
}
void do_add() {}
void do_move() {}
void do_unknown() {}

struct Command {
    word opcode;
    word mask;
    const char * name;
    void (*do_func) ();
    
    
} command[] = {
    {0010000, 0170000, "mov", do_mov},
    {0060000, 0170000, "add", do_add},
    {0000000, 0177777, "halt", do_halt},
    {0000000, 0170000, "unknown", do_unknown}
};
void run() {
    word pc = 0100;
    while(1) {
        word w = w_read(pc);
        fprintf(stdout, "%06o: %06o", pc, w);
        pc += 2;
        int i;
        for (i = 0; i < 4; i++) {
            struct Command cmd = command[i];
            if ((w & cmd.mask) == cmd.opcode) {
                print ("%s ", cmd.name);
                //razbor argumentov mov
                print ("\n");
                cmd.do_func();
            }
        }
        /*
        //print command name
        if (w == 0) {
            print("HALT ");
            print("\n");
            do_halt();
        } else if ((w & 0170000) = 0010000) {
            //0xF << 12
            //0xF00
            //0170000
            print ("MOV ");
            //razbor argumentov mov
            print ("\n");
        } else if ((w & 0170000) = 0010000) {
            //0xF << 12
            //0xF00
            //0170000
            print ("ADD ");
            //razbor argumentov add
            print ("\n");
        }
        * /
     }       
}
int main (int argc, char * argv) {
    file_load (argv[1]);
    run();
    return 0;
}qqq 
