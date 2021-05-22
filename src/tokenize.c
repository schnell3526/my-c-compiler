#include "mcc.h"

/*
functions defined in this file are for tokenize.
mcc tokenizes input strings into Token struct.
String -> Token
*/


bool is_alpha(char c){
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

bool is_alnum(char c){
    return is_alpha(c) || ('0' <= c && c <= '9');
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    tok->len = len;
    return tok;
}

Token *tokenize(){
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p){
        
        // スペース無視
        if(isspace(*p)){
            p++;
            continue;
        }

        // 2文字トークン, 1文字よりも先に読む
        if(memcmp(p, "<=", 2) == 0|| memcmp(p, ">=", 2) == 0|| memcmp(p, "==", 2) == 0|| memcmp(p, "!=", 2) == 0){
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        // 一文字のトークン
        if(strchr("+-*/()<>;=", *p)){
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        // returnの場合
        if(strncmp(p, "return", 6) == 0 && !is_alnum(p[6])){
            cur = new_token(TK_RETURN, cur, p, 6);
            p += 6;
            continue;
        }

        // 変数の場合
        if(is_alpha(*p)){
            char *q = p;
            while(is_alnum(*p)){
                p++;
            }
            cur = new_token(TK_IDENT, cur, q, p-q);
            continue;
        }
        // 数字の場合
        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }


        // error("invalid token");
        error_at(p, "aexpected a number");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

bool at_eof(){
    return token->kind == TK_EOF;
}
