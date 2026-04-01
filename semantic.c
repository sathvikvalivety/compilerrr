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
        
        if (node->left->expr_type != TYPE_INT || node->right->expr_type != TYPE_INT) {
            printf("Semantic Error: Binary operator requires INT operands.\n");
            exit(1);
        }
        node->expr_type = TYPE_INT; // e.g. a + b yields INT
        return;
    }

    if (node->node_type == AST_RELOP) {
        semantic_analyze(node->left);
        semantic_analyze(node->right);
        
        if (node->left->expr_type != TYPE_INT || node->right->expr_type != TYPE_INT) {
            printf("Semantic Error: Relational operator requires INT operands.\n");
            exit(1);
        }
        node->expr_type = TYPE_BOOL; // e.g. a > b yields BOOL
        return;
    }

    printf("Semantic Error: Unknown AST Node type.\n");
    exit(1);
}
