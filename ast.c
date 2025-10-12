#include <stdio.h>
#include "token.h"
#include <stdlib.h>
#include <string.h>

// Token stream helpers
Token peek(void);
Token advance(void);
int match(TokenType type);
Token expect(TokenType type);
ASTNode* parse_return();

// AST node creation helpers (optional but useful to declare if separated)
ASTNode* create_number_node(int value);
ASTNode* create_string_node(const char* str);
ASTNode* create_variable_node(const char* name);
ASTNode* create_binary_node(char op, ASTNode* left, ASTNode* right);
ASTNode* create_assignment_node(const char* name, ASTNode* value);
ASTNode* create_if_node(ASTNode* cond, ASTNode* then_branch, ASTNode* else_branch);
ASTNode* create_while_node(ASTNode* cond, ASTNode* body);
ASTNode* create_function_def_node(const char* name, ASTNode* body);
ASTNode* create_function_call_node(const char* name);
ASTNode* create_block_node(ASTNode** statements, int count);

// Parser entry points
ASTNode* parse_program(void);
ASTNode* parse_block(void);
ASTNode* parse_statement(void);
ASTNode* parse_expression(void);
ASTNode* parse_primary(void);
ASTNode* parse_binary_op(int min_prec);

// Specific statement parsers
ASTNode* parse_assignment(void);
ASTNode* parse_function_def(void);
ASTNode* parse_if(void);
ASTNode* parse_while(void);

char* tokentypes[] = {
    "TOKEN_NUMBER", //type 0
    "TOKEN_IDENTIFIER", //type 1
    "TOKEN_OPERATOR", //type 2
    "TOKEN_ASSIGN", //type 3
    "TOKEN_PAREN_OPEN", //type 4
    "TOKEN_PAREN_CLOSE", //type 5
    "TOKEN_BRACE_OPEN", //type 6
    "TOKEN_BRACE_CLOSE", //type 7
    "TOKEN_SEMICOLON", //type 8
    "TOKEN_KEYWORD_IF", //type 9
    "TOKEN_KEYWORD_ELSE", //type 10
    "TOKEN_KEYWORD_WHILE", //type 11
    "TOKEN_KEYWORD_FUNC", //type 12
    "TOKEN_GT", //type 13, >
    "TOKEN_LT", //type 14, <
    "TOKEN_EQ", //type 15, ==
    "TOKEN_NEQ", //type 16, !=
    "TOKEN_GTE", //type 17, >=
    "TOKEN_LTE", //type 18, <=
    "TOKEN_STRING", //type 19
    "TOKEN_EOF", //type 20
    "TOKEN_COMMA", //type 21, for func call arg
    "TOKEN_KEYWORD_RETURN" // type 22, for func return
    };

int current = 0;

Token peek() {
    return tokens[current];
}

Token advance() {
    return tokens[current++];
}

int match(TokenType type) {
    if (peek().type == type) {
        advance();
        return 1;
    }
    return 0;
}

Token expect(TokenType type) {
    if (!match(type)) {
        Token t = peek();
        printf("PARSER Syntax error: expected token type %s, got %s ('%s') at line %d, column %d\n",
               tokentypes[type], tokentypes[t.type], t.lexeme, t.line, t.column);
        exit(1);
    }
    return tokens[current - 1];  // return the token just matched
}

ASTNode* parse_primary() {
    Token t = advance();
    ASTNode* node = malloc(sizeof(ASTNode));

    if (t.type == TOKEN_NUMBER) {
        node->type = AST_NUMBER;
        node->number = t.value;
    } else if (t.type == TOKEN_STRING) {
        node->type = AST_STRING;
        strcpy(node->string, t.lexeme);
    } else if (t.type == TOKEN_IDENTIFIER) {
        if (peek().type == TOKEN_PAREN_OPEN) {
    advance(); // skip '('

    ASTNode** args = malloc(sizeof(ASTNode*) * 10);
    int count = 0;

    if (peek().type != TOKEN_PAREN_CLOSE) {
            do {
                args[count++] = parse_expression();
            } while (match(TOKEN_COMMA));
        }

        expect(TOKEN_PAREN_CLOSE);  // ✅ THIS IS CRUCIAL

        node->type = AST_FUNCTION_CALL;
        strcpy(node->function_call.name, t.lexeme);
        node->function_call.args = args;
        node->function_call.arg_count = count;
    } else {
        node->type = AST_VARIABLE;
        strcpy(node->name, t.lexeme);
    }
    } else {
        printf("PARSER: Unexpected token in expression: %s\n", t.lexeme);
        exit(1);
    }

    return node;
}

