#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "definitions.h"

#define STACK_SIZE 1024
#define MAX_VARS 256

int stack[STACK_SIZE];
int sp = -1;

Variable vars[MAX_VARS];
int var_count = 0;

#define MAX_FUNCS 64

typedef struct {
    char name[32];
    int address; // instruction index of function entry
} FunctionLabel;

FunctionLabel func_table[MAX_FUNCS];
int func_count = 0;

int get_func_address(const char* name) {
    for (int i = 0; i < func_count; i++) {
        if (strcmp(func_table[i].name, name) == 0) {
            return func_table[i].address;
        }
    }
    printf("VM ERROR: Unknown function '%s'\n", name);
    exit(1);
}

#define CALL_STACK_SIZE 256
int call_stack[CALL_STACK_SIZE];
int call_sp = -1;

void push_call(int addr) {
    if (call_sp >= CALL_STACK_SIZE - 1) {
        printf("VM ERROR: Call stack overflow\n");
        exit(1);
    }
    call_stack[++call_sp] = addr;
}

int pop_call() {
    if (call_sp < 0) {
        printf("VM ERROR: Call stack underflow\n");
        exit(1);
    }
    return call_stack[call_sp--];
}

int get_var_index(const char* name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) return i;
    }
    return -1;
}

void set_var(const char* name, int value) {
    int idx = get_var_index(name);
    if (idx >= 0) {
        vars[idx].value = value;
    } else {
        strcpy(vars[var_count].name, name);
        vars[var_count].value = value;
        var_count++;
    }
}

int get_var(const char* name) {
    int idx = get_var_index(name);
    if (idx >= 0) return vars[idx].value;
    printf("VM ERROR: Undefined variable '%s'\n", name);
    exit(1);
}

void push(int value) {
    if (sp >= STACK_SIZE - 1) {
        printf("VM ERROR: Stack overflow\n");
        exit(1);
    }
    stack[++sp] = value;
}

int pop() {
    if (sp < 0) {
        printf("VM ERROR: Stack underflow\n");
        exit(1);
    }
    return stack[sp--];
}

void call_builtin(const char* name, int arg_count) {
    if (strcmp(name, "print") == 0) {
        for (int i = arg_count - 1; i >= 0; i--) {
            int val = pop();
            printf("%d", val);
            if (i > 0) printf(" ");
        }
        printf("\n");
    } else {
        printf("VM ERROR: Unknown built-in function '%s'\n", name);
        exit(1);
    }
}

void run_vm(Instruction* code, int count) {
    int ip = 0;
    while (ip < count) {
        Instruction instr = code[ip++];
        switch (instr.opcode) {
            case OP_LOAD_CONST:
                push(instr.int_value);
                break;
            case OP_PUSH:
                push(instr.int_value);
                break;
            case OP_POP:
                pop();
                break;
            case OP_LOAD_VAR:
                push(get_var(instr.str_value));
                break;
            case OP_STORE_VAR: {
                int val = pop();
                set_var(instr.str_value, val);
                break;
            }
            case OP_ADD: {
                int b = pop();
                int a = pop();
                push(a + b);
                break;
            }
            case OP_SUB: {
                int b = pop();
                int a = pop();
                push(a - b);
                break;
            }
            case OP_MUL: {
                int b = pop();
                int a = pop();
                push(a * b);
                break;
            }
            case OP_DIV: {
                int b = pop();
                int a = pop();
                if (b == 0) {
                    printf("VM ERROR: Division by zero\n");
                    exit(1);
                }
                push(a / b);
                break;
            }
            case OP_EQ: {
                int b = pop();
                int a = pop();
                push(a == b);
                break;
            }
            case OP_NEQ: {
                int b = pop();
                int a = pop();
                push(a != b);
                break;
            }
            case OP_GT: {
                int b = pop();
                int a = pop();
                push(a > b);
                break;
            }
            case OP_LT: {
                int b = pop();
                int a = pop();
                push(a < b);
                break;
            }
            case OP_GTE: {
                int b = pop();
                int a = pop();
                push(a >= b);
                break;
            }
            case OP_LTE: {
                int b = pop();
                int a = pop();
                push(a <= b);
                break;
            }
            case OP_PRINT: {
                int val = pop();
                printf("%d\n", val);
                break;
            }
            case OP_JMP:
                ip = instr.int_value;
                break;
            case OP_JMP_IF_FALSE: {
                int cond = pop();
                if (!cond) ip = instr.int_value;
                break;
            }
            case OP_LABEL:
                // No-op for now
                break;
            case OP_CALL: {
                if (strcmp(instr.str_value, "print") == 0) {
                    int val = pop();
                    printf("%d\n", val);
                } else {
                    int addr = get_func_address(instr.str_value);
                    push_call(ip);
                    ip = addr;
                }
                break;
            }
            case OP_RET: {
                ip = pop_call();
                break;
            }
            default:
                printf("VM ERROR: Unknown opcode %d\n", instr.opcode);
                exit(1);
        }
    }
}