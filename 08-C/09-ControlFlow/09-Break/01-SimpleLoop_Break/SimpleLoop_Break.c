#include<stdio.h>
#include<conio.h>

int main(void)
{
	int sk_i;
	char sk_c;

	printf("\n\nPrinting Even Numbers From 1 to 100 For Every User Input.\n\nExitting the Loop When User Enters Character 'Q' or 'q'\n\n");
	printf("Enter Character 'Q' or 'q' To Exit Loop\n\n");

	for (sk_i = 1; sk_i <= 100; sk_i++)
	{
		printf("\t%d\n", sk_i);
		sk_c = getch();
		if (sk_c == 'Q' || sk_c == 'q')
		{
			break;
		}

	}

	printf("\n\nEXITTING LOOP\n\n");

	getch();
	return(0);

}
