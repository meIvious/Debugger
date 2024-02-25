#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

struct Trace {
    char var;
    int val;
};

struct Trace Traces[20];
int nTraces = 0;
void add_TT(char var) {
    if (nTraces < 20) {
        Traces[nTraces].var = var;
        Traces[nTraces].val = 0; 
        nTraces++;
    }
}

void display_TT(char var) {
    for (int i = 0; i < nTraces; ++i) {
        if (Traces[i].var == var) {
            printf("%c: %d", var, Traces[i].val);
            while (getchar() != '\n');
            return;
        }
    }
    printf("Variable %c not found in Traces table.\n", var);
}

void update_TT(char var, int val) {
    for (int i = 0; i < nTraces; ++i) {
        if (Traces[i].var == var) {
            Traces[i].val = val;
            return;
        }
    }
}