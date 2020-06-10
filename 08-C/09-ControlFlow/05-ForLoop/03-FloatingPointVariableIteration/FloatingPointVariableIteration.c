#include <stdio.h>
int main(void)
{
	float sk_f;
	float sk_f_num = 2.5f;
		
	printf("\n\n");
	printf("Numbers %f to %f : \n\n", sk_f_num, (sk_f_num * 10.0f));

	for (sk_f = sk_f_num; sk_f <= (sk_f_num * 10.0f); sk_f = sk_f + sk_f_num)
	{
		printf("\t%f\n", sk_f);
	}

	getch();
	return(0);

}
