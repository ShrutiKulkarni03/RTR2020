#include <stdio.h>
int main(void)
{
	
	char sk_option, sk_c = '\0';
	
	printf("\n\n");
	printf("Enter 'Q' or 'q' To Quit The Infinite For Loop : \n\n");

	do
	{
		do
		{
			printf("You are in Infinite Loop\n");
			sk_c = getch();
		}while (sk_c != 'Q' && sk_c != 'q');

		printf("\n\n");
		printf("EXITTING USER CONTROLLED INFINITE LOOP");
		printf("\n\n");
		printf("DO YOU WANT TO BEGIN USER CONTROLLED INFINITE LOOP AGAIN?\n(y/n) : ");
		sk_option = getch();
		printf("\n\n");
	}while (sk_option == 'Y' || sk_option == 'y');

	getch();
	return(0);
}
