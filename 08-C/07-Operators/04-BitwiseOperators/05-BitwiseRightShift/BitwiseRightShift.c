#include <stdio.h>
int main(void)
{
	unsigned int sk_m, n_shift, sk_result;

	printf("\n\n");
	printf("Enter number 'm'\n");
	scanf("%u", &sk_m);

	printf("Enter number to shift 'n_shift'\n");
	scanf("%u", &n_shift);

	sk_result = sk_m >> n_shift;
	printf("\nCondition is (Result = m >> n_shift)\nThat is (Result = %u >> %u)\nTherefore 'Result' is %d\n\n", sk_m, n_shift, sk_result);

	getch();
	return(0);

}
