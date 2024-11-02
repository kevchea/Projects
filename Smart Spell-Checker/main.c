#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256

typedef struct node{
    char *value;
    struct node *next;
} Node;

typedef struct openHashTable{
    int size;
    Node **table;
} OpenHashTable;

Node *initializeNode(char *value,Node *next){
    Node *newNode = malloc(sizeof(Node));
    newNode->value = strdup(value);
    newNode->next = next;
    return newNode;
}

OpenHashTable *initializeHashTable(int size){
    OpenHashTable *hashTable = malloc(sizeof(OpenHashTable));
    hashTable->size = size;
    hashTable->table = malloc(sizeof(Node *)*size);
    for(int i=0;i<size;i++){
        hashTable->table[i] = NULL;
    }
    return hashTable;
} 

int hash(char *value,int N){
    unsigned long hash = 0;
    int i = 7;
    while (*value){
        hash = (i * hash + *value) % N;
        value++;
    }
return (int)(hash % N);
}

void insert(OpenHashTable *hashTable, char *value){
    int position = hash(value, hashTable->size);
    Node *ptr = hashTable->table[position];
    while(ptr!=NULL){
        if(strcmp(ptr->value, value)==0){
            return;
        }
        else{
            ptr=ptr->next;
        }
    }
    hashTable->table[position] = initializeNode(value,hashTable->table[position]);
} 

int isMember(OpenHashTable *hashTable, char *value){
    int position = hash(value, hashTable->size);
    Node *ptr = hashTable->table[position];
    while(ptr!=NULL){
        if(strcmp(ptr->value, value)==0){
            return 1;
        }
        else{
        ptr=ptr->next;
        }
    }
    return 0;
}

//suggestion functions
//inverted adjacent letters (ex: word vs wrod), words with a missing (word vs wor) or extra (word vs wordi) letter at the beginning or at the end

void invertChecker(OpenHashTable *hashTable, const char *word){ //wrod
    size_t wordLen = strlen(word);
    //char *copy = malloc(wordLen + 1); 
    char copy[wordLen + 1];
    strcpy(copy, word); //copy wrod to test

    for (int i=0; i < wordLen-1; i++){
        char temp = copy[i]; //i==0: swap wrod to rwod; i==1: swap wrod to word
        copy[i] = copy[i+1];
        copy[i+1] = temp;

        if(isMember(hashTable, copy)){
            printf("%s ", copy);
        }
        temp = copy[i];      //swap rwod back to wrod
        copy[i] = copy[i + 1];
        copy[i + 1] = temp;
    }
    //free(copy);
}

void missingChecker(OpenHashTable *hashTable, const char *word){ //wor
    size_t wordLen = strlen(word);
    //char *copy = malloc(wordLen + 2); //+1+1
    //char *copy2 = malloc(wordLen + 2); //+1+1
    char copy[wordLen + 2];

    for (char letter='a'; letter <= 'z'; letter++){
        copy[0] = letter; //prepend (d)
        strcpy(copy + 1, word); //copy word  -> iwor

        if(isMember(hashTable, copy)){
            printf("%s ", copy); //dwor is not a guesstion
        }

        strcpy(copy, word); //copy word -> wor
        copy[wordLen] = letter; //append (d)
        copy[wordLen + 1] = '\0'; //null term
        if(isMember(hashTable, copy)){
            printf("%s ", copy); //word is a suggestion
        }
    }
    //free(copy);
    //free(copy2);
}

void extraChecker(OpenHashTable *hashTable, const char *word){ //wordi
    size_t wordLen = strlen(word);
    //char *copy = malloc(wordLen); //-1
    //char *copy2 = malloc(wordLen); //-1
    char copy[wordLen];
    strcpy(copy, word + 1); //copy word and removed first letter (ordi)
    if(isMember(hashTable, copy)){
        printf("%s ", copy); //ordi is not a suggestion
        }

    strncpy(copy, word, wordLen-1); //copy word and remove last letter (word)
    copy[wordLen - 1] = '\0'; // Null terminate

    if(isMember(hashTable, copy)){
        printf("%s ", copy); //print word as a suggestion
        }
    
    //free(copy);
    //free(copy2);
}

