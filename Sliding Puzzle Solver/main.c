#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PuzzleState{
    int *board;
    int move;
	int k;
    struct PuzzleState *parent;
} PuzzleState;

typedef struct Node {
    PuzzleState *state;
    struct Node *next;
} Node;

typedef struct OpenHashTable {
    int size;
    Node **table;
} OpenHashTable;

typedef struct Queue{
    Node *front, *rear;
} Queue;

// hash operations
Node *initializeNode(PuzzleState *state, Node *next){
    Node *newNode = malloc(sizeof(Node));
    newNode->state = state; // Directly assign the pointer
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

unsigned int hash(PuzzleState *state, int N) {
    unsigned long hash = 0;
    for (int i = 0; i < state->k * state->k; i++) {
        hash = (31 * hash + state->board[i]) % N;
    }
return (int)(hash % N);
}

void insert(OpenHashTable *hashTable, PuzzleState *state){
    int position = hash(state, hashTable->size);
    Node *ptr = hashTable->table[position];
    while(ptr!=NULL){
        if(memcmp(ptr->state->board, state->board, state->k * state->k * sizeof(int)) == 0){
            return;
        }
        else{
            ptr=ptr->next;
        }
    }
    hashTable->table[position] = initializeNode(state,hashTable->table[position]);
} 

int isMember(OpenHashTable *hashTable, PuzzleState *state){
    int position = hash(state, hashTable->size);
    Node *ptr = hashTable->table[position];
    while(ptr!=NULL){
        if(memcmp(ptr->state->board, state->board, state->k * state->k * sizeof(int)) == 0){
            return 1;
        }
        else{
        ptr=ptr->next;
        }
    }
    return 0;
}

void freeHashTable(OpenHashTable *hashTable) {
    if (hashTable == NULL) {
        return;
    }

    for (int i = 0; i < hashTable->size; i++) {
        Node *current = hashTable->table[i];
        while (current != NULL) {
            Node *temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(hashTable->table);
    free(hashTable);
}

// queue operations
Queue *createQueue();
void enqueue(Queue *q, PuzzleState *state);
PuzzleState *dequeue(Queue *q);
int isQueueEmpty(Queue *q);
void freeQueue(Queue *q);

Queue* createQueue() {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (!q) {
        fprintf(stderr, "Memory allocation failed for queue\n");
        return NULL;
    }
    q->front = q->rear = NULL;
    return q;
}

void enqueue(Queue *q, PuzzleState *state) {
    if (!q) {
        return;
    }

    Node *newNode = (Node *)malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed for queue node\n");
        return;
    }

    newNode->state = state;
    newNode->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = newNode;
    } 
    else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

PuzzleState *dequeue(Queue *q) {
    if (!q || !q->front) {
        return NULL;
    }

    Node *temp = q->front;
    PuzzleState *state = temp->state;
    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    return state;
}

int isQueueEmpty(Queue *q) {
    return (q == NULL || q->front == NULL);
}

void freeQueue(Queue *q) {
    if (!q) {
        return;
    }
    Node *current = q->front;
    while (current) {
        Node *temp = current;
        current = current->next;
        free(temp->state->board);
        free(temp->state);
        free(temp);
    }

    free(q);
}

// puzzle operations
void solvePuzzle(PuzzleState *initialState, FILE *fp_out, PuzzleState **goalState, OpenHashTable *hashTable, int k);

int isGoalState(PuzzleState *state) {
    int k = state->k;
    for (int i = 0; i < k * k - 1; i++) {
        if (state->board[i] != i + 1) return 0;
    }
    return state->board[k * k - 1] == 0;
}

void generateNeighbor(PuzzleState *currentState, int k, Queue *queue, OpenHashTable *hashTable, int dx, int dy) {
    int blankPos = -1;
    // current blankPosition
    for (int i = 0; i < k * k; i++) {
        if (currentState->board[i] == 0) {
            blankPos = i;
            break;
        }
    }
    
    // new position
    int newRow = blankPos / k + dx;
    int newCol = blankPos % k + dy;

    int newBlankPos = newRow * k + newCol;
    
    if (newRow >= 0 && newRow < k && newCol >= 0 && newCol < k) {

        int *newBoard = malloc(sizeof(int) * k * k);
        memcpy(newBoard, currentState->board, sizeof(int) * k * k);
        
        // swap
        newBoard[blankPos] = newBoard[newBlankPos];
        newBoard[newBlankPos] = 0;

        PuzzleState *newState = malloc(sizeof(PuzzleState));
        newState->board = newBoard;
        newState->move = newBoard[blankPos];
        newState->parent = currentState;
        newState->k = k;

        if (isMember(hashTable, newState)) {
        // revisited state so free allocated memory
            free(newBoard);
            free(newState);
        } else {
        // new state
            enqueue(queue, newState);
            insert(hashTable, newState);
        }
    }
}

void generateLeftNeighbor(PuzzleState *currentState, int k, Queue *queue, OpenHashTable *hashTable) {
    generateNeighbor(currentState, k, queue, hashTable, 0, -1);
}

void generateRightNeighbor(PuzzleState *currentState, int k, Queue *queue, OpenHashTable *hashTable) {
    generateNeighbor(currentState, k, queue, hashTable, 0, 1);
}

void generateTopNeighbor(PuzzleState *currentState, int k, Queue *queue, OpenHashTable *hashTable) {
    generateNeighbor(currentState, k, queue, hashTable, -1, 0);
}

void generateBottomNeighbor(PuzzleState *currentState, int k, Queue *queue, OpenHashTable *hashTable) {
    generateNeighbor(currentState, k, queue, hashTable, 1, 0);
}


int outputSolution(PuzzleState *goalState, FILE *fp_out, int **outMoves) {
    // backtrack from the goal state to the initial state and output sequence of moves to fp_out
	int moveCount = 0;
    PuzzleState *currentState = goalState;

    // find number of moves
    while (currentState && currentState->parent != NULL) {
        moveCount++;
        currentState = currentState->parent;
    }

    // store moves
    *outMoves = (int *)malloc(moveCount * sizeof(int));
    if (!(*outMoves)) {
        fprintf(stderr, "Memory allocation failed\n");
        return 0;
    }

    // backtrack
    currentState = goalState;
    int moveIndex = moveCount - 1;
    while (currentState->parent != NULL) {
        (*outMoves)[moveIndex--] = currentState->move;
        currentState = currentState->parent;
    }

    // print moves
    fprintf(fp_out, "#moves\n");
    for (int i = 0; i < moveCount; i++) {
        fprintf(fp_out, "%d ", (*outMoves)[i]);
    }
    fprintf(fp_out, "\n");

	return moveCount;
}

void solvePuzzle(PuzzleState *initialState, FILE *fp_out, PuzzleState **goalState, OpenHashTable *hashTable, int k){
    Queue *q = createQueue();
    *goalState = NULL;

    insert(hashTable, initialState);
    enqueue(q, initialState);

    while (!isQueueEmpty(q)) {
        PuzzleState *current = dequeue(q);

        if (isGoalState(current)) {
            *goalState = current;
            break;
        }

            generateBottomNeighbor(current, k, q, hashTable);
            generateTopNeighbor(current, k, q, hashTable);
            generateLeftNeighbor(current, k, q, hashTable);
            generateRightNeighbor(current, k, q, hashTable);

        /*
        for (int i = 0; i < numNextStates; i++) {
            if (!isMember(hashTable, current)) {
                insert(hashTable, initialState);
                enqueue(q, initialState);
            } else {
                // Free the state if it has already been visited
                free(current->board);
                free(current);
            }
        }
        */

        // free currentState if currentState is not goalState
        /*
        if (current != *goalState) {
            free(current->board);
            free(current);
        }
        */
    }

    freeQueue(q);
}



int main(int argc, char **argv)
{
	FILE *fp_in,*fp_out;
	
	fp_in = fopen(argv[1], "r");
	if (fp_in == NULL){
		printf("Could not open a file.\n");
		return -1;
	}
	
	fp_out = fopen(argv[2], "w");
	if (fp_out == NULL){
		printf("Could not open a file.\n");
		return -1;
	}
	
	char *line = NULL;
	size_t lineBuffSize = 0;
	ssize_t lineSize;
	int k;
	
	getline(&line,&lineBuffSize,fp_in);//ignore the first line in file, which is a comment
	fscanf(fp_in,"%d\n",&k);//read size of the board
	//printf("k = %d\n", k); //make sure you read k properly for DEBUG purposes
	getline(&line,&lineBuffSize,fp_in);//ignore the second line in file, which is a comment

	int initial_board[k*k];//get kxk memory to hold the initial board
	for(int i=0;i<k*k;i++)
		fscanf(fp_in,"%d ",&initial_board[i]);
	//printBoard(initial_board, k);//Assuming that I have a function to print the board, print it here to make sure I read the input board properly for DEBUG purposes
	fclose(fp_in);

	////////////////////
	// do the rest to solve the puzzle
    OpenHashTable *hashTable = initializeHashTable(10103);
    Queue *queue = createQueue(100000);

    PuzzleState *initialState = malloc(sizeof(PuzzleState));
    initialState->board = malloc(sizeof(int) * k * k);
    memcpy(initialState->board, initial_board, sizeof(int) * k * k);
    initialState->k = k;
    initialState->parent = NULL;
    initialState->move = -1;

    insert(hashTable, initialState);
    enqueue(queue, initialState);


    int goal[k * k];
    for (int i = 1; i < k * k - 1; ++i) {
        goal[i-1] = i;
    }
    goal[k * k - 1] = 0;

    PuzzleState *goalState = NULL;
    solvePuzzle(initialState, fp_out, &goalState, hashTable, k);

    /*
    while (!isQueueEmpty(queue)) {
        PuzzleState* currentState = dequeue(queue);

        if (isGoalState(currentState) { // goal found
            goalState = currentState;
            break;
        } 
        else {
            generateBottomNeighbor(currentState, k, queue, OpenHashTable);
            generateTopNeighbor(currentState, k, queue, OpenHashTable);
            generateLeftNeighbor(currentState, k, queue, OpenHashTable);
            generateRightNeighbor(currentState, k, queue, OpenHashTable);
        }
    }
    */

    if (goalState != NULL) {
        // output the solution
        int *moves;
        int moveCount = outputSolution(goalState, fp_out, &moves);
        free(moves);
        free(goalState->board);
        free(goalState);
    }
    else {
        fprintf(fp_out, "#moves\nno solution\n");
    }

    freeHashTable(hashTable);
    free(line);
    freeQueue(queue);
    fclose(fp_out);

    return 0;
}