#define boolean int
#define false 0
#define true 1

#include <stdio.h>
#include <mm_malloc.h>
#include <memory.h>
#include <limits.h>

#define ZERO 0
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
    NO_SPANNING_TREE
} ReturnCodes;

const char* exitMessages[] = {
        "ok",
        "out of memory",
        "bad number of vertices",
        "bad number of edges",
        "bad vertex",
        "bad length",
        "bad number of lines",
        "no spanning tree"
};

typedef struct _graph Graph;
typedef struct _dsu DSU;

struct _graph {
    int vertices;
    int edges;
    int* verticesFrom;
    int* verticesTo;
    int64_t* lengths;
};

struct _dsu {
    int* ranks;
    int* parents;
};

int get(DSU* dsu, int set) {
    if (set == dsu -> parents[set]) {
        return set;
    }

    return dsu -> parents[set] = get(dsu, dsu -> parents[set]);
}

boolean unite(int setFirst, int setSecond, DSU* dsu) {
    setFirst = get(dsu, setFirst);
    setSecond = get(dsu, setSecond);
    if (setFirst != setSecond) {
        if (setFirst < setSecond) {
            dsu -> parents[setSecond] = setFirst;
            dsu -> ranks[setFirst] += dsu -> ranks[setSecond];
            dsu -> ranks[setSecond] = 0;
        } else {
            dsu -> parents[setFirst] = setSecond;
            dsu -> ranks[setSecond] += dsu -> ranks[setFirst];
            dsu -> ranks[setFirst] = 0;
        }
        return true;
    }
    return false;
}

void swapLong(int64_t *a, int64_t *b) {
    int64_t temp = *a;
    *a = *b;
    *b = temp;
}

void swapInt(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void freeDynamic(Graph* g) {
    if (g) {
        if (g -> verticesFrom) {
            free(g -> verticesFrom);
        }

        if (g -> verticesTo) {
            free(g -> verticesTo);
        }

        if (g -> lengths) {
            free(g -> lengths);
        }

        free(g);
    }
}

boolean inRangeInt(int value, int lb, int ub) {
    if (lb <= value && value <= ub) {
        return true;
    }

    return false;
}

boolean inRangeLong(int64_t value, int64_t lb, int64_t ub) {
    if (lb <= value && value <= ub) {
        return true;
    }

    return false;
}

ReturnCodes createGraph(Graph* g, int vertices, int edges) {
    g -> vertices = vertices;
    g -> edges = edges;

    g -> verticesFrom = (int*)calloc((size_t)vertices, sizeof(int));
    g -> verticesTo = (int*)calloc((size_t)vertices, sizeof(int));
    g -> lengths = (int64_t*)calloc((size_t)vertices, sizeof(int64_t));
    if (!g -> verticesFrom || !g -> verticesTo || !g -> lengths) {
        return OUT_OF_MEMORY;
    }

    for (int i = 0; i < g -> edges; i++) {
        int verticeFrom, verticeTo;
        int64_t length;

        if (scanf("%d%d%lli", &verticeFrom, &verticeTo, &length) < 3) {
            return BAD_INPUT;
        } else {
            if (!inRangeInt(verticeFrom, 1, g -> vertices) || !inRangeInt(verticeTo, 1, g -> vertices)) {
                return BAD_VERTEX;
            } else if (!inRangeLong(length, 0, MAX_LENGTH)) {
                return BAD_LENGTH;
            }
        }

        g -> verticesFrom[i] = verticeFrom;
        g -> verticesTo[i] = verticeTo;
        g -> lengths[i] = length;
    }

    return SUCCESS;
}

int partition(Graph* g, int l, int r) {
    int64_t x = g -> lengths[(l + r) / 2];
    while (l <= r) {
        while (g -> lengths[l] < x) l++;
        while (g -> lengths[r] > x) r--;
        if (l >= r) break;
        swapLong(g -> lengths + l, g -> lengths + r);
        swapInt((g -> verticesFrom + l), (g -> verticesFrom + r));
        swapInt((g -> verticesTo + l++), (g -> verticesTo + r--));
    }

    return r;
}

void sortByLength(Graph* g, int nSt, int nEnd) {
    if (nSt >= nEnd) {
        return;
    }

    int l = nSt, r = nEnd;
    int x = partition(g, l, r);
    sortByLength(g, l, x);
    sortByLength(g, x + 1, r);
}

ReturnCodes startAlgorithm(Graph* g) {
    DSU* dsu = calloc(1, sizeof(DSU));
    if (!dsu) {
        return OUT_OF_MEMORY;
    }

    dsu -> parents = (int*)calloc((size_t)g -> vertices, sizeof(int));
    dsu -> ranks = (int*)calloc((size_t)g -> vertices, sizeof(int));
    if (!dsu -> parents || !dsu -> ranks) {
        return OUT_OF_MEMORY;
    }

    for (int i = 0; i < g -> vertices; i++) {
        dsu -> ranks[i] = 1;
        dsu -> parents[i] = i;
    }

    int** edgesOfTree = (int**)calloc((size_t)g -> vertices - 1, sizeof(int*));
    for (int i = 0; i < g -> vertices - 1; i++) {
        edgesOfTree[i] = (int*)calloc(2, sizeof(int));
    }

    int curIndex = 0;
    for (int i = 0; i < g -> edges; i++) {
        if (unite(g -> verticesFrom[i] - 1, g -> verticesTo[i] - 1, dsu)) {
            edgesOfTree[curIndex][0] = g -> verticesFrom[i];
            edgesOfTree[curIndex++][1] = g -> verticesTo[i];
        }
    }

    if (dsu -> ranks[0] != g -> vertices || g -> vertices == 0) {
        return NO_SPANNING_TREE;
    } else for (int i = 0; i < g -> vertices - 1; i++) {
        printf("%d %d\n", edgesOfTree[i][0], edgesOfTree[i][1]);
    }
    return SUCCESS;
}

ReturnCodes KruskalAlgorithm() {
    int numberOfVertices;
    if (scanf("%d", &numberOfVertices) == 0) {
        return BAD_INPUT;
    }

    int numberOfEdges;
    if (scanf("%d", &numberOfEdges) == 0) {
        return BAD_INPUT;
    }

    if (!inRangeInt(numberOfVertices, ZERO, MAX_VERTICES)) {
        return BAD_NUMBER_VERTICES;
    } else {
        int maxEdges = numberOfVertices * (numberOfVertices + 1) / 2;
        if (!inRangeInt(numberOfEdges, ZERO, maxEdges)) {
            return BAD_NUMBER_EDGES;
        }
    }

    Graph* graph = calloc(1, sizeof(Graph));
    if (!graph) {
        return OUT_OF_MEMORY;
    }

    ReturnCodes creatingGraph;
    if ((creatingGraph = createGraph(graph, numberOfVertices, numberOfEdges)) != SUCCESS) {
        return creatingGraph;
    }

    sortByLength(graph, 0, numberOfEdges - 1);

    ReturnCodes runningAlgorithm;
    if ((runningAlgorithm = startAlgorithm(graph)) != SUCCESS) {

        return runningAlgorithm;
    }
    return SUCCESS;
}

int main() {
    ReturnCodes returnCode;
    if ((returnCode = KruskalAlgorithm()) != SUCCESS) {
        printf("%s", exitMessages[returnCode]);
        return returnCode;
    }
    return SUCCESS;
}
