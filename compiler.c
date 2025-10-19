#include "definitions.h"
#include <stdio.h>
#include <string.h>

void emit_node(ASTNode* node, Instruction* code, int* ip) {
    switch (node->type) {
        case AST_NUMBER:
            code[(*ip)++] = (Instruction){OP_LOAD_CONST, .int_value = node->number, .operand_type = 'i'};
            break;

        case AST_VARIABLE:
            code[(*ip)++] = (Instruction){OP_LOAD_VAR, .str_value = "", .operand_type = 'v'};
            strcpy(code[*ip - 1].str_value, node->name);
            break;

        case AST_ASSIGNMENT:
            emit_node(node->assignment.value, code, ip);
            code[(*ip)++] = (Instruction){OP_STORE_VAR, .str_value = "", .operand_type = 'v'};
            strcpy(code[*ip - 1].str_value, node->assignment.name);
            break;

        case AST_BINARY_OP:
            emit_node(node->binary.left, code, ip);
            emit_node(node->binary.right, code, ip);
            switch (node->binary.op) {
                case '+': code[(*ip)++] = (Instruction){OP_ADD}; break;
                case '-': code[(*ip)++] = (Instruction){OP_SUB}; break;
                case '*': code[(*ip)++] = (Instruction){OP_MUL}; break;
                case '/': code[(*ip)++] = (Instruction){OP_DIV}; break;
                case '>': code[(*ip)++] = (Instruction){OP_GT}; break;
                case '<': code[(*ip)++] = (Instruction){OP_LT}; break;
                case '=': code[(*ip)++] = (Instruction){OP_EQ}; break;
                case '!': code[(*ip)++] = (Instruction){OP_NEQ}; break;
            }
            break;

        case AST_IF: {
            int label_id = (*ip);
            emit_node(node->if_stmt.condition, code, ip);
            code[(*ip)++] = (Instruction){OP_JMP_IF_FALSE, .int_value = label_id + 100, .operand_type = 'i'};
            emit_node(node->if_stmt.then_branch, code, ip);
            if (node->if_stmt.else_branch) {
                code[(*ip)++] = (Instruction){OP_JMP, .int_value = label_id + 200, .operand_type = 'i'};
                code[(*ip)++] = (Instruction){OP_LABEL, .int_value = label_id + 100, .operand_type = 'i'};
                emit_node(node->if_stmt.else_branch, code, ip);
                code[(*ip)++] = (Instruction){OP_LABEL, .int_value = label_id + 200, .operand_type = 'i'};
            } else {
                code[(*ip)++] = (Instruction){OP_LABEL, .int_value = label_id + 100, .operand_type = 'i'};
            }
            break;
        }

        case AST_WHILE: {
            int label_start = (*ip);
            int label_end = label_start + 100;
            code[(*ip)++] = (Instruction){OP_LABEL, .int_value = label_start, .operand_type = 'i'};
            emit_node(node->while_stmt.condition, code, ip);
            code[(*ip)++] = (Instruction){OP_JMP_IF_FALSE, .int_value = label_end, .operand_type = 'i'};
            emit_node(node->while_stmt.body, code, ip);
            code[(*ip)++] = (Instruction){OP_JMP, .int_value = label_start, .operand_type = 'i'};
            code[(*ip)++] = (Instruction){OP_LABEL, .int_value = label_end, .operand_type = 'i'};
            break;
        }

        case AST_RETURN:
            emit_node(node->return_stmt.value, code, ip);
            code[(*ip)++] = (Instruction){OP_RET};
            break;

        case AST_BLOCK:
            for (int i = 0; i < node->block.count; i++) {
                emit_node(node->block.statements[i], code, ip);
            }
            break;

        case AST_FUNCTION_CALL:
            for (int i = 0; i < node->function_call.arg_count; i++) {
                emit_node(node->function_call.args[i], code, ip);  // Emit value (e.g., string)
                code[(*ip)++] = (Instruction){OP_PUSH};            // Push it
            }
            code[(*ip)++] = (Instruction){OP_CALL, .operand_type = 's'};
            strcpy(code[*ip - 1].str_value, node->function_call.name);
            break;
                
        case AST_STRING:
            code[(*ip)++] = (Instruction){
                OP_LOAD_CONST,
                .operand_type = 's'
            };
            strcpy(code[*ip - 1].str_value, node->string);
            break;

        default:
            // Handle other types as needed
            break;
    }
}



void print_asm(Instruction* program, int count) {
    for (int i = 0; i < count; i++) {
        Instruction instr = program[i];
        printf("%04d: ", i);  // Instruction index

        switch (instr.opcode) {
            case OP_LOAD_CONST:
                if (instr.operand_type == 'i') {
                    printf("LOAD_CONST %d\n", instr.int_value);
                } else if (instr.operand_type == 's') {
                    printf("LOAD_CONST \"%s\"\n", instr.str_value);
                }
                break;
            case OP_LOAD_VAR:
                printf("LOAD_VAR %s\n", instr.str_value);
                break;
            case OP_STORE_VAR:
                printf("STORE_VAR %s\n", instr.str_value);
                break;
            case OP_ADD:
                printf("ADD\n");
                break;
            case OP_SUB:
                printf("SUB\n");
                break;
            case OP_MUL:
                printf("MUL\n");
                break;
            case OP_DIV:
                printf("DIV\n");
                break;
            case OP_EQ:
                printf("EQ\n");
                break;
            case OP_NEQ:
                printf("NEQ\n");
                break;
            case OP_GT:
                printf("GT\n");
                break;
            case OP_LT:
                printf("LT\n");
                break;
            case OP_GTE:
                printf("GTE\n");
                break;
            case OP_LTE:
                printf("LTE\n");
                break;
            case OP_JMP:
                printf("JMP %d\n", instr.int_value);
                break;
            case OP_JMP_IF_FALSE:
                printf("JMP_IF_FALSE %d\n", instr.int_value);
                break;
            case OP_LABEL:
                printf("LABEL %d\n", instr.int_value);
                break;
            case OP_CALL:
                printf("CALL %s\n", instr.str_value);
                break;
            case OP_RET:
                printf("RET\n");
                break;
            case OP_PUSH:
                printf("PUSH\n");
                break;
            case OP_POP:
                printf("POP\n");
                break;
            case OP_PRINT:
                printf("PRINT\n");
                break;
            default:
                printf("UNKNOWN OPCODE\n");
                break;
        }
    }
}

/*int main() {
    // AST for: x = 5 + 3;
    ASTNode number5 = {.type = AST_NUMBER, .number = 5};
    ASTNode number3 = {.type = AST_NUMBER, .number = 3};
    ASTNode binary_add = {
        .type = AST_BINARY_OP,
        .binary = {
            .op = '+',
            .left = &number5,
            .right = &number3
        }
    };
    ASTNode assignment = {
        .type = AST_ASSIGNMENT,
        .assignment = {
            .name = "x",
            .value = &binary_add
        }
    };

    // Emit and print
    Instruction program[32];
    int ip = 0;
    emit_node(&assignment, program, &ip);
    print_asm(program, ip);
    return 0;
}
*/