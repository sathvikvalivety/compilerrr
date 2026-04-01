#include "compiler.h"
#include <ctype.h>

Token tokens[MAX_TOKENS];
int token_count = 0;
int token_idx = 0;

Symbol sym_table[MAX_SYMBOLS];
int sym_count = 0;

void add_symbol(const char* name) {
    for (int i = 0; i < sym_count; i++) {
        if (strcmp(sym_table[i].name, name) == 0) return; // already exists
    }
    strcpy(sym_table[sym_count].name, name);
    sym_table[sym_count].type = 0; // default int
    sym_count++;
}

void lex(const char* source) {
    int i = 0;
    while (source[i] != '\0') {
        if (isspace(source[i])) {
            i++;
            continue;
        }

        Token t;
        memset(&t, 0, sizeof(Token));

        if (isalpha(source[i])) { // Identifier
            int j = 0;
            while (isalnum(source[i])) {
                t.lexeme[j++] = source[i++];
            }
            t.lexeme[j] = '\0';
            t.type = TOK_ID;
            add_symbol(t.lexeme);
        } else if (isdigit(source[i])) { // Number
            int j = 0;
            while (isdigit(source[i])) {
                t.lexeme[j++] = source[i++];
            }
            t.lexeme[j] = '\0';
            t.type = TOK_NUM;
            t.value = atoi(t.lexeme);
        } else { // Operators
            switch (source[i]) {
                case '+': t.type = TOK_PLUS; t.lexeme[0] = '+'; i++; break;
                case '-': t.type = TOK_MINUS; t.lexeme[0] = '-'; i++; break;
                case '*': t.type = TOK_MUL; t.lexeme[0] = '*'; i++; break;
                case '/': t.type = TOK_DIV; t.lexeme[0] = '/'; i++; break;
                case '(': t.type = TOK_LPAREN; t.lexeme[0] = '('; i++; break;
                case ')': t.type = TOK_RPAREN; t.lexeme[0] = ')'; i++; break;
                case '>':
                    if (source[i+1] == '=') { t.type = TOK_GE; strcpy(t.lexeme, ">="); i+=2; }
                    else { t.type = TOK_GT; t.lexeme[0] = '>'; i++; }
                    break;
                case '<':
                    if (source[i+1] == '=') { t.type = TOK_LE; strcpy(t.lexeme, "<="); i+=2; }
                    else { t.type = TOK_LT; t.lexeme[0] = '<'; i++; }
                    break;
                case '=':
                    if (source[i+1] == '=') { t.type = TOK_EQ; strcpy(t.lexeme, "=="); i+=2; }
                    else {
                        printf("Syntax Error: Unknown character '%c'\n", source[i]);
                        exit(1);
                    }
                    break;
                default:
                    printf("Syntax Error: Unknown character '%c'\n", source[i]);
                    exit(1);
            }
        }
        tokens[token_count++] = t;
    }
    tokens[token_count].type = TOK_EOF;
}
