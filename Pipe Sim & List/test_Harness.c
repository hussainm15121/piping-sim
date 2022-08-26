/****************************************************
*Purpose: Test Harness for Linked List              *
*Author: Hussain Mehdi                              *
*Last Edited: 27/07/2021			      *
*****************************************************/

#include<stdio.h>
#include "linkedList.h"
#include <stdlib.h>

int main(int argc, char** argv)
{
	printf("------------- TESTING LINKED LIST DATA STRUCTURE ----------------");
	 
	char testData[6]= "Alif";		//Test Data of type char
	printf("\nData to be Loaded: ");
	printf("\n%s", testData);
	char testTwo[6] = "Bravo";
	node *test = malloc(sizeof(node));	//Malloc to have enough space for data
		
	test->data = testData;			//Loading data into list
	
	printf("\nLoaded: ");
	printf("\n%s", test->data);
	printf("\nTEST COMPLETE\n");
	
}
