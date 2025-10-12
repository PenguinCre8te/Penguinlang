#include "token.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
Token tokens[MAX_TOKENS];
int token_count = 0;
int column = 0;
int line = 0;

void add_token(TokenType type, const char* lexeme, int value, char op, int line, int column) {
    Token t;
    t.type = type;
    strncpy(t.lexeme, lexeme, sizeof(t.lexeme));
    t.lexeme[sizeof(t.lexeme) - 1] = '\0';  // Ensure null-termination
    t.value = value;
    t.op = op;
    t.line = line;
    t.column = column;
    tokens[token_count++] = t;
}

int is_keyword(const char* word) {
    if (strcmp(word, "if") == 0) return TOKEN_KEYWORD_IF;
    if (strcmp(word, "else") == 0) return TOKEN_KEYWORD_ELSE;
    if (strcmp(word, "while") == 0) return TOKEN_KEYWORD_WHILE;
    if (strcmp(word, "func") == 0) return TOKEN_KEYWORD_FUNC;
    if (strcmp(word, "return") == 0) return TOKEN_KEYWORD_RETURN;  // ✅ Add this line
    return 0;
}

void tokenize(const char* input) {
    int i = 0;
    int line = 1;
    int column = 1;

    while (input[i] != '\0') {
        // Handle newlines
        if (input[i] == '\n') {
            line++;
            column = 1;
            i++;
            continue;
        }

        // Skip whitespace
        if (isspace(input[i])) {
            column++;
            i++;
            continue;
        }

        // Numbers
        if (isdigit(input[i])) {
            int start = i;
            int start_col = column;
            while (isdigit(input[i])) {
                i++;
                column++;
            }
            char num[32];
            strncpy(num, input + start, i - start);
            num[i - start] = '\0';
            add_token(TOKEN_NUMBER, num, atoi(num), 0, line, start_col);
            continue;
        }

        // Identifiers or keywords
        if (isalpha(input[i])) {
            int start = i;
            int start_col = column;
            while (isalnum(input[i])) {
                i++;
                column++;
            }
            char id[32];
            strncpy(id, input + start, i - start);
            id[i - start] = '\0';

            int kw = is_keyword(id);
            if (kw) {
                add_token(kw, id, 0, 0, line, start_col);
            } else {
                add_token(TOKEN_IDENTIFIER, id, 0, 0, line, start_col);
            }
            continue;
        }

        // Two-character operators
        if (input[i] == '>' && input[i + 1] == '=') {
            add_token(TOKEN_GTE, ">=", 0, 0, line, column);
            i += 2;
            column += 2;
            continue;
        }
        if (input[i] == '<' && input[i + 1] == '=') {
            add_token(TOKEN_LTE, "<=", 0, 0, line, column);
            i += 2;
            column += 2;
            continue;
        }
        if (input[i] == '=' && input[i + 1] == '=') {
            add_token(TOKEN_EQ, "==", 0, 0, line, column);
            i += 2;
            column += 2;
            continue;
        }
        if (input[i] == '!' && input[i + 1] == '=') {
            add_token(TOKEN_NEQ, "!=", 0, 0, line, column);
            i += 2;
            column += 2;
            continue;
        }

        // Single-character tokens
        char ch = input[i];
        switch (ch) {
            case '+': case '-': case '*': case '/':
                add_token(TOKEN_OPERATOR, (char[]){ch, '\0'}, 0, ch, line, column);
                break;
            case '=':
                add_token(TOKEN_ASSIGN, "=", 0, '=', line, column);
                break;
            case '(': add_token(TOKEN_PAREN_OPEN, "(", 0, 0, line, column); break;
            case ')': add_token(TOKEN_PAREN_CLOSE, ")", 0, 0, line, column); break;
            case '{': add_token(TOKEN_BRACE_OPEN, "{", 0, 0, line, column); break;
            case '}': add_token(TOKEN_BRACE_CLOSE, "}", 0, 0, line, column); break;
            case ';': add_token(TOKEN_SEMICOLON, ";", 0, 0, line, column); break;
            case ',': add_token(TOKEN_COMMA, ",", 0, 0, line, column); break;
            case '!':
                printf("TOKENIZER: Unexpected character '!' at line %d, column %d\n", line, column);
                exit(1);
            case '"': case '\'':
            {
                char quote = input[i++];
                int start_col = column++;
                char str[256];
                int j = 0;

                while (input[i] != '\0' && input[i] != quote) {
                    if (input[i] == '\\') {
                        i++;
                        column++;
                        switch (input[i]) {
                            case 'n': str[j++] = '\n'; break;
                            case 't': str[j++] = '\t'; break;
                            case '\\': str[j++] = '\\'; break;
                            case '"': str[j++] = '"'; break;
                            case '\'': str[j++] = '\''; break;
                            default: str[j++] = input[i]; break;
                        }
                    } else {
                        str[j++] = input[i];
                    }
                    i++;
                    column++;
                }

                if (input[i] != quote) {
                    printf("TOKENIZER: Unterminated string literal at line %d, column %d\n", line, start_col);
                    exit(1);
                }

                str[j] = '\0';
                add_token(TOKEN_STRING, str, 0, 0, line, start_col);
                // Do NOT increment i/column again here — let the main loop handle it
            }
            default:
                printf("TOKENIZER: Unknown character '%c' at line %d, column %d\n", ch, line, column);
                exit(1);
        }

        i++;
        column++;
    }

    add_token(TOKEN_EOF, "", 0, 0, line, column);
}

/*
int main() {
    const char* code = "func add(a, b) {return a+b;}";
    tokenize(code);

    for (int i = 0; i < token_count; i++) {
        Token t = tokens[i];
        printf("Type: %d, Lexeme: '%s'\n", t.type, t.lexeme);
    }

    return 0;
}
*/