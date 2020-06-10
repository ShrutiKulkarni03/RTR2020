#include <stdio.h>
int main(void)
{
	int sk_p_num, sk_num, p;
	
	printf("\n\nEnter An Integer Value From Which Iteration Must Begin : ");
	scanf("%d", &sk_p_num);
	printf("How Many Digits Do You Want To Print From %d Onwards ? : ", sk_p_num);
	scanf("%d", &sk_num);
	printf("\nPrinting Digits %d to %d : \n\n", sk_p_num, (sk_p_num + sk_num));


	p = sk_p_num;
	while (p <= (sk_p_num + sk_num))
	{
		printf("\t%d\n", p);
		p++;
	}
	
	getch();
	return(0);

}
