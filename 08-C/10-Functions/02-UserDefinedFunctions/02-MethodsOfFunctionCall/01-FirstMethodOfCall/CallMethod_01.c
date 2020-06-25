#include<stdio.h>

int main(int argc, char* argv[], char* envp[])
{
	void MyAddition(void);
	int MySubtraction(void);
	void MyMultiplication(int, int);
	int MyDivision(int, int);

	int result_subtraction;
	int a_multiplication, b_multiplication;
	int a_division, b_division;
	int result_division;

	MyAddition();
	
	result_subtraction = MySubtraction();
	printf("\nSubtraction Result is %d\n", result_subtraction);

	printf("\n\nEnter Integer 'A' for Multiplication : ");
	scanf("%d", &a_multiplication);

	printf("\nEnter Integer 'B' for Multiplication : ");
	scanf("%d", &b_multiplication);

	MyMultiplication(a_multiplication, b_multiplication);

	printf("\n\nEnter Integer 'C' for Division : ");
	scanf("%d", &a_division);

	printf("\nEnter Integer 'D' for Division : ");
	scanf("%d", &b_division);

	result_division = MyDivision(a_division, b_division);

	printf("\nDivision of %d and %d is %d (Quotient)\n", a_division, b_division, result_division);

	return(0);

}

void MyAddition(void)
{
	int sk_a, sk_b, sk_sum;

	printf("\n\nEnter Integer 'E' for Addition : ");
	scanf("%d", &sk_a);

	printf("\nEnter Integer 'F' for Addition : ");
	scanf("%d", &sk_b);

	sk_sum = sk_a + sk_b;

	printf("\nSum of %d and %d is %d\n", sk_a, sk_b, sk_sum);

}

int MySubtraction(void)
{
	int sk_i, sk_j, sk_subtraction;

	printf("\n\nEnter Integer 'G' for Subtraction : ");
	scanf("%d", &sk_i);

	printf("\nEnter Integer 'H' for Subtraction : ");
	scanf("%d", &sk_j);

	sk_subtraction = sk_i - sk_j;
	return(sk_subtraction);
}

void MyMultiplication(int sk_a, int sk_b)
{
	int sk_multiplication;

	sk_multiplication = sk_a * sk_b;

	printf("\nMultiplication of %d and %d is %d\n", sk_a, sk_b, sk_multiplication);
}

int MyDivision(int sk_a, int sk_b)
{
	int division_quotient;

	if (sk_a > sk_b)
	{
		division_quotient = sk_a / sk_b;

	}
	else
	{
		division_quotient = sk_b / sk_a;

	}
	return(division_quotient);

}
