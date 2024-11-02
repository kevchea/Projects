// PROGRAMMING ASSIGNMENT I STARTER CODE
/*In order to complete this assignment, you need to complete the function defintions in the section commented FUNCTION DEFINITIONS.
When you implement a function, make sure that you uncomment its provided function call from main so that your program can utilize it.*/

// HEADER FILES
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// CONSTANT DECLARATIONS
#define INITIAL_CAPACITY 2

// STRUCT DECLARATIONS
typedef struct entry
{
	char* name;
	char* lastname;
	float height;
	int age;
} Entry;

typedef struct list 
{
	int capacity;
	int size;
	Entry** data;
} List;

// FUNCTION DEFINTIONS
/* The function declarations have been given to you which should make the parameters and the return value for each function obvious.
The initializeEntry, freeEntry, printList, and printEntry function have been coded to start you off.*/

// Returns a pointer to an Entry struct initialized with the specified values for each of the 4 members of the struct (NO NEED TO CHANGE).
Entry* initializeEntry(char* name, char* lastname, float height, int age)
{
	Entry* newEntry = malloc(sizeof(Entry));
	
	newEntry->name = malloc(sizeof(char)*(strlen(name)+1));
	strcpy(newEntry->name, name);
	
	newEntry->lastname = malloc(sizeof(char)*(strlen(lastname)+1));
	strcpy(newEntry->lastname, lastname);
	
	newEntry->height = height;
	
	newEntry->age = age;
	
	return newEntry;
}

// Given a pointer to an Entry struct, this function frees the memory allocated for that Entry struct (NO NEED TO CHANGE).
void freeEntry(Entry* entry)
{
	if (entry != NULL)
	{
		free(entry->name);
		free(entry->lastname);
		free(entry);
	}
}
//Initialize a list: Your program is expected to generate an empty list (i.e., size = 0) with capacity 2.
List* initializeList(){
	List* myList = malloc(sizeof(List)); // dynamically allocates space
	myList->capacity = 2; //set capacity to 2
	myList->size = 0; //empty list
	myList->data = malloc(sizeof(Entry*) * 2);

	return myList;
}

/* Delete a list: You will be expected to delete the list when the command deleteList is provided as a command
in the input file. This operation should free all the memory allocated for and by the list, including the char
arrays that are allocated for each entry, and the array of entries themselves. If an insert command follows a
deleteList command, your program should run into a segmentation fault (which is the expected behavior in
this case) since you have already freed all the memory.
*/
void deleteList(List* myList){

	for (int i=0; i<myList->size; i++){
		freeEntry(myList->data[i]); // goes through list and free individual entries
	}
	free(myList->data); //free memory allocated for each entry
	free(myList); //free memory for list
}

/* Your program should (1) double the capacity of the list if the size equals the capacity
of the list and an insertion is attempted (insert functions)*/
void doubleCapacity(List* myList){
	myList->capacity = (myList->capacity) * 2;
	myList->data = realloc(myList->data, sizeof(Entry*) * (myList->capacity));
}

/*(2) halve its capacity if the size becomes less than half of the
capacity (deletion functions)*/
void halveCapacity(List* myList){
	myList->capacity = (myList->capacity) / 2;
	myList->data = realloc(myList->data, sizeof(Entry*) * (myList->capacity));
}

void insertToHead(List* myList, char* name, char* lastname, float height, int age){
	int arrayLen=myList->size-1;
	if ((myList->size)==(myList->capacity)){
		doubleCapacity(myList);
	}
	for (int i=arrayLen; i >= 0; i-- ){ //Starting from length of array, elements move one index higher to right until first element
			myList->data[i+1] = myList->data[i]; 

	}
	myList->data[0] = initializeEntry(name, lastname, height, age); //insert new entry to head
	myList->size = myList->size+1; //increase size count for new entry
	
}

void insertToTail(List* myList, char* name, char* lastname, float height, int age){
	int i=0;
	int arrayLen=myList->size-1;

	if ((myList->size)==(myList->capacity)){
		doubleCapacity(myList);
	}
	myList->data[arrayLen+1] = initializeEntry(name, lastname, height, age); //insert new entry after last entry
	myList->size = myList->size+1; //increase size count for new entry
}

void insertToPosition(List* myList, int position, char* name, char* lastname, float height, int age){
	int i;
	int arrayLen=myList->size-1;

	if ((myList->size)==(myList->capacity)){
		doubleCapacity(myList);
	}
	if ((myList->size<position)){
		printf("Invalid insert!\n");
	}
	else{

		for (i=arrayLen+1; i >= position; i--){	//starting from the last element and to position, shift elements one index higher to right
			myList->data[i] = myList->data[i-1];
		}
		myList->data[position] = initializeEntry(name, lastname, height, age); //insert new entry to position
		myList->size = myList->size+1; //increase size count for new entry
	}
}

/* Your program needs to be able to search the entries by name (only name,
not last name or anything else) and return the location of the entry in the list and −1 if the element is not in
the list.
– findPosition <name>
You can assume that there will not be two entries with the same name.
*/
int findPosition(List* myList, char* name){

	for (int i=0; i < myList->size; i++){
		int searcher = strcmp(name, myList->data[i]->name);
		if (searcher == 0){ // if name exists at i, return index
			return i;
		}
	}
	return -1;
}

void deleteFromHead(List* myList){
	if ((myList->size-1)<((myList->capacity)/2)){
		halveCapacity(myList);
	}
	if (myList->size == 0)
        {
		printf("Invalid delete!\n");
        }
	else{

		freeEntry(myList->data[0]); //free head element first than shift left
		for (int i=0;i< myList->size-1; i++){	//skip element 0 and other existing elements shift one index lower to left
			myList->data[i] = myList->data[i+1]; 
		}
		myList->size = myList->size-1; //decrease size count for deleted entry
	}
}