ASTNode* parse_binary_op(int min_prec) {
    ASTNode* left = parse_primary();

    while (peek().type == TOKEN_OPERATOR || peek().type >= TOKEN_GT) {
        Token op_token = advance();
        char op = op_token.op;
        ASTNode* right = parse_primary();

        ASTNode* bin = malloc(sizeof(ASTNode));
        bin->type = AST_BINARY_OP;
        bin->binary.op = op;
        bin->binary.left = left;
        bin->binary.right = right;
        left = bin;
    }

    return left;
}

ASTNode* parse_expression() {
    return parse_binary_op(1);
}

ASTNode* parse_assignment() {
    Token var = advance();
    expect(TOKEN_ASSIGN);
    ASTNode* value = parse_expression();
    expect(TOKEN_SEMICOLON);

    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGNMENT;
    strcpy(node->assignment.name, var.lexeme);
    node->assignment.value = value;
    return node;
}

ASTNode* parse_function_def() {
    expect(TOKEN_KEYWORD_FUNC);

    Token name = expect(TOKEN_IDENTIFIER);

    expect(TOKEN_PAREN_OPEN);

    char** params = malloc(sizeof(char*) * 10);
    int count = 0;

    if (peek().type != TOKEN_PAREN_CLOSE) {
        do {
            Token param = expect(TOKEN_IDENTIFIER);
            params[count] = malloc(strlen(param.lexeme) + 1);
            strcpy(params[count++], param.lexeme);
        } while (match(TOKEN_COMMA));
    }

    expect(TOKEN_PAREN_CLOSE);

    ASTNode* body = parse_block();

    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_FUNCTION_DEF;
    strcpy(node->function_def.name, name.lexeme);
    node->function_def.params = params;
    node->function_def.param_count = count;
    node->function_def.body = body;

    return node;
}

ASTNode* parse_if() {
    advance(); // skip 'if'
    expect(TOKEN_PAREN_OPEN);
    ASTNode* condition = parse_expression();
    expect(TOKEN_PAREN_CLOSE);
    ASTNode* then_branch = parse_block();

    ASTNode* else_branch = NULL;
    if (match(TOKEN_KEYWORD_ELSE)) {
        else_branch = parse_block();
    }

    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IF;
    node->if_stmt.condition = condition;
    node->if_stmt.then_branch = then_branch;
    node->if_stmt.else_branch = else_branch;
    return node;
}

ASTNode* parse_while() {
    advance(); // skip 'while'
    expect(TOKEN_PAREN_OPEN);
    ASTNode* condition = parse_expression();
    expect(TOKEN_PAREN_CLOSE);
    ASTNode* body = parse_block();

    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_WHILE;
    node->while_stmt.condition = condition;
    node->while_stmt.body = body;
    return node;
}

ASTNode* parse_statement() {
    if (peek().type == TOKEN_KEYWORD_FUNC) {
        return parse_function_def();
    } else if (peek().type == TOKEN_KEYWORD_IF) {
        return parse_if();
    } else if (peek().type == TOKEN_KEYWORD_WHILE) {
        return parse_while();
    } else if (peek().type == TOKEN_KEYWORD_RETURN) {
        return parse_return();  // ✅ NEW: handles return statements
    } else if (peek().type == TOKEN_IDENTIFIER && tokens[current + 1].type == TOKEN_ASSIGN) {
        return parse_assignment();
    } else {
        ASTNode* expr = parse_expression();
        expect(TOKEN_SEMICOLON);
        return expr;
    }
}

