#include<stdio.h>

int main(void)
{
	int sk_i, sk_j, sk_result;

	printf("\n\n");
	printf("If 'Result' is 0, the relation is False\n");
	printf("If 'Result' is 1, the relation is True\n\n");

	printf("Enter number 'i'\n");
	scanf("%d", &sk_i);

	printf("\nEnter number 'j'\n");
	scanf("%d", &sk_j);

	sk_result = sk_i < sk_j;
	printf("\nCondition is (Result = i < j)\nThat is (Result = %d < %d)\nTherefore 'Result' is %d\n\n",sk_i, sk_j, sk_result);

	sk_result = sk_i > sk_j;
	printf("\nCondition is (Result = i > j)\nThat is (Result = %d > %d)\nTherefore 'Result' is %d\n\n", sk_i, sk_j, sk_result);

	sk_result = sk_i <= sk_j;
	printf("\nCondition is (Result = i <= j)\nThat is (Result = %d <= %d)\nTherefore 'Result' is %d\n\n", sk_i, sk_j, sk_result);

	sk_result = sk_i >= sk_j;
	printf("\nCondition is (Result = i >= j)\nThat is (Result = %d >= %d)\nTherefore 'Result' is %d\n\n", sk_i, sk_j, sk_result);

	sk_result = sk_i == sk_j;
	printf("\nCondition is (Result = i == j)\nThat is (Result = %d == %d)\nTherefore 'Result' is %d\n\n", sk_i, sk_j, sk_result);

	sk_result = sk_i != sk_j;
	printf("\nCondition is (Result = i != j)\nThat is (Result = %d != %d)\nTherefore 'Result' is %d\n\n", sk_i, sk_j, sk_result);

	getch();
	return(0);

}
