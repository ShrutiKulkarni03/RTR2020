/*
 * Stack
 */
#include "vmath.h"
using namespace vmath;
#pragma once

#define STACK_SIZE 100

typedef struct _STACK {

	int top;
    mat4 data[STACK_SIZE];

} STACK, *LPSTACK;


/* Creates a stack */
LPSTACK Create();

/* Delete a stack */
void Delete(LPSTACK);

/* Prints a stack */
void PrintStack(LPSTACK);


/*
 * Stack Operations:
 * 
 * Push():    Insert element into stack at top
 * Pop():     Remove element from stack at top
 * Peek():    Display value of element at top
 * IsEmpty(): Check if the stack is empty
 * IsFull():  Check if the stack is full
 * 
 */


/* Push int into stack pointed by LPSTACK */
void Push (LPSTACK, mat4);

/* Pop int from stack pointed by LPSTACK */
mat4  Pop  (LPSTACK);

/* Peek int from stack pointed by LPSTACK */
mat4  Peek (LPSTACK);

/* returns 1 if stack pointed by LPSTACK is empty, otherwise 0 */
int  IsEmpty (LPSTACK);

/* returns 1 if stack pointed by LPSTACK is full, otherwise 0 */
int  IsFull  (LPSTACK);



#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

/* Creates a stack */
LPSTACK Create()
{
    LPSTACK pStack = (LPSTACK)malloc(sizeof(STACK));
    pStack->top = -1;
    return pStack;
}

/* Delete a stack */
void Delete(LPSTACK pStack)
{
    if (pStack == NULL)
    {
        printf("\nNULL Stack pointer!");
        return;
    }

    free(pStack);
}


/* Push int into stack pointed by LPSTACK */
void Push(LPSTACK pStack, mat4 data)
{
    if (pStack == NULL)
    {
        printf("\nNULL Stack pointer!");
        return;
    }

    if (IsFull(pStack))
    {
        printf("\nStack is FULL, cannot push()...");
    }
    else
    {
        pStack->top++;
        pStack->data[pStack->top] = data;
    }
}

/* Pop int from stack pointed by LPSTACK */
mat4 Pop(LPSTACK pStack)
{
    if (pStack == NULL)
    {
        printf("\nNULL Stack pointer!");
        return -1;
    }

    if (IsEmpty(pStack))
    {
        printf("\nStack is EMPTY, cannot pop()...");
        return -1;
    }
    else
    {
        mat4 topData = pStack->data[pStack->top];
        pStack->top--;
        return topData;
    }
}

/* Peek int from stack pointed by LPSTACK */
mat4 Peek(LPSTACK pStack)
{
    if (pStack == NULL)
    {
        printf("\nNULL Stack pointer!");
        return -1;
    }

    if (IsEmpty(pStack))
    {
        printf("\nStack is EMPTY, cannot peek()...");
        return -1;
    }
    else
    {
        mat4 topData = pStack->data[pStack->top];
        return topData;
    }
}

/* returns 1 if stack pointed by LPSTACK is empty, otherwise 0 */
int IsEmpty(LPSTACK pStack)
{
    if (pStack == NULL)
    {
        printf("\nNULL Stack pointer!");
        return -1;
    }

    if (pStack->top == -1)
        return 1;

    return 0;
}

/* returns 1 if stack pointed by LPSTACK is full, otherwise 0 */
int IsFull(LPSTACK pStack)
{
    if (pStack == NULL)
    {
        printf("\nNULL Stack pointer!");
        return -1;
    }

    if (pStack->top == STACK_SIZE - 1)
        return 1;

    return 0;
}

/* Prints a stack */
void PrintStack(LPSTACK pStack)
{
    if (pStack == NULL)
    {
        printf("\nNULL Stack pointer!");
        return;
    }

    printf("\n\nStack: ");

    if (IsEmpty(pStack))
    {
        printf("\n<empty>");
    }
    else
    {
        for (int i = pStack->top; i >= 0; i--)
        {
            //printf("\n\t| %5d |", pStack->data[i]);
            //printf("\n\t+-------+");
        }
    }

}




