#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>


int main(int argc, char *argv[], char *envp[])
{
	int sk_i;
	int sk_num;
	int sk_sum = 0;

	if (argc == 1)
	{
		printf("\n\nNo Numbers given for Addition!\nEXITTING NOW\n\n");
		printf("Usage : CommandLineArgumentsApplication <first number> <second number> ...\n\n");
		exit(0);
	}

	printf("\n\nSum Of All Integer Command Line Arguements Is :\n\n");

	for (sk_i = 0; sk_i < argc; sk_i++)
	{
		sk_num = atoi(argv[sk_i]);
		sk_sum += sk_num;
	}

	printf("Sum = %d\n\n", sk_sum);


	getch();
	return(0);
}