ASTNode* parse_block() {
    expect(TOKEN_BRACE_OPEN);
    ASTNode** stmts = malloc(sizeof(ASTNode*) * 100);
    int count = 0;

    while (peek().type != TOKEN_BRACE_CLOSE && peek().type != TOKEN_EOF) {
        stmts[count++] = parse_statement();
    }

    expect(TOKEN_BRACE_CLOSE);

    ASTNode* block = malloc(sizeof(ASTNode));
    block->type = AST_BLOCK;
    block->block.statements = stmts;
    block->block.count = count;
    return block;
}

void print_ast(ASTNode* node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) printf("  ");

    switch (node->type) {
        case AST_NUMBER:
            printf("Number: %d\n", node->number);
            break;
        case AST_STRING:
            printf("String: \"%s\"\n", node->string);
            break;
        case AST_VARIABLE:
            printf("Variable: %s\n", node->name);
            break;
        case AST_ASSIGNMENT:
            printf("Assignment: %s =\n", node->assignment.name);
            print_ast(node->assignment.value, indent + 1);
            break;
        case AST_BINARY_OP:
            printf("BinaryOp: %c\n", node->binary.op);
            print_ast(node->binary.left, indent + 1);
            print_ast(node->binary.right, indent + 1);
            break;
        case AST_FUNCTION_CALL:
            printf("FunctionCall: %s()\n", node->function_call.name);
            for (int i = 0; i < node->function_call.arg_count; i++) {
                print_ast(node->function_call.args[i], indent + 1);
            }
            break;
        case AST_FUNCTION_DEF:
            printf("FunctionDef: %s(", node->function_def.name);
            for (int i = 0; i < node->function_def.param_count; i++) {
                printf("%s", node->function_def.params[i]);
                if (i < node->function_def.param_count - 1) printf(", ");
            }
            printf(")\n");
            print_ast(node->function_def.body, indent + 1);
            break;
        case AST_IF:
            printf("If:\n");
            print_ast(node->if_stmt.condition, indent + 1);
            printf("Then:\n");
            print_ast(node->if_stmt.then_branch, indent + 1);
            if (node->if_stmt.else_branch) {
                printf("Else:\n");
                print_ast(node->if_stmt.else_branch, indent + 1);
            }
            break;
        case AST_WHILE:
            printf("While:\n");
            print_ast(node->while_stmt.condition, indent + 1);
            print_ast(node->while_stmt.body, indent + 1);
            break;
        case AST_BLOCK:
            printf("Block:\n");
            for (int i = 0; i < node->block.count; i++) {
                print_ast(node->block.statements[i], indent + 1);
            }
            break;
        case AST_RETURN:
            printf("%*sReturn:\n", indent * 2, "");
            print_ast(node->return_stmt.value, indent + 1);
            break;
        default:
            printf("Unknown AST node type\n");
    }
}

ASTNode* parse_program(void) {
    ASTNode** stmts = malloc(sizeof(ASTNode*) * 100);  // support up to 100 statements
    int count = 0;

    while (peek().type != TOKEN_EOF) {
        stmts[count++] = parse_statement();
    }

    ASTNode* block = malloc(sizeof(ASTNode));
    block->type = AST_BLOCK;
    block->block.statements = stmts;
    block->block.count = count;
    return block;
}

ASTNode* parse_return() {
    expect(TOKEN_KEYWORD_RETURN);

    ASTNode* value = parse_expression();

    expect(TOKEN_SEMICOLON);  // ✅ This is the missing piece!

    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_RETURN;
    node->return_stmt.value = value;

    return node;
}
extern int token_count;
int main() {
    const char* code =
        "print(\"penguin\");";

    tokenize(code);  // from tokenizer.c
    printf("\n=== TOKENS ===\n");
    for (int i = 0; i < token_count; i++) {
        Token t = tokens[i];
        printf("Type: %d, Lexeme: '%s'\n", t.type, t.lexeme);
    }
    ASTNode* program = parse_program();  // from parser.c

    printf("\n=== AST ===\n");
    print_ast(program, 0);

    return 0;
}