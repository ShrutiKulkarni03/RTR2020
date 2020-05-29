#include<stdio.h>

int main(void)
{
	int a, b;

	printf("\n\nEnter number 'a'\n");
	scanf("%d", &a);

	printf("\nEnter number 'b'\n");
	scanf("%d", &b);

	printf("\nAddition of 'a' and 'b' is %d\n", a + b);
	printf("Subtraction of 'a' from 'b' is %d\n", b - a);
	printf("Subtraction of 'b' from 'a' is %d\n", a - b);
	printf("Multiplication of 'a' and 'b' is %d\n", a * b);
	printf("Division of 'a' by 'b' is %d\n", a / b);
	printf("Division of 'b' by 'a' is %d\n", b / a);
	printf("Modulus of 'a' by 'b' is %d\n", a % b); 
	printf("Modulus of 'b' by 'a' is %d\n", b % a);

	getch();
	return(0);
}
