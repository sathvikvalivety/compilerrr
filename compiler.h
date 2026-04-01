#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// --- Phase 1: Lexical Analysis ---

typedef enum {
    TOK_ID,
    TOK_NUM,
    TOK_PLUS,   // +
    TOK_MINUS,  // -
    TOK_MUL,    // *
    TOK_DIV,    // /
    TOK_GT,     // >
    TOK_LT,     // <
    TOK_GE,     // >=
    TOK_LE,     // <=
    TOK_EQ,     // ==
    TOK_AND,    // &&
    TOK_OR,     // ||
    TOK_LPAREN, // (
    TOK_RPAREN, // )
    TOK_EOF
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[64];
    int value; // meaningful if TOK_NUM
} Token;

#define MAX_TOKENS 1024
extern Token tokens[MAX_TOKENS];
extern int token_count;
extern int token_idx; // for parsing

// Symbol Table
#define MAX_SYMBOLS 100
typedef struct {
    char name[64];
    int type; // 0 for int
} Symbol;

extern Symbol sym_table[MAX_SYMBOLS];
extern int sym_count;

// Phase 1 Function
void lex(const char* source);
void add_symbol(const char* name);

// --- Phase 2: Syntax Analysis ---

typedef enum {
    AST_NUM,
    AST_ID,
    AST_BINOP,
    AST_RELOP
} ASTNodeType;

typedef enum {
    TYPE_UNKNOWN,
    TYPE_INT,
    TYPE_BOOL
} ExprType;

typedef struct ASTNode {
    ASTNodeType node_type;
    TokenType op; // +, -, *, /, >, <, etc. meaningful for BINOP/RELOP
    char name[64]; // for AST_ID
    int value; // for AST_NUM
    struct ASTNode* left;
    struct ASTNode* right;
    
    ExprType expr_type; // Populated in Semantic Analysis
} ASTNode;

ASTNode* parse();
void print_ast(ASTNode* root, int space);

// --- Phase 3: Semantic Analysis ---

void semantic_analyze(ASTNode* node);

// --- Phase 4: Intermediate Code Generation (TAC) ---

typedef enum {
    TAC_ADD, TAC_SUB, TAC_MUL, TAC_DIV,
    TAC_GT, TAC_LT, TAC_GE, TAC_LE, TAC_EQ,
    TAC_ASSIGN,
    TAC_IFGOTO, TAC_GOTO,
    TAC_LABEL
} TACOp;

typedef struct {
    TACOp op;
    char arg1[64];
    char arg2[64];
    char result[64]; // or goto label
    char comment[64];
} TACInstr;

#define MAX_TAC 1024
extern TACInstr tac_stream[MAX_TAC];
extern int tac_count;

void generate_tac(ASTNode* node);
void add_tac_comment(TACOp op, const char* arg1, const char* arg2, const char* result, const char* comment);
void add_tac(TACOp op, const char* arg1, const char* arg2, const char* result);
char* new_temp();
char* new_label();

// --- Phase 5: Code Optimization ---

void optimize();

// --- Phase 6: Code Generation ---

void generate_x86();

#endif // COMPILER_H
