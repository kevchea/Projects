#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256

typedef struct item{
  char *word;
  int weight;
}item;

typedef struct suggestions{
  char *word;
  int weight;
}suggestions;

void swap(item* A, int i, int j) {
    item temp = A[i];
    A[i] = A[j];
    A[j] = temp;
}

int partitionWeight(item* A, int start, int stop){
    int pivot = A[stop].weight;
    int i=start;
    for (int j=start; j < stop; j++){
        if (A[j].weight > pivot){
            swap(A,i,j);
            i++;
        }
    }
    swap(A,i,stop);
    return i;
}
void quickSortWeight(item* A, int start, int stop){
    if (start < stop) {
        int p = partitionWeight(A,start,stop);
        quickSortWeight(A,start,p-1);
        quickSortWeight(A,p+1,stop);
    }  
}

int partitionAlpha(item* A, int start, int stop){
    char* pivot = A[stop].word;
    int i=start;
    for (int j=start; j < stop; j++){
        if (strcmp(A[j].word, pivot) < 0){ 
            swap(A,i,j);
            i++;
        }
    }
    swap(A,i,stop);
    return i;
}
void quickSortAlpha(item* A, int start, int stop){
    if (start < stop) {
        int p = partitionAlpha(A,start,stop);
        quickSortAlpha(A,start,p-1);
        quickSortAlpha(A,p+1,stop);
    }
}    

