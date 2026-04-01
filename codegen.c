#include "compiler.h"

void generate_x86() {
    printf("\n--- Final x86 Pseudo-Assembly ---\n");

    char current_reg_value[64] = "";

    for (int i = 0; i < tac_count; i++) {
        TACInstr in = tac_stream[i];

        if (in.op == TAC_ADD || in.op == TAC_SUB || in.op == TAC_MUL || in.op == TAC_DIV) {
            if (strcmp(current_reg_value, in.arg1) != 0) {
                printf("MOV eax, %s\n", in.arg1);
            }
            
            if (in.op == TAC_ADD) {
                printf("ADD eax, %s\n", in.arg2);
            } else if (in.op == TAC_SUB) {
                printf("SUB eax, %s\n", in.arg2);
            } else if (in.op == TAC_MUL) {
                printf("IMUL eax, %s\n", in.arg2);
            } else if (in.op == TAC_DIV) {
                printf("MOV ebx, %s\n", in.arg2);
                printf("CDQ\n");
                printf("IDIV ebx\n");
            }
            printf("MOV %s, eax\n", in.result);
            strcpy(current_reg_value, in.result);
            
        } else if (in.op == TAC_ASSIGN) {
            if (strcmp(current_reg_value, in.arg1) != 0) {
                printf("MOV eax, %s\n", in.arg1);
            }
            printf("MOV %s, eax\n", in.result);
            strcpy(current_reg_value, in.result);
            
        } else if (in.op >= TAC_GT && in.op <= TAC_EQ) {
            if (strcmp(current_reg_value, in.arg1) != 0) {
                printf("MOV eax, %s\n", in.arg1);
            }
            printf("CMP eax, %s\n", in.arg2);
            strcpy(current_reg_value, ""); // Reset register assumed state due to potential jump branch issues
            
            char jump_instr[4] = "";
            if (in.op == TAC_GT) strcpy(jump_instr, "JG");
            if (in.op == TAC_LT) strcpy(jump_instr, "JL");
            if (in.op == TAC_GE) strcpy(jump_instr, "JGE");
            if (in.op == TAC_LE) strcpy(jump_instr, "JLE");
            if (in.op == TAC_EQ) strcpy(jump_instr, "JE");
            
            printf("%s %s\n", jump_instr, in.result);
        } else if (in.op == TAC_GOTO) {
            printf("JMP %s\n", in.result);
            strcpy(current_reg_value, ""); // Reset on jump
        } else if (in.op == TAC_LABEL) {
            if (in.comment[0] != '\0') {
                printf("%s:\t; -- %s --\n", in.result, in.comment);
            } else {
                printf("%s:\n", in.result);
            }
            strcpy(current_reg_value, ""); // Reset on label entry
        }
    }
}
