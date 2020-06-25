#include<stdio.h>

int main(int argc, char* argv[], char* envp[])
{
	unsigned int sk_num;

	void RecursiveFunc(unsigned int);

	printf("\n\nEnter an Number\n");
	scanf("%d", &sk_num);

	printf("\n\nRecursive Funtion is\n");

	RecursiveFunc(sk_num);

	return(0);

}

void RecursiveFunc(unsigned int s)
{
	printf("\nS is %d\n", s);

	if (s > 0)
	{
		RecursiveFunc(s - 1);
	}
	
}
