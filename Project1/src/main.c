#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

int foo(int i){
    if(i == 1){
        return 1;
    }

    return i * foo(--i);
}

int main(){

    printf("Hello world\n");
    printf("Fib 5: %d\n",foo(5));
    return 0;
}