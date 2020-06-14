#include<stdio.h>

int main(int argc, char* argv[], char* envp[])
{
	void display_information(void);
	void function_country(void);

	display_information();
	function_country();

	return(0);

}

void display_information(void)
{
	void function_My(void);
	void function_Name(void);
	void function_Is(void);
	void function_FirstName(void);
	void function_MiddleName(void);
	void function_Surname(void);
	void function_ofAMC(void);

	function_My();
	function_Name();
	function_Is();
	function_FirstName();
	function_MiddleName();
	function_Surname();
	function_ofAMC();

}

void function_My(void)
{
	printf("\n\nMy");
}

void function_Name(void)
{
	printf("\n\nName");
}

void function_Is(void)
{
	printf("\n\nIs");
}

void function_FirstName(void)
{
	printf("\n\nShruti");
}

void function_MiddleName(void)
{
	printf("\n\nParesh");
}

void function_Surname(void)
{
	printf("\n\nKulkarni");
}

void function_ofAMC(void)
{
	printf("\n\nof AMC");
}

void function_country(void)
{
	printf("\n\nI live In India!\n\n");
}
