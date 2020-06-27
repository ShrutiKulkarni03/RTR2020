#include <stdio.h>
int main(void)
{
	
	float sk_f;
	float sk_f_num = 1.5f; 
	
	printf("\n\n");
	printf("Printing Numbers %f to %f : \n\n", sk_f_num, (sk_f_num * 10.0f));
	sk_f = sk_f_num;
	do
	{
		printf("\t%f\n", sk_f);
		sk_f += sk_f_num;

	} while (sk_f <= (sk_f_num * 10.0f));

	printf("\n\n");

	getch();
	return(0);
}
