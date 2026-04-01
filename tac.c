#include "compiler.h"
#include <ctype.h>

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

void add_tac_comment(TACOp op, const char* arg1, const char* arg2, const char* result, const char* comment) {
    TACInstr inst;
    inst.op = op;
    if (arg1) strcpy(inst.arg1, arg1); else inst.arg1[0] = '\0';
    if (arg2) strcpy(inst.arg2, arg2); else inst.arg2[0] = '\0';
    if (result) strcpy(inst.result, result); else inst.result[0] = '\0';
    if (comment) strcpy(inst.comment, comment); else inst.comment[0] = '\0';
    tac_stream[tac_count++] = inst;
}

void add_tac(TACOp op, const char* arg1, const char* arg2, const char* result) {
    add_tac_comment(op, arg1, arg2, result, NULL);
}

int is_numeric_str(const char* str) {
    if (!str || !str[0]) return 0;
    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i]) && !(i == 0 && str[i] == '-')) return 0;
    }
    return 1;
}

char* generate_tac_expr(ASTNode* node) {
    if (!node) return NULL;

    if (node->node_type == AST_ID) return node->name;

    if (node->node_type == AST_NUM) {
        char* v = (char*)malloc(16);
        sprintf(v, "%d", node->value);
        return v;
    }

    if (node->node_type == AST_BINOP && node->op != TOK_AND && node->op != TOK_OR) {
        char* tleft = generate_tac_expr(node->left);
        char* tright = generate_tac_expr(node->right);
        
        // Constant Folding logic
        if (is_numeric_str(tleft) && is_numeric_str(tright)) {
            int v1 = atoi(tleft);
            int v2 = atoi(tright);
            int res = 0;
            if (node->op == TOK_PLUS) res = v1 + v2;
            else if (node->op == TOK_MINUS) res = v1 - v2;
            else if (node->op == TOK_MUL) res = v1 * v2;
            else if (node->op == TOK_DIV) res = (v2 != 0) ? v1 / v2 : 0;
            
            char* folded = (char*)malloc(16);
            sprintf(folded, "%d", res);
            return folded;
        }

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
    
    return NULL;
}

void generate_bool_tac(ASTNode* node, const char* label_true, const char* label_false) {
    if (!node) return;

    if (node->node_type == AST_BINOP && node->op == TOK_OR) {
        char* l_next = new_label();
        generate_bool_tac(node->left, label_true, l_next);
        add_tac_comment(TAC_LABEL, NULL, NULL, l_next, "OR short-circuit check");
        generate_bool_tac(node->right, label_true, label_false);
        return;
    }

    if (node->node_type == AST_BINOP && node->op == TOK_AND) {
        char* l_next = new_label();
        generate_bool_tac(node->left, l_next, label_false);
        add_tac_comment(TAC_LABEL, NULL, NULL, l_next, "AND short-circuit progression");
        generate_bool_tac(node->right, label_true, label_false);
        return;
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
        
        add_tac(op, tleft, tright, label_true);
        add_tac(TAC_GOTO, NULL, NULL, label_false);
        return;
    }
    
    char* val = generate_tac_expr(node);
    add_tac(TAC_GT, val, "0", label_true); 
    add_tac(TAC_GOTO, NULL, NULL, label_false);
}

void generate_tac(ASTNode* node) {
    if (!node) return;
    
    if (node->expr_type == TYPE_BOOL || 
       (node->node_type == AST_BINOP && (node->op == TOK_AND || node->op == TOK_OR)) || 
       node->node_type == AST_RELOP) {
       
        char* label_true = new_label();
        char* label_false = new_label();
        
        generate_bool_tac(node, label_true, label_false);
        
        add_tac_comment(TAC_LABEL, NULL, NULL, label_true, "TRUE branch");
        add_tac_comment(TAC_LABEL, NULL, NULL, label_false, "FALSE branch");
    } else {
        generate_tac_expr(node);
    }
}
