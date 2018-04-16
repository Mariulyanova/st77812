#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef unsigned char byte;
typedef int word;
typedef word adr;

byte mem[56*1024];
word reg[8];
#define pc reg[7];

#define LO(x) (x & 0xFF);
#define HI(x) (((x>>8) & 0xFF));

#define NO_PARAM 0;
#define HAS_SS (1 << 1);
#define HAS_DD (1 << 2);
#define HAS_XX 8;
#define HAS_NN 4;

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
void load_file() {
    FILE *f_in = stdin;
    adr a;
    unsigned int b;
    int n, i;
    while(1) {
        if((fscanf(f_in, "%x%x", &a , &n)) != 2)
            break;
        for (i = 0; i < n ; i++) {
            scanf("%x", &b);
            b_write(a+i,(byte)b);
        }
    }
}
void mem_dump(adr s, word n) {
    int i;
    for (i = 0; i < n; i += 2) {
        printf("%06o : ", s+i);
        printf("%06o\n", w_read(s+i));
    }
}
void test() {
    byte b0, b1;
    word w;
    w = 0x0b0a;
    w_write (2, w);
    b0 = b_read(2); //0x0a
    b1 = b_read(3); //0x0b
    printf ("%04x= %02hhx%02hhx\n", w, b1, b0); //oboa = oboa
}
struct SSDD {
    word val;
    adr a;
} ss, dd;
word nn;	// глобальная, значение nn
 void get_nn (word w) { .. }

void do_halt() {
    printf ("URA, konets - delu venets!\n");
    exit(0);
}
void do_add(word ss, word dd) {
    w_write(dd.a, (w_read (ss.val) + w_read(dd.val));
    //write(dd.a) = ss.val + dd.val;
    }
void do_move(word ss, adr dd) {
    w_write(dd.a, w_read(ss.val));
    //write(dd.a) = ss.val;
}
void do_unknown() {

}

struct Command {
    word opcode;
    word mask;
    const char * name;
    void (*do_func) ();
    byte param;

} command[] = {
    {0010000, 0170000, "mov", do_mov, HAS_SS | HAS_DD},
    {0060000, 0170000, "add", do_add, HAS_SS | HAS_DD},
    {0000000, 0177777, "halt", do_halt, NO_PARAM},
    {0000000, 0000000, "unknown", do_unknown, NO_PARAM}
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
                print ("\n");
                cmd.do_func();
                //break
            }
        }

     }
}
int main (int argc, char * argv) {
    load_file (argv[1]);
    run();
    return 0;
}
