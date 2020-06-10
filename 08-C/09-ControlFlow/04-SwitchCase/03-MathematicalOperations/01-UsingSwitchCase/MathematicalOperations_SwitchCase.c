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
	printf("\n\nEnter Option In Character : \n\n");
	printf("'A' or 'a' For Addition\n");
	printf("'S' or 's' For Subtraction\n");
	printf("'M' or 'm' For Multiplication\n");
	printf("'D' or 'd' For Division\n\n");
	printf("Enter Option : "); 
	sk_opt = getch();
	printf("\n\n");

	switch (sk_opt)
	{
		
		case 'A':
		case 'a':
			sk_result = sk_a + sk_b;
			printf("Addition Of A = %d And B = %d Gives Result %d !!!\n\n", sk_a, sk_b, sk_result);
			break;
		
		case 'S':
		case 's':
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
			break;
		
		case 'M':
		case 'm':
			sk_result = sk_a * sk_b;
			printf("Multiplication Of A = %d And B = %d Gives Result %d !!!\n\n", sk_a, sk_b, sk_result);
			break;
		
		case 'D':
		case 'd':
			printf("Enter Option In Character : \n\n");
			printf("'Q' or 'q' or '/' For Quotient Upon Division : \n");
			printf("'R' or 'r' or '%%' For Remainder Upon Division : \n");
			printf("\nEnter Option : ");
			sk_opt_division = getch();
			printf("\n\n");

			switch (sk_opt_division)
			{
				
				case 'Q':
				case 'q':
				case '/':
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
					break; 
					
				case 'R':
				case 'r':
				case '%':
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
					break; 
				default:

					printf("Invalid Character %c Entered For Division !!! Please Try Again...\n\n", sk_opt_division);
						break; 
			} 
			break; 

		default: 
			printf("Invalid Character %c Input\n\n", sk_opt);
			break;
	} 


	printf("Switch Case Block Complete !!!\n");

	getch();
	return(0);
}
