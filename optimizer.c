#include "compiler.h"
#include <ctype.h>

int is_number(const char* str) {
    if (!str || !str[0]) return 0;
    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i]) && !(i == 0 && str[i] == '-')) return 0;
    }
    return 1;
}

void optimize() {
    // Pass 1: Constant Folding
    for (int i = 0; i < tac_count; i++) {
        TACInstr* inst = &tac_stream[i];
        if (inst->op == TAC_ADD || inst->op == TAC_SUB || 
            inst->op == TAC_MUL || inst->op == TAC_DIV) {
            
            if (is_number(inst->arg1) && is_number(inst->arg2)) {
                int v1 = atoi(inst->arg1);
                int v2 = atoi(inst->arg2);
                int res = 0;
                
                switch (inst->op) {
                    case TAC_ADD: res = v1 + v2; break;
                    case TAC_SUB: res = v1 - v2; break;
                    case TAC_MUL: res = v1 * v2; break;
                    case TAC_DIV: if (v2 != 0) res = v1 / v2; break;
                    default: break;
                }
                
                // Replace this instruction with assignment
                inst->op = TAC_ASSIGN;
                sprintf(inst->arg1, "%d", res);
                inst->arg2[0] = '\0';
                
                // We're converting `t1 = 2 + 3` to `t1 = 5` (TAC_ASSIGN)
                // In a wider optimizer we'd rename later usages of t1 to 5.
            }
        }
    }
    
    // Pass 2: Print optimized TAC output as pseudo Dead Code elimination
    // (A real compiler traverses def-use chains here)
    printf("Optimized TAC Stream:\n");
    for (int i = 0; i < tac_count; i++) {
        TACInstr in = tac_stream[i];
        if (in.op == TAC_ASSIGN) {
            printf("%s = %s\n", in.result, in.arg1);
        } else if (in.op >= TAC_ADD && in.op <= TAC_DIV) {
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
            printf("%s:\n", in.result);
        }
    }
}
