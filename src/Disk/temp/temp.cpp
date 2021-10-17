#include<stdio.h>
#include<cstring>
int main()
{
    unsigned char addr[3];
    memset(addr, 0, sizeof(addr));
    printf("%d\n", sizeof addr);
    printf("%s\n", addr);
    printf("hello");
    return 0;
}