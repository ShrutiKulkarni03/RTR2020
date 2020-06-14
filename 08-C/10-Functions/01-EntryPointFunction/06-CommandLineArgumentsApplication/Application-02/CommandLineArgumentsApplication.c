#include<stdio.h>
#include<stdlib.h>


int main(int argc, char *argv[], char *envp[])
{
	int sk_i;
	
	if (argc != 4)
	{
		printf("\n\nInvalid Usage!\nEXITTING NOW\n\n");
		printf("Usage : CommandLineArgumentsApplication <first name> <middle name> <surname> \n\n");
		exit(0);
	}

	printf("\n\nYour Full Name Is : ");

	for (sk_i = 0; sk_i < argc; sk_i++)
	{
		printf("%s ", argv[sk_i]);
	}

	printf("\n\n");


	getch();
	return(0);
}
