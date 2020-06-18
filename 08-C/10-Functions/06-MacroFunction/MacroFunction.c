#include<stdio.h>
#define MAX_NUM(a, b)((a>b) ? a:b)

int main(int argc, char* argv[], char* envp[])
{
	int s_num1, s_num2, s_result;
	float k_num1, k_num2, k_result;

	printf("\n\nEnter an Integer\n");
	scanf("%d", &s_num1);

	printf("\n\nEnter another Integer\n");
	scanf("%d", &s_num2);

	s_result = MAX_NUM(s_num1, s_num2);
	printf("\nMax Number is %d\n", s_result);


	printf("\n\nEnter a Floating Point Value\n");
	scanf("%f", &k_num1);

	printf("\n\nEnter another Floating Point Value\n");
	scanf("%f", &k_num2);

	k_result = MAX_NUM(k_num1, k_num2);
	printf("\nMax Number is %f\n", k_result);

	getch();
	return(0);
}
