#include <stdio.h>
int main(void)
{
	float sk_f;
	float sk_f_num = 3.3f;
		
	printf("\n\n");
	printf("Numbers %f to %f : \n\n", sk_f_num, (sk_f_num * 10.0f));

	sk_f = sk_f_num;
	while (sk_f <= (sk_f_num * 10.0f))
	{
		printf("\t%f\n", sk_f);
		sk_f = sk_f + sk_f_num;
	}

	getch();
	return(0);

}
