#include <stdio.h>
int main(void)
{
	char sk_opt, sk_c = '\0';
	
	printf("\n\nEnter 'Q' or 'q' To Quit The Infinite For Loop : \n\n");
	printf("Enter 'Y' oy 'y' To Initiate User Controlled Infinite Loop : \n\n");
	sk_opt = getch();

	if (sk_opt == 'Y' || sk_opt == 'y')
	{
		while (1)
		{
			printf("You are in Infinite Loop\n");
			sk_c = getch();
			if (sk_c == 'Q' || sk_c == 'q')
				break;
		}
		printf("\n\n");
		printf("EXITTING USER CONTROLLED INFINITE LOOP");
		printf("\n\n");
	}
	else
		printf("Press 'Y' or 'y' To Initiate The User Controlled Infinite Loop\n\n");

	getch();
	return(0);
}
