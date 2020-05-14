#include <stdio.h>
#include <mm_malloc.h>
#include <memory.h>
#include <assert.h>

#define false 0
#define true 1

#define NUMBER_OF_CHARS 256

FILE* fileIn;
FILE* fileOut;

typedef enum {
    SUCCESS,
    OUT_OF_MEMORY,
    WRONG_INPUT,
    FILE_ERROR
} ExitCodes;

const char* exitMessages[] = {
        "success",
        "out of memory",
        "wrong input",
        "file error"
};

ExitCodes encoding();
ExitCodes decoding();

const char* ZERO = "0";
const char* ONE = "1";

/////Declare structures

typedef struct _node Node;
typedef struct _input_stream InputStream;
typedef struct _output_stream OutputStream;

///////////////////////

/////Starting methods
void getMessage(ExitCodes exitCode) {
    fprintf(fileOut,"%s", exitMessages[exitCode]);
}

int correctOption(unsigned char* option) {
    if (fread(option, sizeof(unsigned char), 2, fileIn) < 2 || option[1] != '\n') {
        return false;
    }

    return true;
}

/////////////////////

/////Definition of struct Node

struct _node {
    int freq;
    unsigned char symbol;
    Node* left;
    Node* right;
};

int isLeaf(Node* curNode) {
    return !curNode -> left && !curNode -> right;
}

Node** initNodePointerArray() {
    Node** newArray = (Node**)calloc(NUMBER_OF_CHARS + 1, sizeof(Node*));
    if (!newArray) {
        return NULL;
    }

    for (int i = 0; i < NUMBER_OF_CHARS; i++) {
        newArray[i] = (Node*)calloc(1, sizeof(Node));
        if (!newArray[i]) {
            for (int j = i - 1; j >= 0; j--) {
                free(newArray[j]);
            }

            free(newArray);
            return NULL;
        }

        newArray[i] -> symbol = (unsigned char)i;
    }

    return newArray;
}

//////////////////////////////

/////Definition of input stream

struct _input_stream {
    unsigned char* buffer;
    unsigned char current;
    int curIndex;
    int capacity;
};

InputStream* allocInputStream() {
    InputStream* newInputStream = (InputStream*)calloc(1, sizeof(InputStream));
    if (!newInputStream) {
        return NULL;
    }
    newInputStream -> buffer = (unsigned char*)calloc(9, sizeof(unsigned char));
    if (!newInputStream -> buffer) {
        free(newInputStream);
        return NULL;
    }

    return newInputStream;
}

int readSingleSymbol(InputStream* input) {
    return fread(&input -> current, sizeof(unsigned char), 1, fileIn) == 1;
}

int isNewSymbol(InputStream* input) {
    assert(input -> buffer);

    return input -> curIndex == 0;
}

unsigned char getCurBit(InputStream* input) {
    assert(input -> buffer);

    return input -> buffer[input -> curIndex];
}

///////////////////////////////

/////Definition of output stream

struct _output_stream {
    unsigned char* buffer;
    int curSize;
    int capacity;
};

OutputStream* allocOutputStream() {
    OutputStream* outputStream = (OutputStream*)calloc(1, sizeof(OutputStream));
    if (!outputStream) {
        return NULL;
    }
    outputStream -> buffer = (unsigned char*)calloc(2 * NUMBER_OF_CHARS + 2, sizeof(unsigned char));
    if (!outputStream -> buffer) {
        free(outputStream);
        return NULL;
    }

    return outputStream;
}

void writeByte(OutputStream* outputStream) {
    int toWrite = 0;
    for (int i = 0; i < 8; i++) {
        toWrite = 2 * toWrite + (outputStream -> buffer[i] - '0');
    }

    unsigned char result = (unsigned char)toWrite;
    fwrite(&result, sizeof(unsigned char), 1, fileOut);

    memmove(outputStream -> buffer, outputStream -> buffer + 8, outputStream -> curSize - 8);
    outputStream -> curSize -= 8;
    if (outputStream -> curSize >= 8) {
        writeByte(outputStream);
    }
}

void append(OutputStream* outputStream, const unsigned char* string) {
    for (int i = 0; i < (int)strlen((const char*)string); i++) {
        outputStream -> buffer[outputStream -> curSize++] = string[i];
    }


    if (outputStream -> curSize >= 8) {
        writeByte(outputStream);
    }
}

void writePadding(OutputStream* outputStream) {
    for (int i = outputStream -> curSize; i < 8; i++) {
        outputStream -> buffer[i] = '0';
    }

    outputStream -> curSize = 8;
    writeByte(outputStream);
}

////////////////////////////////


/////Encoding

int compareNodes(const void** a, const void** b) {
    const Node** first = (const Node**)a;
    const Node** second = (const Node**)b;

    return (*first) -> freq - (*second) -> freq;
}

int power(int base, int degree) {
    int answer = 1;
    for (int i = 1; i <= degree; i++) {
        answer *= base;
    }

    return answer;
}

