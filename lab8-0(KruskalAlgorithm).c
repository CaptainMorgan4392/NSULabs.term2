#define boolean int
#define false 0
#define true 1

#include <stdio.h>
#include <stdlib.h>
#include <mm_malloc.h>
#include <limits.h>

#define MAX_VERTICES 5000
#define MAX_LENGTH INT_MAX

typedef enum {
    SUCCESS,
    OUT_OF_MEMORY,
    BAD_NUMBER_VERTICES,
    BAD_NUMBER_EDGES,
    BAD_VERTEX,
    BAD_LENGTH,
    BAD_INPUT,
    NO_SPAN_TREE
} ExitCodes;

const char* exitMessages[] = {
        "success",
        "no memory",
        "bad number of vertices",
        "bad number of edges",
        "bad vertex",
        "bad length",
        "bad number of lines",
        "no spanning tree"
};

typedef struct _edge Edge;

struct _edge {
    int first;
    int second;
    int length;
};

void swap(int* v1, int* v2) {
    int temp = *v1;
    *v1 = *v2;
    *v2 = temp;
}

int find(int s, int* dsu) {
    if (s == dsu[s]) {
        return s;
    }

    return dsu[s] = find(dsu[s], dsu);
}

boolean unite(int s1, int s2, int* dsu) {
    s1 = find(s1, dsu);
    s2 = find(s2, dsu);
    if (s1 != s2) {
        if (s2 < s1) {
            swap(&s1, &s2);
        }

        dsu[s2] = s1;

        return true;
    }

    return false;
}

Edge* createArray(int edges) {
    Edge* array = (Edge*)calloc((size_t)edges + 1, sizeof(Edge));
    if (!array) {
        return NULL;
    }

    return array;
}

ExitCodes getInputAndCheck(int *vertices, int *edges) {
    if (scanf("%d%d", vertices, edges) < 2) {
        return BAD_INPUT;
    }

    if (*vertices < 0 || *vertices > MAX_VERTICES) {
        return BAD_NUMBER_VERTICES;
    }

    if (*edges < 0 || *edges > (*vertices) * (*vertices + 1) / 2) {
        return BAD_NUMBER_EDGES;
    }

    return SUCCESS;
}

boolean checkSpanTree(int vertices, int edges, Edge* arrayOfEdges, int* dsu) {
    if (vertices == 0) {
        return false;
    }

    for (int i = 0; i < edges; i++) {
        if (!unite(arrayOfEdges[i].first, arrayOfEdges[i].second, dsu)) {
            arrayOfEdges[i].first = -1;
        }
    }

    for (int i = 0; i < vertices; i++) {
        find(i, dsu);
        if (dsu[i] != 0) {
            return false;
        }
    }

    return true;
}

ExitCodes fillArray(int vertices, int edges, Edge* arrayOfEdges) {
    for (int i = 0; i < edges; i++) {
        int firstVertice, secondVertice;
        long long length;
        if (scanf("%d%d%lli", &firstVertice, &secondVertice, &length) < 3) {
            return BAD_INPUT;
        }

        if ((firstVertice < 1 || firstVertice > vertices) || (secondVertice < 1 || secondVertice > vertices)) {
            return BAD_VERTEX;
        }

        if (length < 0 || length > MAX_LENGTH) {
            return BAD_LENGTH;
        }

        arrayOfEdges[i] = (Edge){firstVertice, secondVertice, (int)length};
    }

    return SUCCESS;
}

int* createDSU(int vertices) {
    int* dsu = (int*)calloc((size_t)vertices + 1, sizeof(int));
    if (!dsu) {
        return NULL;
    }

    for (int i = 0; i < vertices; i++) {
        dsu[i] = i;
    }

    return dsu;
}

int compareLengths(const void* first, const void* second) {
    const Edge* firstEdge = (Edge*)first;
    const Edge* secondEdge = (Edge*)second;

    return firstEdge -> length - secondEdge -> length;
}

void freeMemory(Edge* arrEdges, int* dsu) {
    if (arrEdges) {
        free(arrEdges);
    }
    if (dsu) {
        free(dsu);
    }
}

ExitCodes KruskalAlgorithm() {
    int vertices, edges;
    Edge* arrayOfEdges;

    ExitCodes getInput;
    if ((getInput = getInputAndCheck(&vertices, &edges)) != SUCCESS) {
        return getInput;
    }

    arrayOfEdges = createArray(edges);
    if (!arrayOfEdges) {
        return OUT_OF_MEMORY;
    }

    ExitCodes fillingArray;
    if ((fillingArray = fillArray(vertices, edges, arrayOfEdges)) != SUCCESS) {
        freeMemory(arrayOfEdges, NULL);
        return fillingArray;
    }

    qsort(arrayOfEdges, (size_t)edges, sizeof(Edge), compareLengths);

    int* dsu;
    if ((dsu = createDSU(vertices)) == NULL) {
        freeMemory(arrayOfEdges, NULL);
        return OUT_OF_MEMORY;
    }

    if (!checkSpanTree(vertices, edges, arrayOfEdges, dsu)) {
        freeMemory(arrayOfEdges, dsu);
        return NO_SPAN_TREE;
    }

    int counterOfEdges = 0;
    for (int i = 0; i < edges; i++) {
        if (arrayOfEdges[i].first > 0) {
            printf("%d %d\n", arrayOfEdges[i].first, arrayOfEdges[i].second);
            counterOfEdges++;
            if (counterOfEdges == vertices - 1) {
                break;
            }
        }
    }

    freeMemory(arrayOfEdges, dsu);

    return SUCCESS;
}

int main() {
    ExitCodes completingAlgorithm;
    if ((completingAlgorithm = KruskalAlgorithm()) != SUCCESS) {
        printf("%s", exitMessages[completingAlgorithm]);
    }

    return completingAlgorithm;
}
