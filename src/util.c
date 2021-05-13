#include "util.h"
#include <stdio.h>  // fprintf, vfprintf, stderr
#include <stdlib.h> // calloc, exit, strtol
#include <stdarg.h> // va_start
#include <ctype.h>  // isspace, isdigit


Token *new_token(TokenKind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize(){
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }

        if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')'){
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        // 数字の場合
        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        // error("invalid token");
        error_at(p, "aexpected a number");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *expr(){
    Node *node = mul();

    for(;;){
        if(consume('+'))
            node = new_node(ND_ADD, node, mul());
        else if(consume('-'))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *mul(){
    Node *node = unary();

    for(;;){
        if(consume('*'))
            node = new_node(ND_MUL, node, unary());
        else if(consume('/'))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *unary(){
    if(consume('+'))
        return primary();
    if(consume('-'))
        return new_node(ND_SUB, new_node_num(0), primary());
    return primary();
}

Node *primary(){
    if(consume('(')){
        Node *node = expr();
        expect(')');
        return node;
    }
    return new_node_num(expect_number());
}

void gen(Node *node){
    if(node->kind == ND_NUM){
        printf("\tpush %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("\tpop rdi\n");
    printf("\tpop rax\n");

    switch(node->kind){
        case ND_ADD:
            printf("\tadd rax, rdi\n");
            break;
        case ND_SUB:
            printf("\tsub rax, rdi\n");
            break;
        case ND_MUL:
            printf("\timul rax, rdi\n");
            break;
        case ND_DIV:
            printf("\tcqo\n");
            printf("\tidiv rdi\n");
            break;
    }

    printf("\tpush rax\n");
}

void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap,fmt);

    int pos = loc -user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt , ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(char op){
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

void expect(char op){
    if (token->kind != TK_RESERVED || token->str[0] != op)
        // error("expected '%c'", op);
        error_at(token->str, "expected '%c", op);
    token = token->next;
}

int expect_number(){
    if(token->kind != TK_NUM)
        // error("expected number");
        error_at(token->str, "expected a number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(){
    return token->kind == TK_EOF;
}
