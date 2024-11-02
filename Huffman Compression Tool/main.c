#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR 256

struct MinHeapNode {
    char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
};

struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct MinHeapNode** array;
};

struct Node {
    char data;
    unsigned freq;
    struct Node *left, *right;
};

struct HuffmanCode {
    char character;
    char* code;
};

typedef struct code{
  unsigned int freq;
  char *binary_code;
} Code;

struct MinHeapNode* createNode(char data, unsigned freq) {
    struct MinHeapNode* node = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    node->left = node->right = NULL;
    node->data = data;
    node->freq = freq;
    return node;
}

struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq){
        smallest = left;
    }
    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq){
        smallest = right;
    }
    if (smallest != idx) {
        struct MinHeapNode* temp = minHeap->array[smallest];
        minHeap->array[smallest] = minHeap->array[idx];
        minHeap->array[idx] = temp;
        minHeapify(minHeap, smallest);
    }
}

void buildMinHeap(struct MinHeap* minHeap) {
    unsigned size = minHeap->size;
    unsigned n = size / 2;
    for (unsigned i = n; i >= 1; --i) {
        minHeapify(minHeap, i);
    }
}

struct HuffmanCode huffmanTable[MAX_CHAR]; // Closed hash table

// generate Huffman codes and store them in the hash table
void generateHuffmanCodes(struct MinHeapNode* root, char* code, int index, char huffmanCodes[MAX_CHAR][MAX_CHAR]) {
    if (root->left) {
        code[index] = '0';
        generateHuffmanCodes(root->left, code, index + 1, huffmanCodes);
    }
    if (root->right) {
        code[index] = '1';
        generateHuffmanCodes(root->right, code, index + 1, huffmanCodes);
    }
    if (!root->left && !root->right) {
        code[index] = '\0';
        // check if character has been generated into Huffman codes
        //printf("Character: %c: Huffman Code: %s\n", root->data, code);
        huffmanTable[root->data].character = root->data;
        huffmanTable[root->data].code = strdup(code);
        strcpy(huffmanCodes[(unsigned char)root->data], code);  // store the Huffman code for the char
    }
}
struct MinHeapNode* encodeHuffmanTree(char data[], int freq[], int size, char huffmanCodes[MAX_CHAR][MAX_CHAR]) {
    // encode using the Huffman Tree

    // temp array
    struct MinHeapNode* nodes[size];
    for (int i = 0; i < size; i++) {
        nodes[i] = createNode(data[i], freq[i]);
    }

    while (size > 1) {
        // 2 smallest nodes
        int min1 = -1, min2;
        for (int i = 0; i < size; i++) {
            if (min1 == -1 || nodes[i]->freq < nodes[min1]->freq) {
                min2 = min1;
                min1 = i;
            } 
            else if (min2 == -1 || nodes[i]->freq < nodes[min2]->freq) {
                min2 = i;
            }
        }

        struct MinHeapNode* left = nodes[min1];
        struct MinHeapNode* right = nodes[min2];
        struct MinHeapNode* sum = createNode('$', left->freq + right->freq);
        sum->left = left;
        sum->right = right;

        nodes[min1] = sum;
        nodes[min2] = nodes[size - 1];
        size--;
    }

    struct MinHeapNode* root = nodes[0];
    char arr[MAX_CHAR];
    memset(arr, 0, sizeof(arr));
    generateHuffmanCodes(root, arr, 0, huffmanCodes);
    return root;
}

struct MinHeapNode* reconstructHuffmanTree(struct HuffmanCode huffmanTable[MAX_CHAR]) {
    struct MinHeapNode* root = createNode('$', 0);
    for (int i = 0; i < MAX_CHAR; i++) {
        if (huffmanTable[i].code != NULL) {
            struct MinHeapNode* current = root;
            for (int j = 0; huffmanTable[i].code[j] != '\0'; j++) {
                // left
                if (huffmanTable[i].code[j] == '0') {
                    if (!current->left) {
                        current->left = createNode('$', 0);
                    }
                    current = current->left;
                }
                // right
                else {
                    if (!current->right) {
                        current->right = createNode('$', 0);
                    }
                    current = current->right;
                }
            }
            current->data = (char)i;
        }
    }
    return root;
}
void decodeHuffmanTree(struct MinHeapNode* root, char str[], FILE *outputFile) {
    // decode using the Huffman Tree
    struct MinHeapNode* current = root;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '0'){
            current = current->left;
        }
        else{
            current = current->right;
        }

        // check if current is a leaf node
        if (!(current->left) && !(current->right)) {
            fputc(current->data, outputFile);
            current = root; // reset current to root for next character
        }
    }
}

void freeHuffmanTree(struct MinHeapNode* root) {
    if (root == NULL) return;
    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);
    free(root);
}


