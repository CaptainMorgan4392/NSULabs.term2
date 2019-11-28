#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <mm_malloc.h>
#include <limits.h>

#define MAX_VERTICES 5000
#define MAX_LENGTH INT_MAX

typedef enum {
    SUCCESS,
    BAD_NUMBER_VERTICES,
    BAD_NUMBER_EDGES,
    BAD_INDEX_VERTICE,
    BAD_LENGTH,
    BAD_INPUT
} ExitCodes;

int exitCode = SUCCESS;

const char* namesOfExceptions[] = {
        "ok",
        "bad number of vertices",
        "bad number of edges",
        "bad vertex",
        "bad length",
        "bad number of lines"
};

typedef struct _graph Graph;
typedef struct _dsu DSU;

struct _graph {
    int vertices;
    int edges;
    int** connectivityTable;
    bool created;
};

struct _dsu {
    int* sets;
    int* parents;
};

void freeDynamicMemory(Graph* g, DSU* dsu) {
    if (g) {
        if (g -> connectivityTable) {
            for (int i = 0; i < g -> vertices; i++) {
                free(g -> connectivityTable[i]);
            }
            free(g -> connectivityTable);
        }
        free(g);
    }

    if (dsu) {
        free(dsu -> sets);
        free(dsu -> parents);
        free(dsu);
    }
}

bool inRange(int value, int lowerBound, int upperBound) {
    if (value >= lowerBound && value <= upperBound) {
        return true;
    }
    return false;
}

bool checkNumberOfVertices(int vertices) {
    if (inRange(vertices, 0, MAX_VERTICES)) {
        return true;
    }
    return false;
}

bool checkNumberOfEdges(int edges, int vertices) {
    if (inRange(edges, 0, (vertices * (vertices + 1)) / 2)) {
        return true;
    }
    return false;
}

bool checkVertices(int first, int second, int vertices) {
    if (inRange(first, 1, vertices) && inRange(second, 1, vertices)) {
        return true;
    }
    return false;
}

bool checkLength(int length) {
    if (inRange(length, 0, MAX_LENGTH)) {
        return true;
    }
    return false;
}

void getConnectivityTable(Graph* g) {
    g -> connectivityTable = (int**)calloc((size_t)g -> vertices, sizeof(int*));
    for (int i = 0; i < g -> vertices; i++) {
        g -> connectivityTable[i] = (int*)calloc((size_t)g -> vertices, sizeof(int));
    }

    for (int i = 0; i < g -> edges; i++) {
        int verticeFirst,
            verticeSecond,
            length;
        if (scanf("%d%d%d", &verticeFirst, &verticeSecond, &length) != 3) {
            printf("%s", namesOfExceptions[exitCode = BAD_INPUT]);

            freeDynamicMemory(g, NULL);
            return;
        }
        if (!checkVertices(verticeFirst, verticeSecond, g -> vertices)) {
            printf("%s", namesOfExceptions[exitCode = BAD_INDEX_VERTICE]);
            freeDynamicMemory(g, NULL);
            return;
        }
        if (!checkLength(length)) {
            printf("%s", namesOfExceptions[exitCode = BAD_LENGTH]);
            freeDynamicMemory(g, NULL);
            return;
        }

        g -> connectivityTable[verticeFirst - 1][verticeSecond - 1] = length;
        g -> connectivityTable[verticeSecond - 1][verticeFirst - 1] = length;
    }
}

void createGraph(Graph* g) {
    g -> created = false;
    if (scanf("%d%d", &g -> vertices, &g -> edges) < 2) {
        printf("%s", namesOfExceptions[exitCode = BAD_INPUT]);
        freeDynamicMemory(g, NULL);
        return;
    }

    if (!checkNumberOfVertices(g -> vertices)) {
        printf("%s", namesOfExceptions[exitCode = BAD_NUMBER_VERTICES]);
        freeDynamicMemory(g, NULL);
        return;
    }
    if (!checkNumberOfEdges(g -> edges, g -> vertices)) {
        printf("%s", namesOfExceptions[exitCode = BAD_NUMBER_EDGES]);
        freeDynamicMemory(g, NULL);
        return;
    }
    getConnectivityTable(g);

    if (!exitCode) {
        g -> created = true;
    }
}

void KruskalAlgorithm(Graph* g) {
    
}

int main() {
    Graph* graph = calloc(1, sizeof(Graph));
    createGraph(graph);
    if (graph -> created) {
        KruskalAlgorithm(graph);
    }
    return exitCode;
}
