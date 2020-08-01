#include<stdio.h>

int main(void)
{
	int AddIntegers(int, int);
	int SubtractIntegers(int, int);
	float AddFloats(float, float);

	typedef int(*AddIntsFnPtr)(int, int);
	AddIntsFnPtr ptrAddTwoIntegers = NULL;
	AddIntsFnPtr ptrFunc = NULL;

	typedef float(*AddFloatsFnPtr)(float, float);
	AddFloatsFnPtr ptrAddTwoFloats = NULL;

	int iAnswer = 0;
	float fAnswer = 0.0f;

	//code
	ptrAddTwoIntegers = AddIntegers;
	iAnswer = ptrAddTwoIntegers(17, 7);
	
	printf("\n\nSum Of Integers = %d", iAnswer);
	ptrFunc = SubtractIntegers;
	iAnswer = ptrFunc(15, 3);
	
	printf("\n\nSubtraction Of Integers = %d", iAnswer);
	ptrAddTwoFloats = AddFloats;
	fAnswer = ptrAddTwoFloats(6.6f, 9.7f);
	
	printf("\n\nSum Of Floating-Point Numbers = %f", fAnswer);

	getch();
	return(0);
}

int AddIntegers(int x, int y)
{
	
	int z;
	
	z = x + y;
	return(z);
}

int SubtractIntegers(int x, int y)
{
	int z;
	
	if (x > y)
		z = x - y;
	else
		z = x - y;

	return(z);
}

float AddFloats(float f_num1, float f_num2)
{
	float ans;
	
	ans = f_num1 + f_num2;

	return(ans);
}