int readInput(Node** arr, int* total) {
    InputStream* firstReading = (InputStream*)calloc(1, sizeof(InputStream));
    if (!firstReading) {
        return false;
    }

    while (readSingleSymbol(firstReading)) {
        if (firstReading -> current != '\n') {
            arr[firstReading->current] -> freq++;
            (*total)++;
        }
    }

    fseek(fileIn, 2, SEEK_SET);
    free(firstReading);
    return true;
}

int insertNewNode(Node** arr, int indexOfFirst) {
    Node* newNode = (Node*)calloc(1, sizeof(Node));
    if (!newNode) {
        return false;
    }
    newNode -> left = (Node*)calloc(1, sizeof(Node));
    newNode -> right = (Node*)calloc(1, sizeof(Node));
    if (!newNode -> left || !newNode -> right) {
        return false;
    }

    memcpy(newNode -> left, arr[indexOfFirst], sizeof(Node));
    memcpy(newNode -> right, arr[indexOfFirst + 1], sizeof(Node));
    newNode -> freq = newNode -> left -> freq + newNode -> right -> freq;
    memcpy(arr[indexOfFirst + 1], newNode, sizeof(Node));

    return true;
}

int buildTree(Node** arr, Node* tree) {
    int indexOfBeginning = -1;

    qsort(arr, NUMBER_OF_CHARS, sizeof(Node*), (__compar_fn_t)compareNodes);

    for (int i = 0; i < NUMBER_OF_CHARS; i++) {
        if (arr[i] -> freq > 0) {
            indexOfBeginning = i;
            break;
        }
    }

    for (int i = indexOfBeginning; i < NUMBER_OF_CHARS - 1; i++) {
        if (!insertNewNode(arr, i)) {
            return false;
        }

        int curLength = NUMBER_OF_CHARS - i - 1;
        qsort(arr + i + 1, curLength, sizeof(Node*), (__compar_fn_t)compareNodes);
    }

    memcpy(tree, arr[NUMBER_OF_CHARS - 1], sizeof(Node));

    return true;
}

unsigned char* getInBitArray(unsigned char symbol) {
    unsigned char* result = (unsigned char*)calloc(9, sizeof(unsigned char));
    if (!result) {
        return NULL;
    }

    for (int i = 7; i >= 0; i--) {
        result[7 - i] = ((symbol >> i) % 2) + '0';
    }

    return result;
}

void writeTree(Node* curNode, OutputStream* outputStream) {
    if (isLeaf(curNode)) {
        append(outputStream, (unsigned char*)ONE);
        unsigned char* symbolInBitArray = getInBitArray(curNode -> symbol);
        append(outputStream, symbolInBitArray);

        return;
    }

    append(outputStream, (unsigned char*)ZERO);
    writeTree(curNode -> left, outputStream);
    writeTree(curNode -> right, outputStream);
}

int writingData(int count, Node* tree) {
    fwrite(&count, sizeof(int), 1, fileOut);

    OutputStream* outputStream = allocOutputStream();
    if (!outputStream) {
        return false;
    }
    writeTree(tree, outputStream);
    writePadding(outputStream);

    return true;
}

void startFilling(unsigned char** table, Node* curNode, int number, int curLevel) {
    if (isLeaf(curNode)) {
        for (int i = 0; i < curLevel; i++) {
            switch ((number >> i) % 2) {
                case 0:
                    table[curNode -> symbol][i] = '0';
                    break;
                case 1:
                    table[curNode -> symbol][i] = '1';
                    break;
                default:
                    break;
            }
        }

        return;
    }

    startFilling(table, curNode -> left, number, curLevel + 1);
    startFilling(table, curNode -> right, number + power(2, curLevel), curLevel + 1);
}

unsigned char** initCodesTable(Node* tree) {
    unsigned char** newTable = (unsigned char**)calloc(NUMBER_OF_CHARS + 1, sizeof(unsigned char*));
    if (!newTable) {
        return NULL;
    }
    for (int i = 0; i < NUMBER_OF_CHARS; i++) {
        newTable[i] = (unsigned char*)calloc(NUMBER_OF_CHARS + 1, sizeof(unsigned char));
        if (!newTable[i]) {
            for (int j = i - 1; j >= 0; j--) {
                free(newTable[j]);
            }

            free(newTable);
            return NULL;
        }
    }

    startFilling(newTable, tree, 0, 0);

    return newTable;
}

int transformingAndZip(unsigned char** codes) {
    InputStream* secondRead = (InputStream*)calloc(1, sizeof(InputStream));
    if (!secondRead) {
        return false;
    }
    OutputStream* outputStream = allocOutputStream();
    if (!outputStream) {
        return false;
    }

    while (readSingleSymbol(secondRead)) {
        append(outputStream, codes[secondRead -> current]);
    }
    writePadding(outputStream);

    return true;
}

/////////////

