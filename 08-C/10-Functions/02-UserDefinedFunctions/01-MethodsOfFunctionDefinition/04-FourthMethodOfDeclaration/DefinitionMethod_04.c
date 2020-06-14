#include<stdio.h>

int main(int argc, char* argv[], char* envp[])
{
	int MyAddition(int, int); 

	int sk_a, sk_b, result;

	printf("\n\nEnter Integer 'A' : ");
	scanf("%d", &sk_a);

	printf("\nEnter Integer 'B' : ");
	scanf("%d", &sk_b);

	result = MyAddition(sk_a, sk_b);
	printf("\n\nSum Of %d and %d is %d\n\n", sk_a, sk_b, result);

	return(0);

}

int MyAddition(int sk_a, int sk_b)
{
	int result;

	result = sk_a + sk_b;

	return(result);

}
