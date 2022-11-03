#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int addr5;
int addr6;

int foo()
{
    return -1;
}
void point_at(void *p);
void foo1();
char g = 'g';
void foo2();

int main(int argc, char **argv)
{
    int addr2;
    int addr3;
    char *yos = "ree";
    int *addr4 = (int *)(malloc(50));
    printf("- &addr2: %p\n", &addr2);
    printf("- &addr3: %p\n", &addr3);
    printf("- foo: %p\n", &foo);
    printf("- &addr5: %p\n", &addr5);

    point_at(&addr5);

    printf("- &addr6: %p\n", &addr6);
    printf("- yos: %p\n", yos);
    printf("- gg: %p\n", &g);
    printf("- addr4: %p\n", addr4);
    printf("- &addr4: %p\n", &addr4);

    printf("- &foo1: %p\n", &foo1);
    printf("- &foo1: %p\n", &foo2);
    printf("- &foo2 - &foo1: %d\n", &foo2 - &foo1);

    int iarray[] = {1, 2, 3};
    char carray[] = {'a', 'b', 'c'};
    float farray[] = {1.1, 2.2, 3.3};
    double darray[] = {4, 5, 6};
    // int *iarrayPtr;
    // char *carrayPtr;
    // float *farrayPtr;
    // double *darrayPtr;
    // iarrayPtr = iarray;
    // carrayPtr = carray;
    // farrayPtr = farray;
    // darrayPtr = darray;

    printf("%p", iarray);
    printf("    %p", iarray + 1);
    printf("\n");

    printf("%p", carray);
    printf("    %p", carray + 1);
    printf("\n");

    printf("%p", darray);
    printf("    %p", darray + 1);
    printf("\n");

    printf("%p", farray);
    printf("    %p", farray + 1);
    printf("\n");

    return 0;
}

void point_at(void *p)
{
    int local;
    static int addr0 = 2;
    static int addr1;

    long dist1 = (size_t)&addr6 - (size_t)p;
    long dist2 = (size_t)&local - (size_t)p;
    long dist3 = (size_t)&foo - (size_t)p;

    printf("dist1: (size_t)&addr6 - (size_t)p: %ld\n", dist1);
    printf("dist2: (size_t)&local - (size_t)p: %ld\n", dist2);
    printf("dist3: (size_t)&foo - (size_t)p:  %ld\n", dist3);

    printf("- addr0: %p\n", &addr0);
    printf("- addr1: %p\n", &addr1);
}

void foo1()
{
    printf("foo1\n");
}

void foo2()
{
    printf("foo2\n");
}
