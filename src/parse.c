#include "mcc.h"

/*
functions defined in this file are for parsing.
mcc parses input Token list into AST.
Token list -> AST
*/

// ローカル変数リスト
LVar *locals = NULL;

// ローカル変数を初期化
void initialize_lvar(){
    locals->next = NULL;
    locals->offset = 0;
}

// ローカル変数が使われたかどうか検索
LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next){
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)){
            return var;
        }
    }
    return NULL;
}

// 新しいノードを作成(演算子用)
Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

// 新しいノードを作成(数値用)
Node *new_node_num(int val){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}


/*
GRAMMER

program     = stmt*
// stmt        = expr ";"
stmt        = expr ";" | "return" expr ";"
expr        = assign
assign      = equality ("=" assign)?
equality    = relational ( "==" relational | "!=" relational)*
relational  = add ("<" add | "<=" add | ">" add | ">=" add)*
add         = mul ("+" mul | "-" mul)*
mul         = unary ("*" unary | "/" unary)*
unary       = ("+" | "-")? primary
primary     = num | idnet | "(" expr ")" 
*/


void program(){
    int i = 0;
    while(!at_eof())
        code[i++] = stmt();
    code[i] = NULL;
}

Node *stmt(){
    Node *node;

    if(consume_return()){
        // token = token->next;
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
    }else{
        node = expr();
    }

    if(!consume(";")){
        error_at(token->str, "\";\"ではないトークンです.");
    }
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


// 以下判定関数, consumeは統合したい
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

Token *consume_return(){
    Token *tok = token;
    if(token->kind == TK_RETURN){
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