void deleteFromTail(List* myList){
	int arrayLen = myList->size-1;

	if ((myList->size-1)<((myList->capacity)/2)){
		halveCapacity(myList);
	}
	if (myList->size ==0){
		printf("Invalid delete!\n");
	}
	else{

		freeEntry(myList->data[arrayLen]); //delete entry at tail
		myList->size = myList->size-1; //decrease size count for deleted entry
	}
}

void deleteFromPosition(List* myList, int position){
	int arrayLen = myList->size-1;

	if ((myList->size-1)<((myList->capacity)/2)){
		halveCapacity(myList);
	}
	if ((myList->size-1<position)||(myList->size == 0)){
		printf("Invalid delete!\n");
	}
	else{

		freeEntry(myList->data[position]); //delete entry at position first then shift to left starting from 1 higher index

		for (int i=position; i < arrayLen; i++){
		myList->data[i] = myList->data[i+1];
		}
		myList->size = myList->size-1; //decrease size count for deleted entry
	}
}

// Given a pointer to a List struct, this function prints each Entry in that list (NO NEED TO CHANGE).
void printList(List* myList)
{
	if (myList->size == 0)
	{
		printf("List is empty!!\n");
	}
	else
	{
		for (int i = 0; i < myList->size; i++)
		{
			printf("[%d]\t%s\t%s\t%0.2f\t%d\n", i, myList->data[i]->name, myList->data[i]->lastname, myList->data[i]->height, myList->data[i]->age);
		}
	}
}

// Given a pointer to a List struct, this function prints out the size and capacity of that List (NO NEED TO CHANGE).
void printListInfo(List* myList)
{
	printf("size:%d, capacity:%d\n", myList->size, myList->capacity);
}

int main(int argc, char** argv) 
{
	FILE* fp = fopen(argv[1], "r");
	char* line = NULL;
	size_t lineBuffSize = 0;
	ssize_t lineSize;
	
	if (fp == NULL)
	{
		fprintf(stderr, "Error opening file\n");
		return -1;
	}
	
	List* myList;
	// Uncomment the following function call when you implement the initializeList() function
	myList = initializeList(); 
	
	while ((lineSize = getline(&line, &lineBuffSize, fp)) != -1)
	{
		char* token;
		const char delimiter[2] = " ";
		
		if (line[strlen(line) - 1] == '\n')
		{
			line[strlen(line) - 1] = '\0';
		}

		token = strtok(line, delimiter);
					
		if (strcmp(token, "insertToHead") == 0)
		{
			char* name;
			char* lastname;
			float height;
			int age;
			
			name = strtok(NULL, delimiter);
			lastname = strtok(NULL, delimiter);
			height = atof(strtok(NULL, delimiter));
			age = atoi(strtok(NULL, delimiter));
			// Uncomment the following insertToHead function call when you have implemented it
			insertToHead(myList, name, lastname, height, age);
		}
		else if (strcmp(token, "insertToTail") == 0)
		{
			char* name;
			char* lastname;
			float height;
			int age;
			
			name = strtok(NULL, delimiter);
			lastname = strtok(NULL, delimiter);
			height = atof(strtok(NULL, delimiter));
			age = atoi(strtok(NULL, delimiter));
			// Uncomment the following insertToTail function call when you have implemented it
			insertToTail(myList, name, lastname, height, age);
		}
		else if (strcmp(token, "insertToPosition") == 0)
		{
			char* name;
			char* lastname;
			float height;
			int age;
			int position;
			
			position = atoi(strtok(NULL, delimiter));
			name = strtok(NULL, delimiter);
			lastname = strtok(NULL, delimiter);
			height = atof(strtok(NULL, delimiter));
			age = atoi(strtok(NULL, delimiter));
			// Uncomment the following insertToPosition function call when you have implemented it
			insertToPosition(myList, position, name, lastname, height, age);
		}
		else if (strcmp(token, "findPosition") == 0)
		{
			char* name;
			
			name = strtok(NULL, delimiter);
			// Uncomment the following printf statement using the return value from a findPosition function call when you have implemented the function
			printf("%d\n", findPosition(myList, name));
		}
		else if (strcmp(token, "deleteFromHead") == 0)
		{
			// Uncomment the following deleteFromHead function call when you have implemented it
			deleteFromHead(myList);
		}
		else if (strcmp(token, "deleteFromTail") == 0)
		{
			// Uncomment the following insertToHead function call when you have implemented it
			deleteFromTail(myList);
		}
		else if (strcmp(token, "deleteFromPosition") == 0)
		{
			int position;
			
			position = atoi(strtok(NULL, delimiter));
			
			// Uncomment the following deleteFromPosition function call when you have implemented it
			deleteFromPosition(myList, position);
		}
		else if (strcmp(token, "printList") == 0)
		{
			printList(myList);
		}
		else if(strcmp(token, "printListInfo") == 0)
		{
			printListInfo(myList);
		}
		else if (strcmp(token, "deleteList") == 0)
		{
			// Uncomment the following deleteList function call when you have implemented it
			deleteList(myList);
		}
		else
		{
			printf("Invalid command: <%s>\n", token);
		}
	}	

	//getline internally allocates memory, so we need to free it here so as not to leak memory!!
	free(line);
	
	//we should close the file that we have been reading from before exiting!
	fclose(fp);

	return 0;
} 

