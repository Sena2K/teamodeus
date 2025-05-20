#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// --- Parser e avaliação simples de expressões inteiras ( + - * / com precedência ) ---

// Token types
typedef enum {TOK_NUM, TOK_PLUS, TOK_MINUS, TOK_MUL, TOK_DIV, TOK_LPAREN, TOK_RPAREN, TOK_END, TOK_INVALID} TokenType;

typedef struct {
    TokenType type;
    int value;
} Token;

const char *input_expr;
int pos = 0;

Token get_next_token() {
    while (input_expr[pos] && isspace(input_expr[pos])) pos++;
    if (!input_expr[pos]) return (Token){TOK_END, 0};

    if (isdigit(input_expr[pos])) {
        int val = 0;
        while (isdigit(input_expr[pos])) {
            val = val * 10 + (input_expr[pos] - '0');
            pos++;
        }
        return (Token){TOK_NUM, val};
    }

    char ch = input_expr[pos++];
    switch (ch) {
        case '+': return (Token){TOK_PLUS, 0};
        case '-': return (Token){TOK_MINUS, 0};
        case '*': return (Token){TOK_MUL, 0};
        case '/': return (Token){TOK_DIV, 0};
        case '(': return (Token){TOK_LPAREN, 0};
        case ')': return (Token){TOK_RPAREN, 0};
        default: return (Token){TOK_INVALID, 0};
    }
}

// Parsing with precedence:
// expr -> term ((+|-) term)*
// term -> factor ((*|/) factor)*
// factor -> NUM | (expr)

Token current_token;

void error(const char *msg) {
    fprintf(stderr, "Parse error: %s\n", msg);
    exit(1);
}

void next_token() {
    current_token = get_next_token();
}

int parse_factor();

int parse_term() {
    int result = parse_factor();
    while (current_token.type == TOK_MUL || current_token.type == TOK_DIV) {
        TokenType op = current_token.type;
        next_token();
        int rhs = parse_factor();
        if (op == TOK_MUL) result *= rhs;
        else {
            if (rhs == 0) error("Divisão por zero");
            result /= rhs;
        }
    }
    return result;
}

int parse_factor() {
    if (current_token.type == TOK_NUM) {
        int val = current_token.value;
        next_token();
        return val;
    } else if (current_token.type == TOK_LPAREN) {
        next_token();
        int val = parse_term();
        if (current_token.type != TOK_RPAREN) error("Esperado ')'");
        next_token();
        return val;
    } else if (current_token.type == TOK_MINUS) {
        // Suporte para número negativo
        next_token();
        int val = parse_factor();
        return -val;
    }
    error("Esperado número ou '('");
    return 0;
}

int parse_expression() {
    int result = parse_term();
    while (current_token.type == TOK_PLUS || current_token.type == TOK_MINUS) {
        TokenType op = current_token.type;
        next_token();
        int rhs = parse_term();
        if (op == TOK_PLUS) result += rhs;
        else result -= rhs;
    }
    return result;
}

// --- Gera Brainfuck para imprimir número decimal positivo (base 10) ---
// Método simples: converte inteiro em string decimal e gera BF para imprimir caractere a caractere

void gen_bf_print_number(int n, FILE *out) {
    if (n < 0) {
        // Imprime '-' e usa n = -n
        fputs("+++++++++++++++++++++++++++++++++++++++++++++.\n", out); // ASCII '-' é 45, ajuste 45 '+'
        // Mais seguro: gerar '-' diretamente (mais abaixo)
        // Vou gerar '-' com 45 +:
        // ++++++++++++++++++++++++++++++++++++++++++++++ = 45 ( 10 vezes '+' = 10; 4 vezes 10+)
        // Mas para simplificar, gerar 45 '+' e '.' para imprimir '-'
        fprintf(out, "+++++++++++++++++++++++++++++++++++++++++++++.\n"); // '-' (45)
        n = -n;
    }

    // Converte n para string decimal
    char buf[20];
    snprintf(buf, sizeof(buf), "%d", n);

    // Para cada caractere decimal, gera código BF para imprimir ASCII do caractere
    // ASCII de '0' = 48, '1' = 49, etc
    // Para otimizar, pode-se limpar célula e incrementar de 0 para o valor, depois imprimir.

    for (int i = 0; buf[i]; i++) {
        int digit = buf[i];
        // Limpa célula (seta a zero)
        fputs("[-]", out);
        // Incrementa até o ASCII do dígito
        for (int j = 0; j < digit; j++) {
            fputc('+', out);
        }
        fputc('.', out);
    }
}

