#include<stdio.h>

int main(void)
{
	int sk_a, sk_b;
	int sk_p, sk_q;
	char sk_c01, sk_c02;
	int sk_i01, sk_i02;

	printf("\n\n");
	printf("Enter Integer 'A'\n");
	scanf("%d", &sk_a);
	printf("Enter Integer 'B'\n");
	scanf("%d", &sk_b);

	sk_c01 = (sk_a >= sk_b) ? 'A' : 'B';
	sk_i01 = (sk_a >= sk_b) ? sk_a : sk_b;
	printf("Ternary Operator Answer 1 ----- %c and %d.\n\n", sk_c01, sk_i01);

	printf("Enter Integer 'P'\n");
	scanf("%d", &sk_p);
	printf("Enter Integer 'Q'\n");
	scanf("%d", &sk_q);

	sk_c02 = (sk_p != sk_q) ? 'P' : 'Q';
	sk_i02 = (sk_p != sk_q) ? sk_p : sk_q;
	printf("Ternary Operator Answer 1 ----- %c and %d.\n\n", sk_c02, sk_i02);

	getch();
	return(0);
}
