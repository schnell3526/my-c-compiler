#include "mcc.h"

Token *token;

char *user_input;

int main(int argc, char **argv){
    if(argc != 2){
        error("%s: invalid namber of arguments, argv[0]");
        return 1;
    }

    user_input = argv[1];
    token = tokenize();
    Node *node = expr();

    // アセンブリ前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // ASTを下りながらコード生成
    gen(node);

    // スタックトップに式全体の値が残っているはずなので
    // それをRAXにロードして関数からの返り値とする
    printf("\tpop rax\n");
    printf("\tret\n");
    return 0;
}