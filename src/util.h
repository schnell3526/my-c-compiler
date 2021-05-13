#ifndef UTIL_H
#define UTIL_H
#include <stdbool.h>

typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM
} NodeKind;

typedef enum {
    TK_RESERVED,    // sign
    TK_NUM,         // integer token
    TK_EOF,         // A token representing the end of the input string.
} TokenKind;

typedef struct Node Node;

struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

typedef struct Token Token;

struct Token{
    TokenKind kind; // type of token
    Token *next;    // next token
    int val;        // number
    char *str;      // token string
};

Token *new_token(TokenKind kind, Token *cur, char *str);
Token *tokenize();

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *expr();
Node *mul();
Node *primary();

void gen(Node *node);
void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
bool consume(char op);
void expect(char op);
int expect_number();
bool at_eof();

extern Token *token;
extern char *user_input;


#endif