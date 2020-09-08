#include<stdio.h>
#include<stdlib.h>

int main(int argc, char* argv[])
{
	int output;

	switch (*argv[1])
	{
	case '+':
		output = atoi(argv[2]) + atoi(argv[3]) + atoi(argv[4]);
		printf("\n%d\n", output);
		break;

	case '*':
		output = atoi(argv[2]) * atoi(argv[3]) * atoi(argv[4]);
		printf("\n%d\n", output);
		break;

	}

	return(0);
	getch();

}
