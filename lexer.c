#include "compiler.h"
#include <ctype.h>

Token tokens[MAX_TOKENS];
int token_count = 0;
int token_idx = 0;

Symbol sym_table[MAX_SYMBOLS];
int sym_count = 0;

const char* token_to_string(TokenType type) {
    switch (type) {
        case TOK_ID: return "ID";
        case TOK_NUM: return "NUM";
        case TOK_PLUS: return "'+'";
        case TOK_MINUS: return "'-'";
        case TOK_MUL: return "'*'";
        case TOK_DIV: return "'/'";
        case TOK_GT: return "'>'";
        case TOK_LT: return "'<'";
        case TOK_GE: return "'>='";
        case TOK_LE: return "'<='";
        case TOK_EQ: return "'=='";
        case TOK_AND: return "'&&'";
        case TOK_OR: return "'||'";
        case TOK_LPAREN: return "'('";
        case TOK_RPAREN: return "')'";
        case TOK_EOF: return "EOF";
        default: return "UNKNOWN";
    }
}

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
        t.position = i + 1; // 1-indexed strictly based on the starting character

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
                        printf("Lexical Error: Unrecognized character '%c' at position %d\n", source[i], i + 1);
                        exit(1);
                    }
                    break;
                case '&':
                    if (source[i+1] == '&') { t.type = TOK_AND; strcpy(t.lexeme, "&&"); i+=2; }
                    else {
                        printf("Syntax Error: Expected &&\n");
                        exit(1);
                    }
                    break;
                case '|':
                    if (source[i+1] == '|') { t.type = TOK_OR; strcpy(t.lexeme, "||"); i+=2; }
                    else {
                        printf("Syntax Error: Expected ||\n");
                        exit(1);
                    }
                    break;
                default:
                    printf("Lexical Error: Unrecognized character '%c' at position %d\n", source[i], i + 1);
                    exit(1);
            }
        }
        tokens[token_count++] = t;
    }
    
    // Check for dangling operators at the end of input
    if (token_count > 0) {
        Token last_t = tokens[token_count - 1];
        if (last_t.type == TOK_PLUS || last_t.type == TOK_MINUS || last_t.type == TOK_MUL || 
            last_t.type == TOK_DIV || last_t.type == TOK_GT || last_t.type == TOK_LT || 
            last_t.type == TOK_GE || last_t.type == TOK_LE || last_t.type == TOK_EQ || 
            last_t.type == TOK_AND || last_t.type == TOK_OR) {
            printf("Lexical Error: Unexpected end of input after '%s' at position %d\n", last_t.lexeme, last_t.position);
            exit(1);
        }
    }
    
    tokens[token_count].type = TOK_EOF;
    tokens[token_count].position = i + 1; // Assign position to EOF for parser end-of-string catching
}
