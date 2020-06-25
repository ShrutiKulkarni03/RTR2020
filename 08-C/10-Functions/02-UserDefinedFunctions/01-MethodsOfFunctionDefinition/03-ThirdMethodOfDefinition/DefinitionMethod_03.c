#include<stdio.h>

int main(int argc, char* argv[], char* envp[])
{
	void MyAddition(int, int); 

	int sk_a, sk_b;

	printf("\n\nEnter Integer 'A' : ");
	scanf("%d", &sk_a);

	printf("\nEnter Integer 'B' : ");
	scanf("%d", &sk_b);

	MyAddition(sk_a, sk_b);

	return(0);

}

void MyAddition(int sk_a, int sk_b)
{
	int sum;

	sum = sk_a + sk_b;

	printf("\n\n");
	printf("Sum Of %d and %d is %d\n\n", sk_a, sk_b, sum);


}
