#include "compiler.h"

void semantic_analyze(ASTNode* node) {
    if (!node) return;

    if (node->node_type == AST_NUM || node->node_type == AST_ID) {
        node->expr_type = TYPE_INT;
        return;
    }

    if (node->node_type == AST_BINOP) {
        semantic_analyze(node->left);
        semantic_analyze(node->right);
        
        if (node->op == TOK_AND || node->op == TOK_OR) {
            if (node->left->expr_type != TYPE_BOOL || node->right->expr_type != TYPE_BOOL) {
                const char* lt = (node->left->expr_type == TYPE_BOOL) ? "bool" : "int";
                const char* rt = (node->right->expr_type == TYPE_BOOL) ? "bool" : "int";
                char op_str[3] = "";
                if (node->op == TOK_AND) strcpy(op_str, "&&");
                else if (node->op == TOK_OR) strcpy(op_str, "||");
                printf("Semantic Error: '%s' requires boolean operands, got %s and %s\n", op_str, lt, rt);
                exit(1);
            }
            node->expr_type = TYPE_BOOL;
            char op_str[3] = "";
            if (node->op == TOK_AND) strcpy(op_str, "&&");
            else if (node->op == TOK_OR) strcpy(op_str, "||");
            printf("Type Check: BINOP (%s) -> bool op bool = bool [OK]\n", op_str);
            return;
        }

        char op_str[3] = "";
        if (node->op == TOK_PLUS) strcpy(op_str, "+");
        else if (node->op == TOK_MINUS) strcpy(op_str, "-");
        else if (node->op == TOK_MUL) strcpy(op_str, "*");
        else if (node->op == TOK_DIV) strcpy(op_str, "/");

        if (node->left->expr_type != TYPE_INT || node->right->expr_type != TYPE_INT) {
            const char* lt = (node->left->expr_type == TYPE_BOOL) ? "bool" : "int";
            const char* rt = (node->right->expr_type == TYPE_BOOL) ? "bool" : "int";
            printf("Semantic Error: Cannot apply '%s' to %s and %s\n", op_str, lt, rt);
            exit(1);
        }
        node->expr_type = TYPE_INT; 
        printf("Type Check: BINOP (%s) -> int op int = int [OK]\n", op_str);
        return;
    }

    if (node->node_type == AST_RELOP) {
        semantic_analyze(node->left);
        semantic_analyze(node->right);
        
        char op_str[3] = "";
        if (node->op == TOK_GT) strcpy(op_str, ">");
        else if (node->op == TOK_LT) strcpy(op_str, "<");
        else if (node->op == TOK_GE) strcpy(op_str, ">=");
        else if (node->op == TOK_LE) strcpy(op_str, "<=");
        else if (node->op == TOK_EQ) strcpy(op_str, "==");

        if (node->left->expr_type != TYPE_INT || node->right->expr_type != TYPE_INT) {
            const char* lt = (node->left->expr_type == TYPE_BOOL) ? "bool" : "int";
            const char* rt = (node->right->expr_type == TYPE_BOOL) ? "bool" : "int";
            printf("Semantic Error: Cannot apply '%s' to %s and %s\n", op_str, lt, rt);
            exit(1);
        }
        node->expr_type = TYPE_BOOL; 
        printf("Type Check: RELOP (%s) -> int op int = bool [OK]\n", op_str);
        return;
    }

    printf("Semantic Error: Unknown AST Node type.\n");
    exit(1);
}
