#include <stdio.h>
#include <stdbool.h>
#include <mm_malloc.h>

/* Exceptions */
const char* namesOfExceptions[] = {
        "bad number of vertices",
        "bad number of edges",
        "bad vertex",
        "bad number of lines"
};

typedef enum {
    BAD_NUMBER_VERTICES = 1,
    BAD_NUMBER_EDGES,
    BAD_INDEX_OF_VERTICE,
    BAD_INPUT
} Exceptions;

short flagOfException = 0;
bool isPossibleToSort = true;

/* Definition of struct Stack and it's methods */

typedef struct {
    int array[1000];
    size_t size;
} Stack;

bool isEmpty(Stack* stack) {
    return stack -> size == 0;
}

void push(int vertice, Stack* stack) {
    stack -> array[stack -> size++] = vertice;
}

int peek(Stack* stack) {
    return stack -> array[stack -> size - 1];
}

void pop(Stack* stack) {
    stack -> size--;
}

void createStack(Stack* stack) {
    stack -> size = 0;
}

/* Definition of struct Graph and it's methods */

typedef struct {
    bool** connectivityTable;
    int numberOfVertices;
    int numberOfEdges;
} Graph;

typedef enum {
    WHITE,
    GREY,
    BLACK
} Colors;

void freeDynamicMemory(Graph* graph) {
    for (int i = 0; i < graph -> numberOfVertices; i++) {
        free(graph -> connectivityTable[i]);
    }
    free(graph -> connectivityTable);
}

void checkQuantities(int n, int m) {
    if (n < 0) {
        printf("%s", namesOfExceptions[3]);
        flagOfException = BAD_INPUT;
        return;
    } else if (n > 1000) {
        printf("%s", namesOfExceptions[0]);
        flagOfException = BAD_NUMBER_VERTICES;
        return;
    } else if (m < 0) {
        printf("%s", namesOfExceptions[3]);
        flagOfException = BAD_INPUT;
        return;
    } else if (m > (n * (n + 1) / 2)) {
        printf("%s", namesOfExceptions[1]);
        flagOfException = BAD_NUMBER_EDGES;
        return;
    }
}

void checkIndexes(int n, int v1, int v2) {
    if (v1 < 1 || v1 > n) {
        printf("%s", namesOfExceptions[2]);
        flagOfException = BAD_INDEX_OF_VERTICE;
        return;
    } else if (v2 < 1 || v2 > n) {
        printf("%s", namesOfExceptions[2]);
        flagOfException = BAD_INDEX_OF_VERTICE;
        return;
    }
}

void getConnectivityTable(Graph* graph, int n, int m) {
    graph -> connectivityTable = (bool**)calloc((size_t)n, sizeof(bool*));
    for (int i = 0; i < n; i++) {
        graph -> connectivityTable[i] = (bool*)calloc((size_t)n, sizeof(bool));
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            graph -> connectivityTable[i][j] = false;
        }
    }
    for (int i = 0; i < m; i++) {
        int verticeFrom, verticeTo;
        if (scanf("%d%d", &verticeFrom, &verticeTo) != 2) {
            printf("%s", namesOfExceptions[3]);
            flagOfException = BAD_INPUT;
            freeDynamicMemory(graph);
            return;
        } else {
            checkIndexes(n, verticeFrom, verticeTo);
            if (flagOfException == 0) {
                graph -> connectivityTable[verticeFrom - 1][verticeTo - 1] = true;
            } else {
                freeDynamicMemory(graph);
                return;
            }
        }
    }
}

void createGraph(Graph* graph) {
    int n = -1, m = -1;
    if (scanf("%d", &n) == 0) {
        printf("%s", namesOfExceptions[3]);
        flagOfException = BAD_INPUT;
        return;
    }
    if (scanf("%d", &m) == 0) {
        printf("%s", namesOfExceptions[3]);
        flagOfException = BAD_INPUT;
        return;
    }
    checkQuantities(n, m);
    if (flagOfException == 0) {
        graph -> numberOfVertices = n;
        graph -> numberOfEdges = m;
        getConnectivityTable(graph, n, m);
    }
}

/*  Main part of algorithm
    Topologic sort is implemented by Tarjan's algorithm                              Time: O(n) */

void dfs(Graph* graph, Stack* stackOfBlackVertices, Colors* colorsOfVertices, int indexOfVertice) {
    colorsOfVertices[indexOfVertice] = GREY;
    for (int j = 0; j < graph -> numberOfVertices; j++) {
        if (graph -> connectivityTable[indexOfVertice][j] == true) {
            Colors currentColor = colorsOfVertices[j];
            switch (currentColor) {
                case BLACK:
                    continue;
                case GREY:
                    isPossibleToSort = false;
                    break;
                case WHITE:
                    dfs(graph, stackOfBlackVertices, colorsOfVertices, j);
            }
        }
        if (!isPossibleToSort) {
            return;
        }
    }
    colorsOfVertices[indexOfVertice] = BLACK;
    push(indexOfVertice + 1, stackOfBlackVertices);
}

void topologicSort(Graph* graph) {
    Stack* stackOfBlackVertices = calloc(1, sizeof(Stack));
    createStack(stackOfBlackVertices);
    Colors* colorsOfVertices = calloc((size_t)graph -> numberOfVertices, sizeof(Colors));
    for (int i = 0; i < graph -> numberOfVertices; i++) {
        colorsOfVertices[i] = WHITE;
    }
    for (int i = 0; i < graph -> numberOfVertices; i++) {
        if (colorsOfVertices[i] == WHITE) {
            dfs(graph, stackOfBlackVertices, colorsOfVertices, i);
        }
    }
    if (!isPossibleToSort) {
        printf("impossible to sort");
        free(stackOfBlackVertices);
        free(colorsOfVertices);
        return;
    } else {
        while (!isEmpty(stackOfBlackVertices)) {
            printf("%d ", peek(stackOfBlackVertices));
            pop(stackOfBlackVertices);
        }
    }
    free(stackOfBlackVertices);
    free(colorsOfVertices);
}

int main() {
    Graph* graph = calloc(1, sizeof(Graph));
    createGraph(graph);
    if (flagOfException > 0) {
        free(graph);
        return flagOfException;
    }
    topologicSort(graph);
    freeDynamicMemory(graph);
    free(graph);
    return 0;
}
