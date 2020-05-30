#include <stdio.h>
int main(void)
{
	int sk_f, sk_g, sk_result;

	printf("\n\n");
	printf("Enter number 'f'\n");
	scanf("%d", &sk_f);

	printf("\nEnter number 'g'\n");
	scanf("%d", &sk_g);

	sk_result = sk_f | sk_g;
	printf("\nCondition is (Result = f | g)\nThat is (Result = %d | %d)\nTherefore 'Result' is %d\n\n", sk_f, sk_g, sk_result);

	getch();
	return(0);

}