int main(int argc, char **argv) {
    char *dictionaryFilePath = argv[1]; //this keeps the path to dictionary file
    char *queryFilePath = argv[2]; //this keeps the path to the file that keeps a list of query wrods, 1 query per line
    int wordCount=0; //this variable will keep a count of words in the dictionary, telling us how much memory to allocate
    int queryCount=0; //this variable will keep a count of queries in the query file, telling us how much memory to allocate for the query words
    
    ////////////////////////////////////////////////////////////////////////
    ///////////////////////// read dictionary file /////////////////////////
    ////////////////////////////////////////////////////////////////////////
    FILE *fp = fopen(dictionaryFilePath, "r");
    char *line = NULL; //variable to be used for line counting
    size_t lineBuffSize = 0; //variable to be used for line counting
    ssize_t lineSize; //variable to be used for line counting
    
    //check if the file is accessible, just to make sure...
    if(fp == NULL){
        fprintf(stderr, "Error opening file:%s\n",dictionaryFilePath);
        return -1;
    }

    //First, let's count number of lines. This will help us know how much memory to allocate (dictionary)
    while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1)
    {
        wordCount++;
    }

    //Printing wordCount for debugging purposes. You can remove this part from your submission.
    //printf("%d\n",wordCount);
    
    /////////////////PAY ATTENTION HERE/////////////////
    //This might be a good place to allocate memory for your data structure, by the size of "wordCount"
    item* dictionaryWords = (item*)malloc(sizeof(item)*wordCount); // dynamically allocates space

    ////////////////////////////////////////////////////

    
    //Read the file once more, this time to fill in the data into memory
    fseek(fp, 0, SEEK_SET);// rewind to the beginning of the file, before reading it line by line.
    char word[BUFSIZE]; //to be used for reading lines in the loop below
    int weight;
    for(int i = 0; i < wordCount; i++)
    {
        fscanf(fp, "%s %d\n",word,&weight);
        //Let's print them to the screen to make sure we can read input, for debugging purposes. You can remove this part from your submission.
        //printf("%s %d\n",word,weight);

        /////////////////PAY ATTENTION HERE/////////////////
        //This might be a good place to store the dictionary words into your data structure
        dictionaryWords[i].word = (char*)malloc(strlen(word) + 1);;
        dictionaryWords[i].weight = weight;
        strcpy(dictionaryWords[i].word, word);
        ////////////////////////////////////////////////////
    }
    //close the input file
    fclose(fp);

    ////////////////////////////////////////////////////////////////////////
    ///////////////////////// read query list file /////////////////////////
    ////////////////////////////////////////////////////////////////////////
    fp = fopen(queryFilePath, "r");
        
    //check if the file is accessible, just to make sure...
    if(fp == NULL){
        fprintf(stderr, "Error opening file:%s\n",queryFilePath);
        return -1;
    }

    //First, let's count number of queries. This will help us know how much memory to allocate
    while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1)
    {
        queryCount++;
    }
    free(line); //getline internally allocates memory, so we need to free it here so as not to leak memory!!

    //Printing line count for debugging purposes. You can remove this part from your submission.
    //printf("%d\n",queryCount);

    /////////////////PAY ATTENTION HERE/////////////////
    //This might be a good place to allocate memory for storing query words, by the size of "queryCount"
    item* queryWords = (item*)malloc(sizeof(item)*queryCount); // dynamically allocates space

    ////////////////////////////////////////////////////
    

    fseek(fp, 0, SEEK_SET);// rewind to the beginning of the file, before reading it line by line.
    for(int i = 0; i < queryCount; i++)
    {
        fscanf(fp, "%s\n",word);
        //Let's print them to the screen to make sure we can read input, for debugging purposes. You can remove this part from your submission.
        //printf("%s %d\n",word);
        
        /////////////////PAY ATTENTION HERE/////////////////
        //This might be a good place to store the query words in a list like data structure
        queryWords[i].word = (char*)malloc(strlen(word) + 1);                                                                                                                                                                                        
        strcpy(queryWords[i].word, word);

        ////////////////////////////////////////////////////   
    }
    //close the input file
    fclose(fp);

    ////////////////////////////////////////////////////////////////////////
    ///////////////////////// reading input is done ////////////////////////
    ////////////////////////////////////////////////////////////////////////
    
    //Now it is your turn to do the magic!!!
    //do search/sort/print, whatever you think you need to do to satisfy the requirements of the assignment!
    //loop through the query words and list suggestions for each query word if there are any
    //don't forget to free the memory before you quit the program!

    quickSortAlpha(dictionaryWords, 0, wordCount-1);
    /*
    printf("Sorted Dictionary:\n");
    for (int i = 0; i < wordCount; i++) {
    printf("%s %d\n", dictionaryWords[i].word, dictionaryWords[i].weight);
    }
    */
    
    //Search: You will need a search algorithm that will efficiently search for the given query. Keep in mind that,
    //your query will most probably be incomplete words. Thus, you will need to search for terms in the knowledge
    //base for entries whose initial characters match the query word. But the query can be an exact match as well.

    
    /*for (int i = 0; i < queryCount; i++){ //loop through each word in query
        int suggestionCounter = 0;
        item *suggestions = (item*)malloc(sizeof(item)*wordCount);

        /*for (int j = 0; j < wordCount; j++){ //loop through each word in dictionary
            int initialChar = strlen(queryWords[i].word);
            if (strncmp(dictionaryWords[j].word, queryWords[i].word, initialChar) == 0) { //compare each keystroke of query word to dictionary word
                suggestions[suggestionCounter] = dictionaryWords[j];
                suggestionCounter++;
            }

        }
        */
    for (int i = 0; i < queryCount; i++){ //iterate through each word in query file
        int low = 0;
        int high = wordCount - 1;
        int suggestionCounter = 0;
        item *suggestions = (item*)malloc(sizeof(item)*wordCount);
        printf("Query word:%s\n", queryWords[i].word);

        while (low <= high) { //binary search through dictionary file to find match
            int mid = low + (high-low) / 2;
            int initialChar = strlen(queryWords[i].word);

           if (strncmp(dictionaryWords[mid].word, queryWords[i].word, initialChar) == 0) { //Match
                suggestions[suggestionCounter] = dictionaryWords[mid];
                suggestionCounter++;

                //check for similar initial letters on left until nonmatch
                int left = mid-1;
                while ((left >= low) && (strncmp(dictionaryWords[left].word, queryWords[i].word, initialChar)) == 0){
                    suggestions[suggestionCounter] = dictionaryWords[left];
                    suggestionCounter++;
                    left--;
                }
                //check for similar initial letters on right until nonmatch
                int right = mid+1;
                while ((right <= high) && (strncmp(dictionaryWords[right].word, queryWords[i].word, initialChar)) == 0){
                    suggestions[suggestionCounter] = dictionaryWords[right];
                    suggestionCounter++;
                    right++;
                }
                break;
            }
            else if (strncmp(dictionaryWords[mid].word, queryWords[i].word, initialChar) < 0) {
                    low = mid + 1;
            }
            else {
                high = mid - 1;
            }
        }
                   
        if (suggestionCounter == 0){
            printf("No suggestion!\n");
        }
        else {
            quickSortWeight(suggestions, 0, suggestionCounter-1); //quicksort here to sort frequency ahead for output
            if (suggestionCounter<10){ 
                for (int n = 0; n < suggestionCounter; n++){
                    printf("%s %d\n", suggestions[n].word, suggestions[n].weight);
                }
            }
            else { //top 10
                for (int n = 0; n < 10; n++){
                    printf("%s %d\n", suggestions[n].word, suggestions[n].weight);
                }
            }   
        } 
    free(suggestions);
    }
    
    for (int i = 0; i < wordCount; i++) {
        free(dictionaryWords[i].word);
    }
    free(dictionaryWords);

    for (int i = 0; i < queryCount; i++) {
        free(queryWords[i].word);
    }
    free(queryWords);
        
    return 0;
}

