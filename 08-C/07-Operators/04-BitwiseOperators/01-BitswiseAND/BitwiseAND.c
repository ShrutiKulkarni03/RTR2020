#include <stdio.h>
int main(void)
{
	int sk_d, sk_e, sk_result;

	printf("\n\n");
	printf("Enter number 'd'\n");
	scanf("%d", &sk_d);

	printf("\nEnter number 'e'\n");
	scanf("%d", &sk_e);

	sk_result = sk_d & sk_e;
	printf("\nCondition is (Result = d & e)\nThat is (Result = %d & %d)\nTherefore 'Result' is %d\n\n",sk_d, sk_e, sk_result);

	getch();
	return(0);

}
