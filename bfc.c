#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_TERMS 100

typedef struct {
    int sign;     // 1 para positivo, -1 para negativo
    int is_mul;   // 1 se for multiplicação, 0 caso contrário
    int is_div;   // 1 se for divisão, 0 caso contrário
    int a, b;     // operandos para multiplicação ou divisão
    int val;      // valor para termo único
} Term;

void trim(char *s) {
    char *p = s, *w = s;
    while (*p) {
        if (!isspace((unsigned char)*p)) *w++ = *p;
        p++;
    }
    *w = '\0';
}

int main() {
    char line[MAX_LINE];
    if (!fgets(line, sizeof(line), stdin)) return 0;

    char *nl = strchr(line, '\n');
    if (nl) *nl = '\0';

    char varname[MAX_LINE];
    int varlen = 0;
    while (line[varlen] != '=' && line[varlen] != '\0') varlen++;
    strncpy(varname, line, varlen);
    varname[varlen] = '\0';
    trim(varname);

    char expr[MAX_LINE];
    char *eq = strchr(line, '=');
    if (!eq) return 1;  // Fixed line 42
    strcpy(expr, eq + 1);
    trim(expr);

    Term terms[MAX_TERMS];
    int term_count = 0;
    char *p = expr;
    int sign = 1;

    while (*p) {
        while (*p == ' ') p++;
        if (*p == '+' || *p == '-') {
            sign = (*p == '+') ? 1 : -1;
            p++;
            continue;
        }
        char *term_start = p;
        while (*p && *p != '+' && *p != '-') p++;
        char term[MAX_LINE];
        strncpy(term, term_start, p - term_start);
        term[p - term_start] = '\0';
        trim(term);
        if (strlen(term) == 0) continue;

        char *mul = strchr(term, '*');
        char *div = strchr(term, '/');
        if (mul) {
            *mul = '\0';
            char left[MAX_LINE], right[MAX_LINE];
            strcpy(left, term);
            strcpy(right, mul + 1);
            trim(left); trim(right);
            terms[term_count].is_mul = 1;
            terms[term_count].is_div = 0;
            terms[term_count].a = atoi(left);
            terms[term_count].b = atoi(right);
            terms[term_count].sign = sign;
        } else if (div) {
            *div = '\0';
            char left[MAX_LINE], right[MAX_LINE];
            strcpy(left, term);
            strcpy(right, div + 1);
            trim(left); trim(right);
            terms[term_count].is_mul = 0;
            terms[term_count].is_div = 1;
            terms[term_count].a = atoi(left);
            terms[term_count].b = atoi(right);
            terms[term_count].sign = sign;
        } else {
            terms[term_count].is_mul = 0;
            terms[term_count].is_div = 0;
            terms[term_count].val = atoi(term);
            terms[term_count].sign = sign;
        }
        term_count++;
    }

    for (int i = 0; i < (int)strlen(varname); i++) {
        unsigned char c = (unsigned char)varname[i];
        printf(">");
        for (int j = 0; j < c; j++) printf("+");
        printf(".");
    }
    printf(">");
    for (int i = 0; i < 61; i++) printf("+");
    printf(".");
    for (int i = 0; i < (int)strlen(varname) + 1; i++) printf("<");
    printf("[-]");

    for (int i = 0; i < term_count; i++) {
        if (terms[i].is_mul) {
            int A = terms[i].a;
            int B = terms[i].b;
            int S = terms[i].sign;
            printf(">");  // Fixed printf statement
            printf("[-]");
            for (int j = 0; j < B; j++) printf("+");
            printf("[<");
            if (S == 1) {
                for (int j = 0; j < A; j++) printf("+");
            } else {
                for (int j = 0; j < A; j++) printf("-");
            }
            printf(">-]");
            printf("<");
        } else if (terms[i].is_div) {
            int A = terms[i].a;
            int B = terms[i].b;
            int S = terms[i].sign;
            printf(">>[-]");
            for (int j = 0; j < A; j++) printf("+");
            printf(">[-]");
            for (int j = 0; j < B; j++) printf("+");
            printf(">[-]");
            printf("<<[");
            printf(">[-");
            for (int j = 0; j < B; j++) printf("-");
            printf(">>+<<]");
            printf(">>[-]<<<<]");
            printf(">>[<<");
            if (S == 1) printf("+"); else printf("-");
            printf(">>-]");
            printf("<<");
        } else {
            int V = terms[i].val;
            int S = terms[i].sign;
            if (S == 1) {
                for (int j = 0; j < V; j++) printf("+");
            } else {
                for (int j = 0; j < V; j++) printf("-");
            }
        }
    }

    return 0;
}