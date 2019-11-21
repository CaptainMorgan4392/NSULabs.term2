#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <mm_malloc.h>
#include <limits.h>

/* Exceptions */

const char* namesOfExceptions[] = {
        "bad number of vertices",
        "bad number of edges",
        "bad vertex",
        "bad length",
        "bad number of lines"
};

typedef enum {
    BAD_NUMBER_VERTICES = 1,
    BAD_NUMBER_EDGES,
    BAD_INDEX_VERTICE,
    BAD_LENGTH,
    BAD_INPUT
} Exceptions;

typedef enum {
    WHITE,
    BLACK
} Colors;

short flagOfException = 0;
bool isTreeExist = true;

typedef struct {
    size_t** connectivityTable;
    size_t** distancesBetweenVertices;
    int numberOfVertices;
    int numberOfEdges;
    size_t counterOfEdges;
} Graph;

void freeDynamicMemory(Graph* graph) {
    for (int i = 0; i < graph -> numberOfVertices; i++) {
        free(graph -> connectivityTable[i]);
    }
    free(graph -> connectivityTable);
    if (graph -> distancesBetweenVertices) {
        for (size_t i = 0; i < graph -> counterOfEdges; i++) {
            free(graph -> distancesBetweenVertices[i]);
        }
        free(graph -> distancesBetweenVertices);
    }
}

void checkQuantities(int n, int m) {
    if (n < 0) {
        printf("%s", namesOfExceptions[4]);
        flagOfException = BAD_INPUT;
        return;
    } else if (n > 1000) {
        printf("%s", namesOfExceptions[0]);
        flagOfException = BAD_NUMBER_VERTICES;
        return;
    } else if (m < 0) {
        printf("%s", namesOfExceptions[4]);
        flagOfException = BAD_INPUT;
        return;
    } else if (m > (n * (n + 1) / 2)) {
        printf("%s", namesOfExceptions[1]);
        flagOfException = BAD_NUMBER_EDGES;
        return;
    } else if (n == 0) {
        printf("no spanning tree");
        isTreeExist = false;
        return;
    } else if (n == 1 && m == 0) {
        isTreeExist = false;
        return;
    }
}

void checkParameters(int n, int v1, int v2, size_t length) {
    if (v1 < 1 || v1 > n) {
        printf("%s", namesOfExceptions[2]);
        flagOfException = BAD_INDEX_VERTICE;
        return;
    } else if (v2 < 1 || v2 > n) {
        printf("%s", namesOfExceptions[2]);
        flagOfException = BAD_INDEX_VERTICE;
        return;
    } else if (length > INT_MAX) {
        printf("%s", namesOfExceptions[3]);
        flagOfException = BAD_LENGTH;
        return;
    }
}

