#include <stdio.h>

typedef struct {
    int i1;
    int i2;
    int i3;
    char c1;
    char c2;
    char c3;
    int ia1[5];
    int ia2[5];
    int ia3[5];
} data;

data mydata;

int main(int argc, char **argv){
    printf("mydata is at %p\n", &(mydata));
    printf("i1 is at %p\n", &(mydata.i1));
    printf("i2 is at %p\n", &(mydata.i2));
    printf("i3 is at %p\n", &(mydata.i3));
    printf("c1 is at %p\n", &(mydata.c1));
    printf("c2 is at %p\n", &(mydata.c2));
    printf("c3 is at %p\n", &(mydata.c3));
    printf("ia1 is at %p\n", &(mydata.ia1));
    printf("ia2 is at %p\n", &(mydata.ia2));
    printf("ia3 is at %p\n", &(mydata.ia3));
    printf("the size of my data is %ld\n", sizeof(mydata[1]));
    return 0;
}