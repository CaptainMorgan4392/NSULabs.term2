#define boolean int
#define false 0
#define true 1
#define MAX_VERTICES 5000

#include <stdio.h>
#include <mm_malloc.h>
#include <limits.h>

#define MAX_LENGTH INT_MAX

const char* exitMessages[] = {
        "success",
        "bad number of vertices",
        "bad number of edges",
        "bad vertex",
        "bad length",
        "bad number of lines"
};

typedef enum {
    SUCCESS,
    BAD_NUMBER_VERTICES,
    BAD_NUMBER_EDGES,
    BAD_VERTEX,
    BAD_LENGTH,
    BAD_INPUT
} ExitCodes;

typedef struct _graph Graph;

struct _graph {
    int vertices;
    long** distancesTable;
};

void freeDynamicMemory(Graph* g) {
    if (g -> distancesTable) {
        for (int i = 0; i < g -> vertices; i++) {
            free(g -> distancesTable[i]);
        }
        free(g -> distancesTable);
    }
    free(g);
}

boolean inRange(int val, int lowerBound, int upperBound) {
    if (lowerBound <= val && val <= upperBound) {
        return true;
    }
    return false;
}

boolean checkVertices(int val) {
    if (inRange(val, 0, 5000)) {
        return true;
    }
    return false;
}

boolean checkEdges(int edges, int vertices) {
    if (inRange(edges, 0, vertices * (vertices + 1) / 2)) {
        return true;
    }
    return false;
}

boolean checkIndexVertice(int val, int upper) {
    if (inRange(val, 1, upper)) {
        return true;
    }
    return false;
}

boolean checkLength(long length) {
    if (0 <= length && length <= MAX_LENGTH) {
        return true;
    }
    return false;
}

ExitCodes createGraph(Graph* g, int vertices, int edges) {
    g = calloc(1, sizeof(Graph));
    g -> distancesTable = (long**)calloc((size_t)vertices, sizeof(long*));
    for (int i = 0; i < vertices; i++) {
        g -> distancesTable[i] = (long*)calloc((size_t)vertices, sizeof(long));
    }

    for (int i = 0; i < edges; i++) {
        int verticeFrom, verticeTo;
        long length;
        if (scanf("%d%d%li", &verticeFrom, &verticeTo, &length) != 3) {
            printf("%s", exitMessages[BAD_INPUT]);
            return BAD_INPUT;
        } else if (!checkIndexVertice(verticeFrom, vertices) || !checkIndexVertice(verticeTo, vertices)) {
            printf("%s", exitMessages[BAD_VERTEX]);
            return BAD_VERTEX;
        } else if (!checkLength(length)) {
            printf("%s", exitMessages[BAD_LENGTH]);
            return BAD_LENGTH;
        }
    }

    return SUCCESS;
}

void startAlgorithm(Graph* g) {

}

ExitCodes DijkstraAlgorithm() {
    int vertices;
    if (scanf("%d", &vertices) == 0) {
        printf("%s", exitMessages[BAD_INPUT]);
        return BAD_INPUT;
    } else if (!checkVertices(vertices)) {
        printf("%s", exitMessages[BAD_NUMBER_VERTICES]);
        return BAD_NUMBER_VERTICES;
    }

    int start, end;
    if (scanf("%d%d", &start, &end) < 2) {
        printf("%s", exitMessages[BAD_INPUT]);
        return BAD_INPUT;
    } else if (!checkIndexVertice(start, vertices) || !checkIndexVertice(end, vertices)) {
        printf("%s", exitMessages[BAD_VERTEX]);
        return BAD_VERTEX;
    }

    int edges;
    if (scanf("%d", &edges) == 0) {
        printf("%s", exitMessages[BAD_INPUT]);
        return BAD_INPUT;
    } else if (!checkEdges(edges, vertices)) {
        printf("%s", exitMessages[BAD_NUMBER_EDGES]);
        return BAD_NUMBER_EDGES;
    }

    ExitCodes msgByCreatingGraph = SUCCESS;
    Graph* graph;
    if ((msgByCreatingGraph = createGraph(graph, vertices, edges)) != SUCCESS) {
        printf("%s", exitMessages[msgByCreatingGraph]);
        return msgByCreatingGraph;
    }

    //startAlgorithm(graph);

    return SUCCESS;
}

int main() {
    return DijkstraAlgorithm();
}
