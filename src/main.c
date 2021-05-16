#include "mcc.h"

Token *token;

char *user_input;

Node *code[100];

int main(int argc, char **argv){
    if(argc != 2){
        error("%s: invalid namber of arguments, argv[0]");
        return 1;
    }

    // トークナイズしてパーズする
    // 結果をcodeに保存
    user_input = argv[1];
    token = tokenize();
    program();

    // アセンブリ前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // プロローグ
    // 変数26個分の領域を確保する
    printf("\tpush rbp\n");
    printf("\tmov rbp, rsp\n");
    printf("\tsub rsp, 208\n");

    // ASTを下りながらコード生成
    for(int i = 0; code[i]; i++){
        gen(code[i]);

        // 式の評価結果としてスタックに値が残っているので溢れないようにポップする
        printf("\tpop rax\n");
    }

    // エピローグ
    // 最後の式の結果がRAXに残っているのでそれが返り値になる
    printf("\tmov rsp, rbp\n");
    printf("\tpop rbp\n");
    printf("\tret\n");
    return 0;
}