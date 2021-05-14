#include "mcc.h"

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
        if(isspace(*p)){
            p++;
            continue;
        }

        if(memcmp(p, "<=", 2) == 0|| memcmp(p, ">=", 2) == 0|| memcmp(p, "==", 2) == 0|| memcmp(p, "!=", 2) == 0){
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        // 一文字のトークン
        if(strchr("+-*/()<>", *p)){
            cur = new_token(TK_RESERVED, cur, p++, 1);
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
    Node *node = equality();
    return node;
}

Node *equality(){
    Node *node = relational();
    for(;;){
        if(consume("=="))
            node = new_node(ND_EQUAL, node, relational());
        else if(consume("!="))
            node = new_node(ND_NOT_EQUAL, node, relational());
        else
            return node;
    }
}

Node *relational(){
    Node *node = add();

    for(;;){
        if(consume("<="))
            node = new_node(ND_LESS_EQUAL, node, add());
        else if(consume(">="))
            node = new_node(ND_LESS_EQUAL, add(), node);
        else if(consume("<"))
            node = new_node(ND_LESS, node, add());
        else if(consume(">"))
            node = new_node(ND_LESS, add(), node);
        else
            return node;
    }
}

Node *add(){
    Node *node = mul();

    for(;;){
        if(consume("+")){
            node = new_node(ND_ADD, node, mul());
        }else if(consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *mul(){
    Node *node = unary();

    for(;;){
        if(consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if(consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *unary(){
    if(consume("+"))
        return primary();
    if(consume("-"))
        return new_node(ND_SUB, new_node_num(0), primary());
    return primary();
}

Node *primary(){
    if(consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }
    return new_node_num(expect_number());
}

bool consume(char *op){
    if (token->kind != TK_RESERVED || 
        strlen(op) != token->len || 
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

void expect(char *op){
    if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
        // error("expected '%c'", op);
        error_at(token->str, "expected \"%s\"", op);
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
