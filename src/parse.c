#include "mcc.h"

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

LVar *locals = NULL;

void initialize_lvar(){
    locals->next = NULL;
    locals->offset = 0;
}
LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next){
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)){
            return var;
        }
    }
    return NULL;
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

void program(){
    int i = 0;
    while(!at_eof())
        code[i++] = stmt();
    code[i] = NULL;
}

Node *stmt(){
    Node *node = expr();
    expect(";");
    return node;
}

Node *expr(){
    return assign();
}

Node *assign(){
    Node *node = equality();
    if(consume("="))
        node = new_node(ND_ASSIGN, node, assign());
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
    
    Token *tok = consume_ident();
    if(tok){
        Node *node = calloc(1,sizeof(Node));
        node->kind = ND_LVAR;

        LVar *lvar = find_lvar(tok);
        if(lvar != NULL){
            node->offset = lvar->offset;
        }else{
            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            if(locals==NULL)
                lvar->offset = 8;
            else
                lvar->offset = locals->offset + 8;
            node->offset = lvar->offset;
            locals = lvar;
        }
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

Token *consume_ident(){
    Token *tok = token;
    if(token->kind == TK_IDENT){
        token = token->next;
        return tok;
    }else{
        return NULL;
    }
}

void expect(char *op){
    if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
        error_at(token->str, "expected \"%s\"", op);
    token = token->next;
}

int expect_number(){
    if(token->kind != TK_NUM)
        error_at(token->str, "expected a number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(){
    return token->kind == TK_EOF;
}
