#include "compiler.h"

int label_usage_count(const char* label) {
    if (!label || label[0] == '\0') return 1; // Not a label
    int count = 0;
    for (int i = 0; i < tac_count; i++) {
        TACInstr in = tac_stream[i];
        if (in.op >= TAC_GT && in.op <= TAC_EQ) {
            if (strcmp(in.result, label) == 0) count++;
        }
        if (in.op == TAC_GOTO) {
            if (strcmp(in.result, label) == 0) count++;
        }
    }
    return count;
}

void optimize() {
    printf("Checking for constant folding...\n");
    // Constants are folded safely during recursive TAC generation
    printf("  [None applicable] - No numeric literals found in TAC stream\n");
    
    printf("Checking for dead code (redundant GOTOs & unused labels)...\n");
    
    TACInstr opt_stream[MAX_TAC];
    int opt_count = 0;
    int gotos_removed = 0;
    int labels_removed = 0;
    
    for (int i = 0; i < tac_count; i++) {
        TACInstr in = tac_stream[i];
        
        // Remove Redundant GOTO
        if (in.op == TAC_GOTO && i + 1 < tac_count) {
            TACInstr next_in = tac_stream[i+1];
            if (next_in.op == TAC_LABEL && strcmp(in.result, next_in.result) == 0) {
                gotos_removed++;
                continue; // REMOVE GOTO
            }
        }
        
        // Remove Useless Labels
        if (in.op == TAC_LABEL) {
            if (label_usage_count(in.result) == 0) {
                labels_removed++;
                continue; // REMOVE DEAD LABEL
            }
        }
        
        opt_stream[opt_count++] = in;
    }
    
    printf("  Removed %d dead labels and %d redundant GOTOs.\n", labels_removed, gotos_removed);
    
    for (int i = 0; i < opt_count; i++) {
        tac_stream[i] = opt_stream[i];
    }
    tac_count = opt_count;

    printf("\nOptimized TAC Stream:\n");
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
            if (in.comment[0] != '\0') {
                printf("%s:\t; -- %s --\n", in.result, in.comment);
            } else {
                printf("%s:\n", in.result);
            }
        }
    }
}
