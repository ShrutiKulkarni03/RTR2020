#include<stdio.h>

int main(int argc, char* argv[], char* envp[])
{
	int MyAddition(void);

	int result;

	result = MyAddition();

	printf("\n\n");
	printf("Sum = %d\n", result);
	return(0);

}

int MyAddition(void)
{
	int sk_a, sk_b, sum;

	printf("\n\nEnter Integer 'A' : ");
	scanf("%d", &sk_a);

	printf("\nEnter Integer 'B' : ");
	scanf("%d", &sk_b);

	sum = sk_a + sk_b;

	return(sum);

}
