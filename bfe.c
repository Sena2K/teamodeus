#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 30000
#define MAX_CODE_SIZE 100000
#define MAX_NESTED_LOOPS 100

// Interpretador de Brainfuck
int execute_brainfuck(const char* code) {
    unsigned char memory[MEMORY_SIZE] = {0};
    int ptr = 0;
    int code_pos = 0;
    int loop_stack[MAX_NESTED_LOOPS];
    int loop_index = 0;
    
    while (code[code_pos] != '\0') {
        switch (code[code_pos]) {
            case '>':
                ptr++;
                if (ptr >= MEMORY_SIZE) {
                    fprintf(stderr, "Erro: Ponteiro fora dos limites de memória (para frente)\n");
                    return 1;
                }
                break;
                
            case '<':
                ptr--;
                if (ptr < 0) {
                    fprintf(stderr, "Erro: Ponteiro fora dos limites de memória (para trás)\n");
                    return 1;
                }
                break;
                
            case '+':
                memory[ptr]++;
                break;
                
            case '-':
                memory[ptr]--;
                break;
                
            case '.':
                putchar(memory[ptr]);
                break;
                
            case ',':
                memory[ptr] = getchar();
                break;
                
            case '[':
                if (memory[ptr] == 0) {
                    // Pula para o correspondente ]
                    int depth = 1;
                    while (depth > 0) {
                        code_pos++;
                        if (code[code_pos] == '[') {
                            depth++;
                        } else if (code[code_pos] == ']') {
                            depth--;
                        }
                        
                        if (code[code_pos] == '\0') {
                            fprintf(stderr, "Erro: Loop não fechado\n");
                            return 1;
                        }
                    }
                } else {
                    if (loop_index >= MAX_NESTED_LOOPS) {
                        fprintf(stderr, "Erro: Loops aninhados demais\n");
                        return 1;
                    }
                    loop_stack[loop_index++] = code_pos;
                }
                break;
                
            case ']':
                if (loop_index <= 0) {
                    fprintf(stderr, "Erro: ']' sem correspondente '['\n");
                    return 1;
                }
                
                if (memory[ptr] != 0) {
                    code_pos = loop_stack[loop_index - 1];
                } else {
                    loop_index--;
                }
                break;
        }
        
        code_pos++;
    }

    int result = memory[0];
    printf("%d\n", result);
    
    return 0;
}

int main() {
    char code[MAX_CODE_SIZE];
    int pos = 0;
    int c;
    
    // Lê o código Brainfuck da entrada padrão
    while ((c = getchar()) != EOF && pos < MAX_CODE_SIZE - 1) {
        // Apenas caracteres válidos de Brainfuck são armazenados
        if (c == '>' || c == '<' || c == '+' || c == '-' || 
            c == '.' || c == ',' || c == '[' || c == ']') {
            code[pos++] = c;
        }
    }
    code[pos] = '\0';
    
    // Executa o código Brainfuck
    execute_brainfuck(code);
    
    return 0;
}