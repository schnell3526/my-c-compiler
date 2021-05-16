#ifndef UTIL_H
#define UTIL_H
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>  // fprintf(), vfprintf(), stderr
#include <stdlib.h> // calloc(), exit(), strtol()
#include <stdarg.h> // va_start()
#include <ctype.h>  // isspace(), isdigit()
#include <string.h> // memcmp()

typedef enum{
    ND_ADD,         // '+'
    ND_SUB,         // '-'
    ND_MUL,         // '*'
    ND_DIV,         // '/'
    ND_NUM,         // '1-9'
    ND_EQUAL,       // '=='
    ND_NOT_EQUAL,   // '!='
    ND_LESS,        // '<'
    ND_LESS_EQUAL,  // '<='
    ND_ASSIGN,      // '='
    ND_LVAR         // ローカル変数
} NodeKind;

typedef enum {
    TK_RESERVED,    // 記号
    TK_IDENT,       // 識別子
    TK_NUM,         // 整数トークン
    TK_EOF,         // 入力終了トークン
} TokenKind;

typedef struct Node Node;

struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
    int offset;
};

typedef struct Token Token;

struct Token{
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合、その数値
    char *str;      // トークンの文字列
    int len;        // トークンの長さ
};

Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize();

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

// 作成したToken列を元にASTを構築する関数
void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// 作成したASTをもとにアセンブリを出力する関数
void gen_lval(Node *node);
void gen(Node *node);

// エラー出力関数
void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);

// トークン判定関数
bool consume(char *op);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();

// グローバル変数を出力
extern Token *token;
extern char *user_input;
extern Node *code[100];


#endif