#include<stdio.h>

int main(void)
{
	int sk_a, sk_b, sk_c, sk_result;

	printf("\n\n");
	printf("If 'Result' is 0, the relation is False\n");
	printf("If 'Result' is 1, the relation is True\n\n");

	printf("Enter number 'a'\n");
	scanf("%d", &sk_a);

	printf("\nEnter number 'b'\n");
	scanf("%d", &sk_b);

	printf("\nEnter number 'c'\n");
	scanf("%d", &sk_c);

	sk_result = (sk_a >= sk_b) && (sk_a != sk_c);
	printf("\nCondition is [Result = (a >= b) && (a != c)]\nThat is [Result = (%d >= %d) && (%d != %d)]\nTherefore 'Result' is %d\n",sk_a, sk_b, sk_a, sk_c, sk_result);

	sk_result = (sk_a < sk_b) && (sk_b == sk_c);
	printf("\nCondition is [Result = (a < b) && (b == c)]\nThat is [Result = (%d < %d) && (%d == %d)]\nTherefore 'Result' is %d\n", sk_a, sk_b, sk_b, sk_c, sk_result);

	sk_result = (sk_a > sk_c) || (sk_c <= sk_b);
	printf("\nCondition is [Result = (a > c) || (c <= b)]\nThat is [Result = (%d > %d) || (%d <= %d)]\nTherefore 'Result' is %d\n", sk_a, sk_c, sk_c, sk_b, sk_result);

	sk_result = (sk_c != sk_b) || (sk_b > sk_a);
	printf("\nCondition is [Result = (c != b) || (b > a)]\nThat is [Result = (%d != %d) || (%d > %d)]\nTherefore 'Result' is %d\n", sk_c, sk_b, sk_b, sk_a, sk_result);

	sk_result = !sk_a;
	printf("\nCondition is (Result = !a)\nThat is (Result = !%d)\nTherefore 'Result' is %d\n",sk_a, sk_result);

	sk_result = !sk_b;
	printf("\nCondition is (Result = !b)\nThat is (Result = !%d)\nTherefore 'Result' is %d\n", sk_b, sk_result);

	sk_result = !sk_c;
	printf("\nCondition is (Result = !c)\nThat is (Result = !%d)\nTherefore 'Result' is %d\n", sk_c, sk_result);

	getch();
	return(0);
	
}
