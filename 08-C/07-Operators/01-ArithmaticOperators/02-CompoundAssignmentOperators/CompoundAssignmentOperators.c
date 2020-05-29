#include <stdio.h>

int main(void)
{
	//variable declarations
	int a;
	int b;
	int x;

	//code
	printf("\n\n");
	printf("Enter A Number : ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter Another Number : ");
	scanf("%d", &b);

	printf("\n\n");

	x = a;
	a += b; // a = a + b
	printf("Addition Of A = %d And B = %d Gives %d.\n", x, b, a);

	x = a;
	a -= b; // a = a - b
	printf("Subtraction Of A = %d And B = %d Gives %d.\n", x, b, a);

	x = a;
	a *= b; // a = a * b
	printf("Multiplication Of A = %d And B = %d Gives %d.\n", x, b, a);

	x = a;
	a /= b; // a = a / b 
	printf("Division Of A = %d And B = %d Gives Quotient %d.\n", x, b, a);

	x = a;
	a %= b; // a = a % b
	printf("Division Of A = %d And B = %d Gives Remainder %d.\n", x, b, a);

	printf("\n\n");

	getch();
	return(0);
}