int main(int argc, char **argv)
{
	char *dictionaryFilePath = argv[1]; //this keeps the path to the dictionary file file
	char *inputFilePath = argv[2]; //this keeps the path to the input text file
	char *check = argv[3]; // this keeps the flag to whether we should insert mistyped words into dictionary or ignore
	int numOfWords=0; //this variable will tell us how much memory to allocate

	int insertToDictionary;
	if(strcmp(check,"add")==0)
		insertToDictionary = 1;
	else
		insertToDictionary = 0;
    
	////////////////////////////////////////////////////////////////////
	//read dictionary file
    FILE *fp = fopen(dictionaryFilePath, "r");
    char *line = NULL; //variable to be used for line counting
    size_t lineBuffSize = 0; //variable to be used for line counting
    ssize_t lineSize; //variable to be used for line counting

    //check if the file is accessible, just to make sure...
    if(fp == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }

    //First, let's count number of words in the dictionary.
    //This will help us know how much memory to allocate for our hash table
    while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1)
        numOfWords++;

    //Printing line count for debugging purposes.
    //You can remove this part from your submission.
    //printf("%d\n",numOfWords);
    
    //HINT: You can initialize your hash table here, since you know the size of the dictionary
    OpenHashTable *dictionaryHash = initializeHashTable(numOfWords/10);

    
    //rewind file pointer to the beginning of the file, to be able to read it line by line.
    fseek(fp, 0, SEEK_SET);

    char wrd[BUFSIZE];
    for (int i = 0; i < numOfWords; i++)
    {
        fscanf(fp, "%s \n", wrd);
        //You can print the words for Debug purposes, just to make sure you are loading the dictionary as intended
        //printf("%d: %s\n",i,wrd);
        
        //HINT: here is a good place to insert the words into your hash table
        insert(dictionaryHash, wrd);

    }
    fclose(fp);
    
	////////////////////////////////////////////////////////////////////
	//read the input text file word by word
    fp = fopen(inputFilePath, "r");
	
	//check if the file is accessible, just to make sure...
	if(fp == NULL)
	{
		fprintf(stderr, "Error opening file\n");
		return -1;
	}

    //HINT: You can use a flag to indicate if there is a misspelled word or not, which is initially set to 1 == no typo
	int noTypo=1;

	//read a line from the input file
	while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1)
	{
		char *word;
        //These are the delimiters you are expected to check for. Nothing else is needed here.
		const char delimiter[]= " ,.:;!\n";

		//split the buffer by delimiters to read a single word
		word = strtok(line,delimiter); 
		
		//read the line word by word
		while(word!=NULL)
		{
            // You can print the words of the inpit file for Debug purposes, just to make sure you are loading the input text as intended
			//printf("%s\n",word);

            
            // HINT: Since this nested while loop will keep reading the input text word by word, here is a good place to check for misspelled words
            if(!isMember(dictionaryHash, word)){
                // INPUT/OUTPUT SPECS: use the following line for printing a "word" that is misspelled.
                printf("Misspelled word: %s\n",word);

                // INPUT/OUTPUT SPECS: use the following line for printing suggestions, each of which will be separated by a comma and whitespace.
                printf("Suggestions: "); //the suggested words should follow
                invertChecker(dictionaryHash, word);
                missingChecker(dictionaryHash, word);
                extraChecker(dictionaryHash, word);
                printf("\n");

                noTypo=0;
                if (insertToDictionary==1){
                    insert(dictionaryHash, word);
                }
            }        
			word = strtok(NULL,delimiter); 
		}
	}
	fclose(fp);
    
    //HINT: If the flag noTypo is not altered (which you should do in the loop above if there exists a word not in the dictionary), then you should print "No typo!"
    if(noTypo==1)
        printf("No typo!\n");
    

    // DON'T FORGET to free the memory that you allocated
for(int i = 0; i < dictionaryHash->size; i++) {
    Node *current = dictionaryHash->table[i];
    while(current != NULL) {
        Node *toDelete = current;
        current = current->next;
        free(toDelete->value); // free the string duplicated with strdup
        free(toDelete); // free the node itself
    }
}
free(dictionaryHash->table); // free the table of node pointers
free(dictionaryHash); // free the hash table structure

// Free the line buffer allocated by getline
free(line);
    
	return 0;
}
