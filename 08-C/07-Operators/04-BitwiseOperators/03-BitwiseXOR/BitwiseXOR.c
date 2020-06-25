#include <stdio.h>
int main(void)
{
	int sk_h, sk_k, sk_result;

	printf("\n\n");
	printf("Enter number 'h'\n");
	scanf("%d", &sk_h);

	printf("\nEnter number 'k'\n");
	scanf("%d", &sk_k);

	sk_result = sk_h ^ sk_k;
	printf("\nCondition is (Result = h ^ k)\nThat is (Result = %d ^ %d)\nTherefore 'Result' is %d\n\n", sk_h, sk_k, sk_result);

	getch();
	return(0);

}
