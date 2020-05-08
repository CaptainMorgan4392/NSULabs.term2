#include <stdio.h>
#include <mm_malloc.h>
#include <memory.h>

#define NUMBER_OF_CHARS 256

#define boolean int
#define false 0
#define true 1

#define ZERO "0"
#define ONE "1"

typedef enum {
    SUCCESS,
    OUT_OF_MEMORY,
    WRONG_INPUT
} ExitCodes;

const char* exitMessages[] = {
        "success",
        "out of memory",
        "wrong input"
};

FILE* fileIn;
FILE* fileOut;



typedef struct _node Node;

struct _node {
    int freq;
    unsigned char symb;
    Node* left;
    Node* right;
};

ExitCodes HuffmanEncoding() {


    return SUCCESS;
}

ExitCodes getLength(int* length) {
    if (fread(length, sizeof(int), 1, fileIn) == 0) {
        return WRONG_INPUT;
    }

    return SUCCESS;
}

Node* getTree() {

}

void decodeText(Node* tree, int lengthOfText) {
    for (int i = 0; i < lengthOfText; i++) {


    }
}

ExitCodes HuffmanDecoding() {
    int lengthOfText;
    ExitCodes currentAction;

    if ((currentAction = getLength(&lengthOfText)) != SUCCESS) {
        return currentAction;
    }

    if (lengthOfText == 0) {
        return SUCCESS;
    }

    Node* newNode = getTree();
    if (!newNode) {
        return OUT_OF_MEMORY;
    }

    decodeText(newNode, lengthOfText);

    return SUCCESS;
}


ExitCodes start() {
    unsigned char funcSymbol;
    if (fread(&funcSymbol, sizeof(unsigned char), 1, fileIn) == 0) {
        return WRONG_INPUT;
    }

    switch (funcSymbol) {
        case 'c':
            return HuffmanEncoding();
        case 'd':
            return HuffmanDecoding();
        default:
            return WRONG_INPUT;
    }
}

int main() {
    fileIn = fopen("in.txt", "rb");
    fileOut = fopen("out.txt", "wb");

    ExitCodes exec;

    if ((exec = start()) != SUCCESS) {
        printf("%s", exitMessages[exec]);
    }

    fclose(fileIn);
    fclose(fileOut);

    return exec;
}