int main(int argc, char **argv) {
    const char *mode = argv[1];

    //---------------------ENCODER--------------------------------//
    if (strcmp(mode, "encode") == 0) {
        const char *inputTextFilePath = argv[2];
        FILE *inputFile = fopen(inputTextFilePath, "r");
        if (inputFile == NULL){
            printf("Could not open file to read: %s\n",inputTextFilePath);
            return 1;
        }
        // initialize the code table, which will store the frequency of each character,
        // and eventually the binary code. We are allocating a space of 256 in the
        // table, and the character's ASCII code will serve as hashing the index for
        // that character.
        Code *codeTable = malloc(sizeof(Code)*256);
        // set counters to 0
        for(int i = 0; i < 256; i++){
            codeTable[i].freq = 0;
        }

        int totalNumOfCharacters=0;
        int c;
        // read the file character by character
        while ((c = fgetc(inputFile)) != EOF && c!='\n'){
            if (c < MAX_CHAR){
                codeTable[c].freq++;
                totalNumOfCharacters++;
            }
        }
        fclose(inputFile);

        char characters[MAX_CHAR];
        int characterFreqs[MAX_CHAR];
        int numOfCharacters = 0;

        for (int i = 0; i < MAX_CHAR; i++) {
            if (codeTable[i].freq > 0) {
                characters[numOfCharacters] = (char)i;
                characterFreqs[numOfCharacters] = codeTable[i].freq;
                numOfCharacters++;
            }
        }

        char huffmanCodes[MAX_CHAR][MAX_CHAR];
        memset(huffmanCodes, 0, sizeof(huffmanCodes));

        struct MinHeapNode* root = encodeHuffmanTree(characters, characterFreqs, numOfCharacters, huffmanCodes);

        char arr[MAX_CHAR]; // MAX_CHAR is enough for code length
        memset(arr, 0, sizeof(arr));
        generateHuffmanCodes(root, arr, 0, huffmanCodes);

        // write the huffman codes to the code table file
        FILE *codeTableFile = fopen(argv[3], "w");

        for (int i = 0; i < numOfCharacters; i++) {
            fprintf(codeTableFile, "%c\t%s\n", characters[i], huffmanCodes[characters[i]]);
            // check if characters have been written as huffman codes
            //printf("Character: %c: Huffman Code: %s\n", characters[i], huffmanCodes[characters[i]]);

        }
        fclose(codeTableFile);

        // encode text and write the encoded data to the output file
        FILE *encodedFile = fopen(argv[4], "w");
        inputFile = fopen(inputTextFilePath, "r"); // Reopen the input file to read the text again
        int encodedTextSizeInBits=0;;

        while ((c = fgetc(inputFile)) != EOF && c != '\n') {
            fputs(huffmanCodes[c], encodedFile); //huffman code for each character
            // check if the encoded data is correct
            // printf("Encoding character: %c with Huffman code: %s\n", c, huffmanCodes[c]);
            encodedTextSizeInBits += strlen(huffmanCodes[c]);

        }

        fclose(inputFile);
        fclose(encodedFile);

        //To print the statistics about the compression, use print statements as follows        
        int originalFileSizeInBits = totalNumOfCharacters * 8;
        double compressionRatio = (float)encodedTextSizeInBits / ((float)originalFileSizeInBits) * 100;

        printf("Original: %d bits\n", originalFileSizeInBits);
        printf("Compressed: %d bits\n", encodedTextSizeInBits);
        printf("Compression Ratio: %.2f%%\n", compressionRatio);
        free(codeTable);
        return 0;
    }
        /*
        void printHuffmanTree(struct Node* root) {
            if (root) {
                printf("Character: %c, Frequency: %d\n", root->data, root->freq);
                printHuffmanTree(root->left);
                printHuffmanTree(root->right);
            }
        }
        printHuffmanTree(root_of_tree);
        */

//---------------------DECODER--------------------------------//
//1) read code table: you can use fscanf() function, since the code table file is well structured. Alternatively, you can also use the read statements from above as was used for reading input text file.
//2) read encoded text, which is a single line consisting of 0/1 characters: This file is better be read character by character, for which you can use a code similar to getc() code above
//3) write the decoded text into file: for that, you can write it into the file using a code similar to fprintf() usages exemplified above.
    else if (strcmp(mode, "decode") == 0) {
        const char *inputCodeTablePath = argv[2];
        const char *inputEncodedFilePath = argv[3];
        const char *outputDecodedFilePath = argv[4];
        
        FILE *codeTable = fopen(inputCodeTablePath, "r");
        if (codeTable == NULL){
            printf("Could not open file to read: %s\n", inputCodeTablePath);
            return 1;
        }

        struct HuffmanCode huffmanTable[MAX_CHAR] = {0};
        char ch;
        char code[256];
        while (fscanf(codeTable, "%c\t%s\n", &ch, code) == 2) {
            huffmanTable[(unsigned char)ch].character = ch;
            huffmanTable[(unsigned char)ch].code = strdup(code);
        }

        fclose(codeTable);

        struct MinHeapNode* root = reconstructHuffmanTree(huffmanTable);

        FILE *encodedFile = fopen(inputEncodedFilePath, "r");
        if (encodedFile == NULL){
            printf("Could not open file to read: %s\n", inputEncodedFilePath);
            return 1;
        }
        fseek(encodedFile, 0, SEEK_END);
        long fileSize = ftell(encodedFile);
        fseek(encodedFile, 0, SEEK_SET);

        char *str = malloc(fileSize + 1);
        fread(str, 1, fileSize, encodedFile);
        str[fileSize] = '\0';

        fclose(encodedFile);

        // write decoded string to output
        FILE *decodedFile = fopen(outputDecodedFilePath, "w");
        decodeHuffmanTree(root, str, decodedFile);
        free(str);

        // free Huffman tree
        freeHuffmanTree(root);     
        return 0;
    }
}
