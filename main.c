#include "compiler.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s \"expression\"\n", argv[0]);
        printf("Example: %s \"(a + b) * 2 > c + 1\"\n", argv[0]);
        return 1;
    }

    const char* source = argv[1];
    printf("Compiling source: %s\n\n", source);

    // Phase 1: Lexical Analysis
    printf("--- Phase 1: Lexical Analysis ---\n");
    lex(source);
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == TOK_ID || tokens[i].type == TOK_NUM) {
            printf("Token: %d, Lexeme: %s\n", tokens[i].type, tokens[i].lexeme);
        } else if (tokens[i].type != TOK_EOF) {
            printf("Token: %d, Lexeme: %s\n", tokens[i].type, tokens[i].lexeme);
        }
    }

    // Phase 2: Syntax Analysis
    printf("\n--- Phase 2: Syntax Analysis ---\n");
    ASTNode* ast_root = parse();
    printf("\n--- AST ---\n");
    print_ast(ast_root, 0);
    printf("\n");

    // Phase 3: Semantic Analysis
    printf("\n--- Phase 3: Semantic Analysis ---\n");
    printf("Symbol Table:\n");
    for (int i = 0; i < sym_count; i++) {
        printf("  %s -> type: int\n", sym_table[i].name);
    }
    semantic_analyze(ast_root);
    printf("Semantic analysis passed.\n");

    // Phase 4: Intermediate Code Generation
    printf("\n--- Phase 4: Intermediate Code Generation (TAC) ---\n");
    generate_tac(ast_root);
    for (int i = 0; i < tac_count; i++) {
        TACInstr in = tac_stream[i];
        if (in.op >= TAC_ADD && in.op <= TAC_DIV) {
            char op_char = '+';
            if (in.op == TAC_SUB) op_char = '-';
            if (in.op == TAC_MUL) op_char = '*';
            if (in.op == TAC_DIV) op_char = '/';
            printf("%s = %s %c %s\n", in.result, in.arg1, op_char, in.arg2);
        } else if (in.op >= TAC_GT && in.op <= TAC_EQ) {
            char op_str[3] = "";
            if (in.op == TAC_GT) strcpy(op_str, ">");
            if (in.op == TAC_LT) strcpy(op_str, "<");
            if (in.op == TAC_GE) strcpy(op_str, ">=");
            if (in.op == TAC_LE) strcpy(op_str, "<=");
            if (in.op == TAC_EQ) strcpy(op_str, "==");
            printf("if %s %s %s goto %s\n", in.arg1, op_str, in.arg2, in.result);
        } else if (in.op == TAC_GOTO) {
            printf("goto %s\n", in.result);
        } else if (in.op == TAC_LABEL) {
            if (in.comment[0] != '\0') {
                printf("%s:\t; -- %s --\n", in.result, in.comment);
            } else {
                printf("%s:\n", in.result);
            }
        }
    }

    // Phase 5: Code Optimization
    printf("\n--- Phase 5: Code Optimization ---\n");
    optimize();

    // Phase 6: Code Generation
    generate_x86();

    return 0;
}
