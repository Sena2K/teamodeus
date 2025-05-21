#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_TERMS 100
#define MAX_LOOP 1000  

typedef struct {
    int sign;     
    int is_mul;   
    int is_div;   
    int a, b;     
    int val;      
} Term;

void trim(char *s) {
    char *start = s;
    while (isspace((unsigned char)*start)) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);
    char *end = s + strlen(s) - 1;
    while (end >= s && isspace((unsigned char)*end)) *end-- = '\0';
}

int main() {
    char line[MAX_LINE];
    if (!fgets(line, sizeof(line), stdin)) return 0;

    char *nl = strchr(line, '\n');
    if (nl) *nl = '\0';

    char varname[MAX_LINE];
    size_t varlen = 0;
    while (line[varlen] != '=' && line[varlen] != '\0') varlen++;
    if (varlen >= MAX_LINE) varlen = MAX_LINE - 1;
    strncpy(varname, line, varlen);
    varname[varlen] = '\0';
    trim(varname);

    char expr[MAX_LINE];
    char *eq = strchr(line, '=');
    if (!eq) return 1;  
    strcpy(expr, eq + 1);
    trim(expr);

    Term terms[MAX_TERMS];
    int term_count = 0;
    char *p = expr;
    int sign = 1;

    while (*p && term_count < MAX_TERMS) {
        while (*p == ' ') p++;
        if (*p == '+' || *p == '-') {
            sign = (*p == '+') ? 1 : -1;
            p++;
            continue;
        }
        char *term_start = p;
        while (*p && *p != '+' && *p != '-') p++;
        char term[MAX_LINE];
        size_t len = p - term_start;
        if (len >= MAX_LINE) len = MAX_LINE - 1;
        strncpy(term, term_start, len);
        term[len] = '\0';
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

    for (size_t i = 0; i < strlen(varname); i++) {
        unsigned char c = (unsigned char)varname[i];
        printf(">");
        for (int j = 0; j < c && j < MAX_LOOP; j++) printf("+");
        printf(".");
    }
    printf(">");
    for (int i = 0; i < 61; i++) printf("+");
    printf(".");
    for (size_t i = 0; i < strlen(varname) + 1; i++) printf("<");
    printf("[-]");

    for (int i = 0; i < term_count; i++) {
        if (terms[i].is_mul) {
            int A = terms[i].a;
            int B = terms[i].b;
            int S = terms[i].sign;
            if (A < 0) A = -A, S = -S;
            if (B < 0) B = -B, S = -S;
            if (A > MAX_LOOP) A = MAX_LOOP;
            if (B > MAX_LOOP) B = MAX_LOOP;

         
            printf(">[-]");          
            for (int j = 0; j < B; j++) printf("+"); 
            printf("[<");
            for (int j = 0; j < A; j++) {
                printf(S == 1 ? "+" : "-");
            }
            printf("> -]");
            printf("<");
        } else if (terms[i].is_div) {

            int A = terms[i].a;
            int B = terms[i].b;
            int S = terms[i].sign;
            if (A < 0) A = -A, S = -S;
            if (B < 0) B = -B, S = -S;
            if (A > MAX_LOOP) A = MAX_LOOP;
            if (B > MAX_LOOP || B == 0) B = 1; 

            printf(">>[-]");          
            for (int j = 0; j < A; j++) printf("+"); 
            printf(">[-]");            
            printf(">[-]");            


            printf("<<[");            
            printf(">[-");            
            for (int j = 0; j < B; j++) printf("-"); 
            printf(">>+<<]");        
            printf(">>[-]");         

            printf(">>]");

        
            printf(">>");             
            if (S == 1) {
                printf("[>+<-]");     
            } else {
                printf("[>-<-]");     
            }
            printf("<<");             
        } else {
            int V = terms[i].val;
            int S = terms[i].sign;
            if (V < 0) V = -V, S = -S;
            if (V > MAX_LOOP) V = MAX_LOOP;

            if (S == 1) {
                for (int j = 0; j < V; j++) printf("+");
            } else {
                for (int j = 0; j < V; j++) printf("-");
            }
        }
    }

    printf("\n"); 
    return 0;
}
