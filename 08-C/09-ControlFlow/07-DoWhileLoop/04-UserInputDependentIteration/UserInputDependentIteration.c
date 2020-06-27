#include <stdio.h>
int main(void)
{

	int sk_i_num, sk_num, sk_i;
	
	printf("\n\n");
	printf("Enter An Integer Value From Which Iteration Must Begin : ");
	scanf("%d", &sk_i_num);
	printf("How Many Digits Do You Want To Print From %d Onwards? : ", sk_i_num);
	scanf("%d", &sk_num);
	printf("Printing Digits %d to %d : \n\n", sk_i_num, (sk_i_num + sk_num));
	sk_i = sk_i_num;
	do
	{
		printf("\t%d\n", sk_i);
		sk_i++;
	} while (sk_i <= (sk_i_num + sk_num));
	printf("\n\n");

	getch();
	return(0);
}
