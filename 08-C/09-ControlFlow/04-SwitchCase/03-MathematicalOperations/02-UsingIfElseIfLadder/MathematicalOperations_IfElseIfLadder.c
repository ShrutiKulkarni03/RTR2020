#include <stdio.h> 
#include <conio.h> 
int main(void)
{
	
	int sk_a, sk_b;
	int sk_result;
	char sk_opt, sk_opt_division;

	printf("\n\n");
	printf("Enter Value For 'A'\n");
	scanf("%d", &sk_a);
	printf("Enter Value For 'B'\n");
	scanf("%d", &sk_b);
	printf("\nEnter Option In Character : \n\n");
	printf("'A' or 'a' For Addition\n");
	printf("'S' or 's' For Subtraction\n");
	printf("'M' or 'm' For Multiplication\n");
	printf("'D' or 'd' For Division\n\n");
	printf("Enter Option : ");
	sk_opt = getch();
	printf("\n\n");
	if (sk_opt == 'A' || sk_opt == 'a')
	{
		sk_result = sk_a + sk_b;
		printf("Addition Of A = %d And B = %d Gives Result %d !!!\n\n", sk_a, sk_b, sk_result);
	}
	else if (sk_opt == 'S' || sk_opt == 's')
	{
		if (sk_a >= sk_b)
		{
			sk_result = sk_a - sk_b;
			printf("Subtraction Of B = %d From A = %d Gives Result %d !!!\n\n", sk_b, sk_a, sk_result);
		}
		else
		{
			sk_result = sk_b - sk_a;
			printf("Subtraction Of A = %d From B = %d Gives Result %d !!!\n\n", sk_a, sk_b, sk_result);
		}
	}
	else if (sk_opt == 'M' || sk_opt == 'm')
	{
		sk_result = sk_a * sk_b;
		printf("Multiplication Of A = %d And B = %d Gives Result %d !!!\n\n", sk_a, sk_b, sk_result);
	}
	else if (sk_opt == 'D' || sk_opt == 'd')
	{
		printf("Enter Option In Character : \n\n");
		printf("'Q' or 'q' or '/' For Quotient Upon Division\n");
		printf("'R' or 'r' or '%%' For Remainder Upon Division\n");
		printf("\nEnter Option : ");
		sk_opt_division = getch();
		printf("\n\n");
		if (sk_opt_division == 'Q' || sk_opt_division == 'q' || sk_opt_division == '/')
		{
			if (sk_a >= sk_b)
			{
				sk_result = sk_a / sk_b;
				printf("Division Of A = %d By B = %d Gives Quotient = %d !!!\n\n", sk_a, sk_b, sk_result);
			}
			else
			{
				sk_result = sk_b / sk_a;
				printf("Division Of B = %d By A = %d Gives Quotient = %d !!!\n\n", sk_b, sk_a, sk_result);
			}
		}
		else if (sk_opt_division == 'R' || sk_opt_division == 'r' || sk_opt_division == '%')
		{
			if (sk_a >= sk_b)
			{
				sk_result = sk_a % sk_b;
				printf("Division Of A = %d By B = %d Gives Remainder = %d !!!\n\n", sk_a, sk_b, sk_result);
			}
			else
			{
				sk_result = sk_b % sk_a;
				printf("Division Of B = %d By A = %d Gives Remainder = %d !!!\n\n", sk_b, sk_a, sk_result);
			}
		}
		else
					printf("Invalid Character %c Input\n\n", sk_opt_division);
	}
	else
						printf("Invalid Character %c Entered !!! Please Try Again...\n\n", sk_opt);

	printf("If - Else If - Else Ladder Complete !!!\n");

	getch();
	return(0);

}
