#include "compiler.h"

TACInstr tac_stream[MAX_TAC];
int tac_count = 0;

int temp_idx = 1;
int label_idx = 1;

char* new_temp() {
    char* t = (char*)malloc(16);
    sprintf(t, "t%d", temp_idx++);
    return t;
}

char* new_label() {
    char* l = (char*)malloc(16);
    sprintf(l, "L%d", label_idx++);
    return l;
}

void add_tac(TACOp op, const char* arg1, const char* arg2, const char* result) {
    TACInstr inst;
    inst.op = op;
    if (arg1) strcpy(inst.arg1, arg1); else inst.arg1[0] = '\0';
    if (arg2) strcpy(inst.arg2, arg2); else inst.arg2[0] = '\0';
    if (result) strcpy(inst.result, result); else inst.result[0] = '\0';
    tac_stream[tac_count++] = inst;
}

char* generate_tac_expr(ASTNode* node) {
    if (!node) return NULL;

    if (node->node_type == AST_ID) {
        return node->name;
    }

    if (node->node_type == AST_NUM) {
        char* v = (char*)malloc(16);
        sprintf(v, "%d", node->value);
        return v;
    }

    if (node->node_type == AST_BINOP) {
        char* tleft = generate_tac_expr(node->left);
        char* tright = generate_tac_expr(node->right);
        
        TACOp op;
        switch (node->op) {
            case TOK_PLUS:  op = TAC_ADD; break;
            case TOK_MINUS: op = TAC_SUB; break;
            case TOK_MUL:   op = TAC_MUL; break;
            case TOK_DIV:   op = TAC_DIV; break;
            default: op = TAC_ADD;
        }

        char* res = new_temp();
        add_tac(op, tleft, tright, res);
        return res;
    }

    if (node->node_type == AST_RELOP) {
        char* tleft = generate_tac_expr(node->left);
        char* tright = generate_tac_expr(node->right);
        
        TACOp op;
        switch (node->op) {
            case TOK_GT: op = TAC_GT; break;
            case TOK_LT: op = TAC_LT; break;
            case TOK_GE: op = TAC_GE; break;
            case TOK_LE: op = TAC_LE; break;
            case TOK_EQ: op = TAC_EQ; break;
            default: op = TAC_GT;
        }
        
        char* label_true = new_label();
        char* label_false = new_label();
        
        // Emitting `if tleft op tright goto label_true`
        add_tac(op, tleft, tright, label_true);
        // Emitting `goto label_false`
        add_tac(TAC_GOTO, NULL, NULL, label_false);

        // Normally backpatching sets these up or returns them for upper layers.
        // For simple nested output, we print the labels right here to demonstrate control flow.
        add_tac(TAC_LABEL, NULL, NULL, label_true);
        // ... Code for true block would go here
        add_tac(TAC_LABEL, NULL, NULL, label_false);
        // ... Code for false block
        
        // Relational operator doesn't return a value register in this simple scheme, 
        // it controls layout, or we can just return a boolean register. Let's return the left side 
        // just to not crash, though in reality it resolves to branches.
        return NULL;
    }
    
    return NULL;
}

void generate_tac(ASTNode* node) {
    if (!node) return;
    generate_tac_expr(node);
}