// --- Brainfuck interpretador simplificado ---

#define BF_MEM_SIZE 30000

int run_brainfuck(const char *code, char *output, int max_out_len) {
    char memory[BF_MEM_SIZE] = {0};
    int ptr = 0;
    int outpos = 0;

    int code_len = strlen(code);
    int pc = 0;

    // Precalcular saltos para '[' e ']' (usar pilha)
    int *jump_forward = malloc(sizeof(int) * code_len);
    int *jump_backward = malloc(sizeof(int) * code_len);
    int *stack = malloc(sizeof(int) * code_len);
    int sp = 0;

    for (int i = 0; i < code_len; i++) {
        jump_forward[i] = -1;
        jump_backward[i] = -1;
    }
    for (int i = 0; i < code_len; i++) {
        if (code[i] == '[') {
            stack[sp++] = i;
        } else if (code[i] == ']') {
            if (sp == 0) {
                free(jump_forward);
                free(jump_backward);
                free(stack);
                return -1; // erro de sintaxe
            }
            int start = stack[--sp];
            int end = i;
            jump_forward[start] = end;
            jump_backward[end] = start;
        }
    }
    if (sp != 0) {
        free(jump_forward);
        free(jump_backward);
        free(stack);
        return -1; // erro de sintaxe
    }

    while (pc < code_len) {
        switch (code[pc]) {
            case '>':
                ptr++;
                if (ptr >= BF_MEM_SIZE) ptr = 0;
                break;
            case '<':
                ptr--;
                if (ptr < 0) ptr = BF_MEM_SIZE - 1;
                break;
            case '+':
                memory[ptr]++;
                break;
            case '-':
                memory[ptr]--;
                break;
            case '.':
                if (outpos < max_out_len - 1) {
                    output[outpos++] = memory[ptr];
                }
                break;
            case ',':
                // Não usado aqui
                break;
            case '[':
                if (memory[ptr] == 0) {
                    pc = jump_forward[pc];
                }
                break;
            case ']':
                if (memory[ptr] != 0) {
                    pc = jump_backward[pc];
                }
                break;
            default:
                // ignora comentários
                break;
        }
        pc++;
    }

    output[outpos] = '\0';

    free(jump_forward);
    free(jump_backward);
    free(stack);

    return 0;
}

// --- Programa principal ---

int main() {
    char line[256];
    // NÃO imprimir prompt, só ler direto
    if (!fgets(line, sizeof(line), stdin)) {
        fprintf(stderr, "Erro lendo entrada\n");
        return 1;
    }

    // Retirar possível '\n' do final
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n')
        line[len - 1] = '\0';

    // Restante código igual...

    // Parsear linha para extrair prefixo "AÇÃO=", expressão matemática e pipeline "bfc | bfe"
    char *pipe1 = strstr(line, "|");
    if (!pipe1) {
        fprintf(stderr, "Erro: entrada deve conter '|'\n");
        return 1;
    }
    char before_pipe[128];
    strncpy(before_pipe, line, pipe1 - line);
    before_pipe[pipe1 - line] = '\0';

    char *eq = strchr(before_pipe, '=');
    if (!eq) {
        fprintf(stderr, "Erro: esperado '=' na entrada\n");
        return 1;
    }

    char prefix[64];
    strncpy(prefix, before_pipe, eq - before_pipe + 1);
    prefix[eq - before_pipe + 1] = '\0';

    char *expr_str = eq + 1;

    // remover espaços no final da expressão
    char *end_expr = expr_str + strlen(expr_str) - 1;
    while (end_expr > expr_str && isspace(*end_expr)) {
        *end_expr = '\0';
        end_expr--;
    }

    input_expr = expr_str;
    pos = 0;
    next_token();
    int valor = parse_expression();
    if (current_token.type != TOK_END) {
        fprintf(stderr, "Erro: expressão com token inesperado no final\n");
        return 1;
    }

    // Imprime prefixo direto
    printf("%s", prefix);

    // Gera código Brainfuck na memória
    char bfcode[1024] = {0};
    FILE *tmp = fmemopen(bfcode, sizeof(bfcode), "w");
    if (!tmp) {
        perror("fmemopen");
        return 1;
    }
    gen_bf_print_number(valor, tmp);
    fclose(tmp);

    // Interpreta o código Brainfuck gerado
    char output[128];
    if (run_brainfuck(bfcode, output, sizeof(output)) != 0) {
        fprintf(stderr, "Erro no interpretador Brainfuck\n");
        return 1;
    }

    printf("%s\n", output);

    return 0;
}
