#include <stdio.h>
int main(void)
{
	int sk_l, sk_result;

	printf("\n\n");
	printf("Enter number 'l'\n");
	scanf("%d", &sk_l);

	sk_result = ~sk_l; 
	printf("\nCondition is (Result = ~l)\nThat is (Result = ~%d)\nTherefore 'Result' is %d\n\n", sk_l, sk_result);

	getch();
	return(0);

}
