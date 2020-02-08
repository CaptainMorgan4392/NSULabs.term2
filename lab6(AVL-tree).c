#include <stdio.h>
#include <mm_malloc.h>

typedef enum {
    SUCCESS,
    ERROR
} ExitID;

typedef struct _node Node;

struct _node {
    int value;
    unsigned char height;
    Node* left;
    Node* right;
};

unsigned char height(Node* vertice) {
    return (unsigned char) (vertice ? vertice -> height : 0);
}

int balanceFactor(Node* vertice) {
    return height(vertice -> right) - height(vertice -> left);
}

void fixHeight(Node* p) {
    unsigned char heightLeft = height(p -> left);
    unsigned char heightRight = height(p -> right);
    p -> height = (unsigned char)((heightLeft > heightRight ? heightLeft : heightRight) + 1);
}

Node* rotateLeft(Node* q) {
    Node* p = q -> right;
    q -> right = p -> left;
    p -> left = q;
    fixHeight(q);
    fixHeight(p);
    return p;
}

Node* rotateRight(Node* p) {
    Node* q = p -> left;
    p -> left = q -> right;
    q -> right = p;
    fixHeight(p);
    fixHeight(q);
    return q;
}

Node* balance(Node* vertice) {
    fixHeight(vertice);
    if (balanceFactor(vertice) == 2) {
        if (balanceFactor(vertice -> right) < 0) {
            vertice -> right = rotateRight(vertice -> right);
        }
        vertice = rotateLeft(vertice);
    }
    if (balanceFactor(vertice) == -2) {
        if (balanceFactor(vertice -> left) > 0) {
            vertice -> left = rotateLeft(vertice -> left);
        }
        vertice = rotateRight(vertice);
    }

    return vertice;
}

Node* createNode(int value) {
    Node* newNode = (Node*)calloc(1, sizeof(Node));
    newNode -> value = value;
    newNode -> height = 1;
    return newNode;
}

Node* insert(Node* root, int value) {
    if (height(root) == 0) {
        return createNode(value);
    } else if (value <= root -> value) {
        root -> left = insert(root -> left, value);
    } else {
        root -> right = insert(root -> right, value);
    }

    return balance(root);
}

void inOrderFree(Node* root) {
    if (root -> left) {
        inOrderFree(root -> left);
    }
    if (root -> right) {
        inOrderFree(root -> right);
    }
    free(root);
}

int createAVL() {
    Node* root = NULL;
    int n;
    if (scanf("%d", &n) == 0) {
        printf("Bad input");
        free(root);
        return ERROR;
    } else for (int i = 0; i < n; i++) {
            int current;
            if (scanf("%d", &current) == 0) {
                printf("Bad input");
                return ERROR;
            }
            root = insert(root, current);
        }

    printf("%d", height(root));

    if (root) {
        inOrderFree(root);
    }

    return SUCCESS;
}

int main() {
    return createAVL();
}
