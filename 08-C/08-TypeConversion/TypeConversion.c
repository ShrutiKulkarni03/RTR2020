#include<stdio.h>

int main(void)
{
	int sk_i, sk_j;
	char sk_ch1, sk_ch2;

	int sk_a, sk_result_int;
	float sk_f, sk_result_float;

	int i_explicit;
	float f_explicit;

	printf("\n\n");
	printf("Enter an Integer\n");

	scanf("%d", &sk_i);
	sk_ch1 = sk_i;

	printf("\nInteger = %d\n", sk_i);
	printf("Character is '%c'\n\n", sk_ch1);

	printf("Enter a Character\n");

	sk_ch2 = getch();
	sk_j = sk_ch2;

	printf("\nCharacter is %c\n", sk_ch2);
	printf("Integer is %d\n\n", sk_j);

	printf("Enter an Integer\n");
	scanf("%d", &sk_a);

	printf("Enter a Floating point Value\n");
	scanf("%f", &sk_f);

	sk_result_float = sk_a + sk_f;

	printf("Integer %d + Float %ff gives %ff\n", sk_a, sk_f, sk_result_float);
	printf("Float %ff + Integer %d gives %d\n\n", sk_f, sk_a, sk_result_int);

	printf("Enter a Floating point Value\n");
	scanf("%f", &f_explicit);

	i_explicit = (int)f_explicit;

	printf("f_explicit is %ff\n", f_explicit);
	printf("Integer after type casting is %d\n", i_explicit);

	return(0);
}