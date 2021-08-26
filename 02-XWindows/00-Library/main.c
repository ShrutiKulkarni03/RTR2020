#include<stdio.h>
#include"ArithmeticOperations.h"

int main(void)
{
    
	int a, b;

	printf("\nEnter number 'a'\n");
	scanf("%d", &a);

	printf("\nEnter number 'b'\n");
	scanf("%d", &b);

	printf("\nAddition of 'a' and 'b' is %d\n", add(a, b));
	printf("Subtraction of 'a' from 'b' is %d\n", sub(b, a));
	printf("Subtraction of 'b' from 'a' is %d\n", sub(a, b));
	printf("Multiplication of 'a' and 'b' is %d\n", mul(a, b));
	printf("Division of 'a' by 'b' is %d\n", div(a, b));
	printf("Division of 'b' by 'a' is %d\n", div(b, a));
	printf("Modulus of 'a' by 'b' is %d\n", mod(a, b)); 
	printf("Modulus of 'b' by 'a' is %d\n\n", mod(b, a));

	return(0);
}

