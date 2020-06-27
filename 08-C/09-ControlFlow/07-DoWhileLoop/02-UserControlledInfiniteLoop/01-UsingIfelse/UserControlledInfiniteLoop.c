#include <stdio.h>
int main(void)
{
	
	char sk_option, sk_c = '\0';
	
	printf("\n\n");
	printf("Enter 'Q' or 'q' To Quit The Infinite For Loop : \n\n");
	printf("Enter 'Y' oy 'y' To Initiate User Controlled Infinite Loop : ");
	printf("\n\n");
	sk_option = getch();

	if (sk_option == 'Y' || sk_option == 'y')
	{
		do
		{
			printf("You are in Infinite Loop\n");
			sk_c = getch();
			if (sk_c == 'Q' || sk_c == 'q')
				break; 
		} while (1);
		printf("\n\n");
		printf("EXITTING USER CONTROLLED INFINITE LOOP...");
		printf("\n\n");
	}
	else

		printf("Enter 'Y' or 'y'\n\n");

	getch();
	return(0);
}
