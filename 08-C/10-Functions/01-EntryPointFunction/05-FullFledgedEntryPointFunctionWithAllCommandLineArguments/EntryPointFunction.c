#include<stdio.h>

int main(int argc, char *argv[], char *envp[])
{
	int sk_i;

	printf("\nShruri, \"Hello World!\"\n");

	printf("Number of Command Line Arguements = %d\n\n", argc);

	printf("Command Line Arguements Passed to this Program are :\n\n");
	for (sk_i = 0; sk_i < argc; sk_i++)
	{
		printf("Command Line Arguement Number %d = %s\n", (sk_i + 1), argv[sk_i]);
	}

	printf("First 20 Environmental Variables Passed to this Program are:\n\n");
	for (sk_i = 0; sk_i < 20; sk_i++)
	{
		printf("Environmental Variable Number %d = %s\n", (sk_i + 1), envp[sk_i]);

	}

	getch();
	return(0);
}
