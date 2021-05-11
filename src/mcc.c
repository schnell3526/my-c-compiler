#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// kind of token
typedef enum {
    TK_RESERVED,    // sign
    TK_NUM,         // integer token
    TK_EOF,         // A token representing the end of the input string.
} TokenKind;

typedef struct Token Token;

struct Token{
    TokenKind kind; // type of token
    Token *next;    // next token
    int val;        // number
    char *str;      // token string
};

Token *token;

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
        error("'%c' is expected", op);
    token = token->next;
}

int expect_number(){
    if(token->kind != TK_NUM)
        error("it's not number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(){
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *p){
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }

        if(*p == '+' || *p == '-'){
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("トークナイズできません");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "The number of argument is incorrect.\n");
        return 1;
    }

    token = tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("\tmov rax, %d\n", expect_number());

    while(!at_eof()){
        if(consume('+')){
            printf("\tadd rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("\tsub rax, %d\n", expect_number());
    }

    printf("\tret\n");
    return 0;
}