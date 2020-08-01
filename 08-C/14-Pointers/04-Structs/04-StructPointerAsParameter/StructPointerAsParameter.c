#include <stdio.h>

struct SData
{
	int i;
	float f;
	double d;
};

int main(void)
{
	//function declaration
	void ChangeValues(struct SData*);

	//variable declarations
	struct SData* sData = NULL;

	//code
	sData = (struct SData*)malloc(sizeof(struct SData));

	if (sData == NULL)
	{
		printf("\n\nMemory Allocation to 'struct SData' failed! Exitting Now.\n\n");
		exit(0);
	}
	else
	{
		printf("\n\nMemory allocation to 'struct SData' successfull!");
	}

	//Assigning values
	sData->i = 20;
	sData->f = 20.03f;
	sData->d = 18.32019;

	//Displaying Values
	
	printf("\n\nData members of 'struct SData' are : \n\n");
	printf("i = %d\n", sData->i);
	printf("f = %f\n", sData->f);
	printf("d = %lf\n", sData->d);

	ChangeValues(sData);

	//Displaying Values
	
	printf("\n\nData members of 'struct SData' are : \n\n");
	printf("i = %d\n", sData->i);
	printf("f = %f\n", sData->f);
	printf("d = %lf\n", sData->d);

	if(sData)
	{
		free(sData);
		sData = NULL;
		printf("\n\nMemory Allocated to 'struct SData' freed!\n\n");
	}

	getch();
	return(0);

}

void ChangeValues(struct SData* pParam_Data)
{
	//code
	pParam_Data->i = 1;
	pParam_Data->f = 1.6f;
	pParam_Data->d = 1.163679;
	
}