ExitCodes encoding() {
    //First of all we have to build tree
    Node** arrayOfNodes = initNodePointerArray();
    if (!arrayOfNodes) {
        return OUT_OF_MEMORY;
    }

    int totalRead = 0;
    if (!readInput(arrayOfNodes, &totalRead)) {
        return OUT_OF_MEMORY;
    }
    if (totalRead == 0) {
        return SUCCESS;
    }

    Node* huffmanTree = (Node*)calloc(1, sizeof(Node));
    if (!huffmanTree) {
        return OUT_OF_MEMORY;
    }
    if (!buildTree(arrayOfNodes, huffmanTree)) {
        return OUT_OF_MEMORY;
    }

    //Writing data for decoding
    if (!writingData(totalRead, huffmanTree)) {
        return OUT_OF_MEMORY;
    }

    //Secondly we should get table of codes to encode symbols for O(1)
    unsigned char** codesTable = initCodesTable(huffmanTree);
    if (!codesTable) {
        return OUT_OF_MEMORY;
    }

    //Finally we're starting encoding
    if (!transformingAndZip(codesTable)) {
        return OUT_OF_MEMORY;
    }

    return SUCCESS;
}

void readSymbolAndInsert(InputStream* input, Node* curNode) {
    unsigned char result = 0;
    for (int i = 0; i < 8; i++) {
        if (isNewSymbol(input) && readSingleSymbol(input)) {
            input -> buffer = getInBitArray(input -> current);
        }

        result = 2 * result + (input -> buffer[input -> curIndex] - '0');
        input -> curIndex = (input -> curIndex + 1) % 8;
    }

    curNode -> symbol = result;
}

void readTree(InputStream* input, Node* root) {
    if (isNewSymbol(input) && readSingleSymbol(input)) {
        input -> buffer = getInBitArray(input -> current);
    }

    int isCurrentNodeLeaf = (input -> buffer[input -> curIndex] == '1');
    input -> curIndex = (input -> curIndex + 1) % 8;
    if (isCurrentNodeLeaf) {
        readSymbolAndInsert(input, root);

        return;
    }

    root -> left = (Node*)calloc(1, sizeof(Node));
    root -> right = (Node*)calloc(1, sizeof(Node));
    readTree(input, root -> left);
    readTree(input, root -> right);
}

ExitCodes getDecodingData(int* count, Node* rootOfTree) {
    if (fread(count, sizeof(int), 1, fileIn) == 0) {
        return WRONG_INPUT;
    }
    if (count == 0) {
        return SUCCESS;
    }

    InputStream* readingDecodingData = allocInputStream();
    if (!readingDecodingData) {
        return OUT_OF_MEMORY;
    }

    readTree(readingDecodingData, rootOfTree);

    free(readingDecodingData -> buffer);
    free(readingDecodingData);
    return SUCCESS;
}

void getSymbol(Node* curNode, InputStream* input) {
    if (isNewSymbol(input) && readSingleSymbol(input)) {
        input -> buffer = getInBitArray(input -> current);
    }

    switch (getCurBit(input)) {
        case '0':
            curNode = curNode -> left;
            break;
        case '1':
            curNode = curNode -> right;
            break;
    }
    input -> curIndex = (input -> curIndex + 1) % 8;

    if (isLeaf(curNode)) {
        fwrite(&curNode -> symbol, sizeof(unsigned char), 1, fileOut);

        return;
    }

    getSymbol(curNode, input);
}

ExitCodes unzip(int count, Node* root) {
    InputStream* input = allocInputStream();
    if (!input) {
        return OUT_OF_MEMORY;
    }

    Node* newNode = (Node*)calloc(1, sizeof(Node));
    if (!newNode) {
        free(input -> buffer);
        free(input);
        return OUT_OF_MEMORY;
    }

    for (int i = 0; i < count; i++) {
        memcpy(newNode, root, sizeof(Node));
        getSymbol(newNode, input);
        memcpy(newNode, root, sizeof(Node));
    }

    free(newNode);
    free(input -> buffer);
    free(input);
    return SUCCESS;
}

ExitCodes decoding() {
    //Firstly we have to read encoded data for decoding
    int count;
    Node* tree = (Node*)calloc(1, sizeof(Node));
    if (!tree) {
        return OUT_OF_MEMORY;
    }

    ExitCodes curAction;
    if ((curAction = getDecodingData(&count, tree)) != SUCCESS) {
        return curAction;
    }

    if (count == 0) {
        return SUCCESS;
    }

    //So we can start decoding
    if ((curAction = unzip(count, tree)) != SUCCESS) {
        return curAction;
    }

    return SUCCESS;
}

ExitCodes start() {
    fileIn = stdin;
    fileOut = stdout;

    if (!fileIn || !fileOut) {
        return FILE_ERROR;
    }

    /*unsigned char readingOption[3];
    if (!correctOption(readingOption)) {
        return WRONG_INPUT;
    }*/

    /*unsigned char option = readingOption[0];*/
    unsigned char option;
    if (fread(&option, sizeof(unsigned char), 1, fileIn) == 0) {
        return WRONG_INPUT;
    }

    switch (option) {
        case 'c':
            return encoding();
        case 'd':
            return decoding();
        default:
            return WRONG_INPUT;
    }
}

int main() {
    ExitCodes execution;
    if ((execution = start()) != SUCCESS) {
        getMessage(execution);
    }

    return execution;
}

