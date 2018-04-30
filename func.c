#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef unsigned char byte;
typedef int word;
typedef int adr;
typedef word adr;

byte mem[56*1024];
word reg[8];
#define pc reg[7]

#define LO(x) (x & 0xFF)
#define HI(x) (((x>>8) & 0xFF))

#define NO_PARAM 0
#define HAS_SS (1 << 1)
#define HAS_DD (1 << 2)
#define HAS_XX 8
#define HAS_NN 4

void b_write (adr a, byte val) {
    mem[a] = val;
}
byte b_read (adr a) {
    return mem[a];
}
word w_read (adr a) {
    if (a < 8) {
        return reg[a];  //Первые 8 слов памяти выделяем под регистры
    }
    word res;
    assert (a % 2 == 0);
    res = (word)(mem[a])|(word)(mem[a+1]<<8);
    return res;
}
void w_write (adr a, word val) {
    if (a < 8) {//work with registers
        reg[a]=val;
        return;
    }
    //work with memory
    assert (a % 2 == 0);
    mem[a] = LO(val);
    mem[a+1] = HI(val);
}
void load_file() {
    //FILE *f_in = stdin;
    FILE *f_in = fopen("C:\\Users\маша\st77812\sum_o.txt", "r");
    if (f_in == NULL) {
        perror("sum_o.txt");
    };
    adr a;
    unsigned int b;
    int n, i;
    while(1) {
        if((fscanf(f_in, "%x%x", &a , &n)) != 2)
            break;
        for (i = 0; i < n ; i++) {
            fscanf(f_in, "%x", &b);
            b_write(a+i,(byte)b);
        }
    }
    fclose (f_in);
}
void mem_dump(adr s, word n) { //распечатать
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
typedef struct SSDD {
    word val;
    adr a;
    int mode; //0 - a - reg number, !0 - a memory adress
} SSDD;
SSDD ss, dd;
word nn;	// глобальная, значение nn

word get_nn (word w) {
    return (w&077);
}
SSDD get_mr(word w) {
    SSDD res;
    int rn = w & 7;// номер регистра, последние 3 бита
    int mode = (w >>3)& 7;// номер моды, первые 3 бита
    switch (mode) {
    case 0:
        res.a = rn;
        res.val = reg[rn];
        printf("R%d", rn);
        break;
    case 1:
        res.a=reg[rn];
        res.val = w_read(res.a); //TODO: byte variant;
        printf("(R%d)", rn);
    case 2:
        res.a = reg[rn];
        res.val = w_read(res.a); //TODO: byte variant;
        reg[rn]+=2; //todo +=1
        if (rn ==  7)
            printf ("#%o", res.val);
        else
            printf("(R%d)+", rn);
        break;
    default:
        printf("Netu modi %d eshcho", mode);
        exit(2);
    }
    return res;
}

void dump_reg () {
int i;
for (i = 0; i < 8; i++) {
printf("R%d : %06o ", i, reg[i]);
}
printf("\n");
}

void do_halt() {
    printf("\n");
    dump_reg();
    printf ("URA, konets - delu venets!\n");
    exit(1);
}
void do_add(SSDD ss, SSDD dd) {
    //printf("add");
    w_write(dd.a, ss.val + dd.val);
    //write(dd.a) = ss.val + dd.val;
    }
void do_mov() {
    //printf("MOVE");
    w_write(dd.a, ss.val);
    //write(dd.a) = ss.val;
    printf("dd.a=%u ss.val=%u\n", dd.a, ss.val);
}
void do_unknown() {
    //printf("unknown");
}

struct Command {
    word opcode;
    word mask;
    const char * name;
    void (*do_func) ();
    byte param;

} command[] = {
    {0010000, 0170000, "move", do_mov, HAS_SS | HAS_DD},
    {0060000, 0170000, "add", do_add, HAS_SS | HAS_DD},
    {0000000, 0177777, "halt", do_halt, NO_PARAM},
    {0000000, 0000000, "unknown", do_unknown, NO_PARAM}
};
void run() {
    pc = 01000;
    while(1) {
        word w = w_read(pc);
        fprintf(stdout, "%06o: %06o", pc, w);
        pc += 2;
        int i;
        for (i = 0; i < 4; i++) {
            struct Command cmd = command[i];
            if ((w & cmd.mask) == cmd.opcode) {
                printf ("%s ", cmd.name);
                printf ("\n");
                if (cmd.param & HAS_SS) {
                    ss = get_mr(w);
                }
                if (cmd.param & HAS_DD) {
                    dd = get_mr(w);
                }
                if (cmd.param & HAS_NN) {
                    nn = get_nn(w);
                }
                cmd.do_func();
                break;
            }
        }
      dump_reg();
     }
}
int main () { //int argc, char * argv
    test ();
    load_file ();
    run();
    return 0;
}
