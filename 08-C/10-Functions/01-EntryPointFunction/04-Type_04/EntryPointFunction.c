#include<stdio.h>

int main(int argc, char *argv[])
{
	int sk_i;

	printf("\nShruri, \"Hello World!\"\n");
	printf("Number of Command Line Arguements = %d\n\n", argc);

	printf("Command Line Arguements Passed to this Program are :\n\n");
	for (sk_i = 0; sk_i < argc; sk_i++)
	{
		printf("Command Line Arguement Number %d = %s\n", (sk_i + 1), argv[sk_i]);
	}

	getch();
	return(0);
}
