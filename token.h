#define MAX_TOKENS 1000

void tokenize(const char* input);



typedef enum {
    TOKEN_NUMBER, //type 0
    TOKEN_IDENTIFIER, //type 1
    TOKEN_OPERATOR, //type 2
    TOKEN_ASSIGN, //type 3
    TOKEN_PAREN_OPEN, //type 4
    TOKEN_PAREN_CLOSE, //type 5
    TOKEN_BRACE_OPEN, //type 6
    TOKEN_BRACE_CLOSE, //type 7
    TOKEN_SEMICOLON, //type 8
    TOKEN_KEYWORD_IF, //type 9
    TOKEN_KEYWORD_ELSE, //type 10
    TOKEN_KEYWORD_WHILE, //type 11
    TOKEN_KEYWORD_FUNC, //type 12
    TOKEN_GT, //type 13, >
    TOKEN_LT, //type 14, <
    TOKEN_EQ, //type 15, ==
    TOKEN_NEQ, //type 16, !=
    TOKEN_GTE, //type 17, >=
    TOKEN_LTE, //type 18, <=
    TOKEN_STRING, //type 19
    TOKEN_EOF, //type 20
    TOKEN_COMMA, //type 21, for func call arg
    TOKEN_KEYWORD_RETURN // type 22, for func return
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[32];
    int value;
    char op;
    int line;     // ✅ NEW: line number
    int column;   // ✅ NEW: character position in line
} Token;

typedef enum {
    AST_NUMBER,
    AST_STRING,
    AST_VARIABLE,
    AST_BINARY_OP,
    AST_ASSIGNMENT,
    AST_IF,
    AST_WHILE,
    AST_FUNCTION_DEF,
    AST_FUNCTION_CALL,
    AST_BLOCK,
    AST_RETURN
} ASTNodeType;
typedef struct ASTNode ASTNode;
typedef struct ASTNode {
    ASTNodeType type;
    union {
        int number;
        char string[256];
        char name[32];
        struct {
            char op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary;
        struct {
            char name[32];
            struct ASTNode* value;
        } assignment;
        struct {
            struct ASTNode* condition;
            struct ASTNode* then_branch;
            struct ASTNode* else_branch;
        } if_stmt;
        struct {
            struct ASTNode* condition;
            struct ASTNode* body;
        } while_stmt;
        struct {
            char name[32];
            char** params;           // array of parameter names
            int param_count;
            struct ASTNode* body;
        } function_def;
        struct {
            char name[32];
            struct ASTNode** args;  // array of arguments
            int arg_count;
        } function_call;
        struct {
            struct ASTNode** statements;
            int count;
        } block;
        struct {
            struct ASTNode* value;
        } return_stmt;
    };
} ASTNode;

extern Token tokens[];