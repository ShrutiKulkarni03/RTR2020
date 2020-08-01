#include<stdio.h>

int main(void)
{
	//function prototype
	void MathematicalOperations(int, int, int*, int*, int*, int*, int*);

	//variable declaration
	int s, k;
	int* ans_sum = NULL;
	int* ans_difference = NULL;
	int* ans_product = NULL; 
	int* ans_quotient = NULL;
	int* ans_remainder = NULL;

	printf("\n\nEnter Value of 'S' : ");
	scanf("%d", &s);

	printf("\nEnter Value of 'K' : ");
	scanf("%d", &k);

	ans_sum = (int*)malloc(1 * sizeof(int));
	if (ans_sum == NULL)
	{
		printf("\n\nCould not Allocate Memory for 'ans_sum'. Exitting now.\n\n");
		exit(0);
	}

	ans_difference = (int*)malloc(1 * sizeof(int));
	if (ans_difference == NULL)
	{
		printf("\n\nCould not Allocate Memory for 'ans_difference'. Exitting now.\n\n");
		exit(0);
	}

	ans_product = (int*)malloc(1 * sizeof(int));
	if (ans_product == NULL)
	{
		printf("Could not Allocate Memory for 'ans_product'. Exitting Now.\n\n");
		exit(0);
	}

	ans_quotient = (int*)malloc(1 * sizeof(int));
	if (ans_quotient == NULL)
	{
		printf("Could not Allocate Memory for 'ans_quotient'. Exitting Now.\n\n");
		exit(0);
	}

	ans_remainder = (int*)malloc(1 * sizeof(int));
	if (ans_remainder == NULL)
	{
		printf("Could not Allocate Memory for 'ans_remainder'. Exitting Now.\n\n");
		exit(0);
	}

	MathematicalOperations(s, k, ans_sum, ans_difference, ans_product, ans_quotient, ans_remainder);

	printf("\n\nResult\n\n");
	printf("Sum = %d\n", *ans_sum);
	printf("Difference = %d\n", *ans_difference);
	printf("Product = %d\n", *ans_product);
	printf("Quotient = %d\n", *ans_quotient);
	printf("Remainder = %d\n\n", *ans_remainder);

	if (ans_remainder)
	{
		free(ans_remainder);
		ans_remainder = NULL;
		printf("Memory Allocated for 'ans_remainder' successfully freed!\n");
	}

	if (ans_quotient)
	{
		free(ans_quotient);
		ans_quotient = NULL;
		printf("Memory Allocated for 'ans_quotient' successfully freed!\n");
	}

	if (ans_product)
	{
		free(ans_product);
		ans_product = NULL;
		printf("Memory Allocated for 'ans_product' successfully freed!\n");
	}

	if (ans_difference)
	{
		free(ans_difference);
		ans_difference = NULL;
		printf("Memory Allocated for 'ans_difference' successfully freed!\n");
	}

	if (ans_sum)
	{
		free(ans_sum);
		ans_sum = NULL;
		printf("Memory Allocated for 'ans_sum' successfully freed!\n");
	}

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

