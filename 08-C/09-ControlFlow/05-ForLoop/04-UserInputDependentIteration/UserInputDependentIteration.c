#include <stdio.h>
int main(void)
{
	int sk_p_num, sk_num, p;
	
	printf("\n\nEnter An Integer Value From Which Iteration Must Begin : ");
	scanf("%d", &sk_p_num);
	printf("How Many Digits Do You Want To Print From %d Onwards ? : ", sk_p_num);
	scanf("%d", &sk_num);
	printf("\nPrinting Digits %d to %d : \n\n", sk_p_num, (sk_p_num + sk_num));


	for (p = sk_p_num; p <= (sk_p_num + sk_num); p++)
	{
		printf("\t%d\n", p);
	}
	
	getch();
	return(0);

}
