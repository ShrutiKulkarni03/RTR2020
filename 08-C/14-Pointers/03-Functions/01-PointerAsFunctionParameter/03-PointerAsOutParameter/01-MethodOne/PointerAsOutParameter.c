#include<stdio.h>

int main(void)
{
	//function prototype
	void MathematicalOperations(int, int, int*, int*, int*, int*, int*);

	//variable declaration
	int s, k, ans_sum, ans_difference, ans_product, ans_quotient, ans_remainder;

	printf("\n\nEnter Value of 'S' : ");
	scanf("%d", &s);

	printf("\nEnter Value of 'K' : ");
	scanf("%d", &k);

	MathematicalOperations(s, k, &ans_sum, &ans_difference, &ans_product, &ans_quotient, &ans_remainder);

	printf("\n\nResult\n\n");
	printf("Sum = %d\n", ans_sum);
	printf("Difference = %d\n", ans_difference);
	printf("Product = %d\n", ans_product);
	printf("Quotient = %d\n", ans_quotient);
	printf("Remainder = %d\n", ans_remainder);

	return(0);
}

void MathematicalOperations(int a, int b, int* sum, int* difference, int* product, int* quotient, int* remainder)
{
	*sum = a + b;
	*difference = a - b;
	*product = a * b;
	*quotient = a / b;
	*remainder = a % b;
}

