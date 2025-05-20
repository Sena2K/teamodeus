// bfc.c - Compilador Brainfuck para expressões simples com UTF-8 no nome da variável

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_TERMS 100

typedef struct {
    int is_mul;   // 1 = termo A*B, 0 = termo único
    int a, b;     // caso is_mul==1
    int val;      // caso is_mul==0
} Term;

void trim(char *s) {
    // remove espaços e tabs
    char *p = s, *w = s;
    while (*p) {
        if (!isspace((unsigned char)*p)) *w++ = *p;
        p++;
    }
    *w = '\0';
}

int main() {
    char line[MAX_LINE];
    if (!fgets(line, sizeof(line), stdin))
        return 0;

    // retira newline
    char *nl = strchr(line, '\n');
    if (nl)
        *nl = '\0';

    // pega a parte antes do '=' para varname, sem cortar no meio do UTF-8
    char varname[MAX_LINE];
    int varlen = 0;
    while (line[varlen] != '=' && line[varlen] != '\0') varlen++;
    strncpy(varname, line, varlen);
    varname[varlen] = '\0';
    trim(varname);

    // parte depois do '='
    char expr[MAX_LINE];
    char *eq = strchr(line, '=');
    if (!eq) return 1;
    strcpy(expr, eq+1);
    trim(expr);

    // quebra termos por '+'
    Term terms[MAX_TERMS];
    int term_count = 0;
    char *p = expr;
    while (1) {
        char *plus = strchr(p, '+');
        char piece[MAX_LINE];
        if (plus) {
            int len = plus - p;
            strncpy(piece, p, len);
            piece[len] = '\0';
        } else {
            strcpy(piece, p);
        }
        trim(piece);

        // detecta multiplicação A*B
        char *mul = strchr(piece, '*');
        if (mul) {
            *mul = '\0';
            char left[MAX_LINE], right[MAX_LINE];
            strcpy(left, piece);
            strcpy(right, mul+1);
            trim(left); trim(right);
            terms[term_count].is_mul = 1;
            terms[term_count].a = atoi(left);
            terms[term_count].b = atoi(right);
        } else {
            terms[term_count].is_mul = 0;
            terms[term_count].val = atoi(piece);
        }
        term_count++;
        if (!plus) break;
        p = plus+1;
    }

    // imprime nome da variável byte a byte em Brainfuck
    // Cada byte do UTF-8 vira um caractere pra imprimir
    for (int i = 0; i < (int)strlen(varname); i++) {
        unsigned char c = (unsigned char)varname[i];
        printf(">");
        for (int j = 0; j < c; j++) printf("+");
        printf(".");
    }
    printf(">");
    for (int i = 0; i < 61; i++) printf("+"); // imprime o '=' fixo (61 = código ASCII '=')
    printf(".");
    for (int i = 0; i < (int)strlen(varname) + 1; i++) printf("<");

    // zera cell 0
    printf("[-]");

    // para cada termo:
    // • se for número, adiciona '+' v vezes em cell0
    // • se for A*B, usa cell1 para o loop de multiplicação
    for (int i = 0; i < term_count; i++) {
        if (terms[i].is_mul) {
            int A = terms[i].a;
            int B = terms[i].b;
            // vai para cell1 e zera
            printf(">");
            printf("[-]");
            // coloca B em cell1
            for (int j = 0; j < B; j++) printf("+");
            // para cada unidade em cell1, soma A em cell0
            printf("[<");
            for (int j = 0; j < A; j++) printf("+");
            printf(">-]");
            // volta para cell0
            printf("<");
        } else {
            int V = terms[i].val;
            // permanece em cell0 e soma V
            for (int j = 0; j < V; j++) printf("+");
        }
    }

    // não imprimimos '\n' no compilador
    return 0;
}
