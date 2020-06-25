#include <stdio.h>
int main(void)
{
	unsigned int sk_o, n_shift, sk_result;

	printf("\n\n");
	printf("Enter number 'o'\n");
	scanf("%u", &sk_o);

	printf("Enter number to shift 'n_shift'\n");
	scanf("%u", &n_shift);

	sk_result = sk_o << n_shift;
	printf("\nCondition is (Result = o << n_shift)\nThat is (Result = %u << %u)\nTherefore 'Result' is %d\n\n", sk_o, n_shift, sk_result);

	getch();
	return(0);

}