void getConnectivityTable(Graph* graph, int n, int m) {
    graph -> connectivityTable = (size_t**)calloc((size_t)n, sizeof(size_t*));
    for (int i = 0; i < n; i++) {
        graph -> connectivityTable[i] = (size_t*)calloc((size_t)n, sizeof(size_t));
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            graph -> connectivityTable[i][j] = 0;
        }
    }
    for (int i = 0; i < m; i++) {
        int verticeFirst, verticeSecond;
        size_t length;
        if (scanf("%d%d%zu", &verticeFirst, &verticeSecond, &length) != 3) {
            printf("%s", namesOfExceptions[4]);
            flagOfException = BAD_INPUT;
            freeDynamicMemory(graph);
            return;
        } else {
            checkParameters(n, verticeFirst, verticeSecond, length);
            if (flagOfException == 0) {
                graph -> counterOfEdges++;
                graph -> connectivityTable[verticeFirst - 1][verticeSecond - 1] = length;
                graph -> connectivityTable[verticeSecond - 1][verticeFirst - 1] = length;
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
        printf("%s", namesOfExceptions[4]);
        flagOfException = BAD_INPUT;
        return;
    }
    if (scanf("%d", &m) == 0) {
        printf("%s", namesOfExceptions[4]);
        flagOfException = BAD_INPUT;
        return;
    }
    checkQuantities(n, m);
    if (flagOfException == 0 && isTreeExist) {
        graph -> numberOfVertices = n;
        graph -> numberOfEdges = m;
        getConnectivityTable(graph, n, m);
    }
}

void merge(size_t** table, size_t left, size_t mid, size_t right) {
    size_t iterator1 = 0,
           iterator2 = 0;

    size_t** forUpdate = (size_t**)calloc(right - left, sizeof(size_t*));
    for (size_t i = 0; i < right - left; i++) {
        forUpdate[i] = (size_t*)calloc(3, sizeof(size_t));
    }

    while (left + iterator1 < mid && mid + iterator2 < right) {
        if (table[left + iterator1][2] < table[mid + iterator2][2]) {
            forUpdate[iterator1 + iterator2] = table[left + iterator1];
            iterator1++;
        } else {
            forUpdate[iterator1 + iterator2] = table[mid + iterator2];
            iterator2++;
        }
    }

    while (left + iterator1 < mid) {
        forUpdate[iterator1 + iterator2] = table[left + iterator1];
        iterator1++;
    }
    while (mid + iterator2 < right) {
        forUpdate[iterator1 + iterator2] = table[mid + iterator2];
        iterator2++;
    }

    for (size_t i = 0; i < iterator1 + iterator2; i++) {
        table[left + i] = forUpdate[i];
    }
}

void sortTable(size_t** table, size_t left, size_t right) {
    if (left + 1 >= right) {
        return;
    }

    size_t mid = (left + right) / 2;
    sortTable(table, left, mid);
    sortTable(table, mid, right);
    merge(table, left, mid, right);
}

void getSortedTableOfDistances(Graph *graph) {
    graph -> distancesBetweenVertices = (size_t**)calloc(graph -> counterOfEdges, sizeof(size_t*));
    for (size_t i = 0; i < graph -> counterOfEdges; i++) {
        graph -> distancesBetweenVertices[i] = (size_t*)calloc(3, sizeof(size_t));
    }

    size_t current = 0;
    for (size_t i = 0; i < graph -> counterOfEdges; i++) {
        for (size_t j = i; j < graph -> counterOfEdges; j++) {
            if (graph -> connectivityTable[i][j] != 0) {
                graph -> distancesBetweenVertices[current][0] = (size_t)i;
                graph -> distancesBetweenVertices[current][1] = (size_t)j;
                graph -> distancesBetweenVertices[current++][2] = graph -> connectivityTable[i][j];
            }
        }
    }
    sortTable(graph -> distancesBetweenVertices, 0, graph -> counterOfEdges);
}

bool hasUntiedVertice(Graph* graph) {
    for (int i = 0; i < graph -> numberOfVertices; i++) {
        bool hasBound = false;
        for (int j = 0; j < graph -> numberOfVertices; j++) {
            if (graph -> connectivityTable[i][j] > 0 && i != j) {
                hasBound = true;
            }
        }
        if (!hasBound) {
            return true;
        }
    }

    return false;
}

void KruskalAlgorithm(Graph *graph) {
    Colors* colorsOfVertices = calloc(graph -> counterOfEdges, sizeof(Colors));
    for (size_t i = 0; i < graph -> counterOfEdges; i++) {
        colorsOfVertices[i] = WHITE;
    }

    if (hasUntiedVertice(graph)) {
        printf("no spanning tree");
        free(colorsOfVertices);
        freeDynamicMemory(graph);
        return;
    }

    int currentNumberOfInvolvedVertice = 0;
    for (size_t i = 0; i < graph -> counterOfEdges; i++) {
        if (currentNumberOfInvolvedVertice == graph -> numberOfVertices) {
            break;
        }

        if (colorsOfVertices[graph -> distancesBetweenVertices[i][0]] == WHITE) {
            colorsOfVertices[graph -> distancesBetweenVertices[i][0]] = BLACK;
            if (colorsOfVertices[graph -> distancesBetweenVertices[i][1]] == WHITE) {
                colorsOfVertices[graph -> distancesBetweenVertices[i][1]] = BLACK;
                currentNumberOfInvolvedVertice++;
            }
            printf("%zu %zu\n", graph -> distancesBetweenVertices[i][0] + 1, graph -> distancesBetweenVertices[i][1] + 1);
            currentNumberOfInvolvedVertice++;
        } else if (colorsOfVertices[graph -> distancesBetweenVertices[i][1]] == WHITE) {
            colorsOfVertices[graph -> distancesBetweenVertices[i][1]] = BLACK;
            currentNumberOfInvolvedVertice++;
            printf("%zu %zu\n", graph -> distancesBetweenVertices[i][0] + 1, graph -> distancesBetweenVertices[i][1] + 1);
        } else {
            continue;
        }
    }
}

int main() {
    Graph* graph = calloc(1, sizeof(Graph));
    createGraph(graph);
    if (flagOfException != 0 || !isTreeExist) {
        free(graph);
        return flagOfException;
    }

    getSortedTableOfDistances(graph);
    KruskalAlgorithm(graph);
    return 0;
}
