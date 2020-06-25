#include<stdio.h>

int main(void)
{
	int a, b, x;

	printf("\n\n");
	printf("Enter number 'a'\n");
	scanf("%d", &a);

	printf("\n\nEnter number 'b'\n");
	scanf("%d", &b);

	x = a;
	a += b;
	printf("\nAddition of %d + %d = %d\n", x, b, a);

	x = a;
	a -= b;
	printf("Subtraction of %d - %d = %d\n", x, b, a);

	x = a;
	a *= b;
	printf("Multiplication of %d * %d = %d\n", x, b, a);

	x = a;
	a /= b;
	printf("Division of %d / %d = %d\n", x, b, a);
	
	x = a;
	a %= b;
	printf("Modulus of %d / %d = %d\n", x, b, a);

	getch();
	return(0);

}
