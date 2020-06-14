#include<stdio.h>

int main(int argc, char* argv[], char* envp[])
{
	void MyAddition(void);

	MyAddition();

	getch();
	return(0);

}

void MyAddition(void)
{
	int sk_a, sk_b, sk_sum;
	
	printf("\n\nEnter Integer value of 'A' : ");
	scanf("%d", &sk_a);

	printf("\n\nEnter Integer value of 'B' : ");
	scanf("%d", &sk_b);

	sk_sum = sk_a + sk_b;

	printf("\n\nAddition of %d and %d is %d\n\n", sk_a, sk_b, sk_sum);

}
