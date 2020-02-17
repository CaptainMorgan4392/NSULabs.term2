#define boolean int
#define false 0
#define true 1

#include <stdio.h>
#include <limits.h>
#include <mm_malloc.h>
#include <memory.h>

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
        "ok",
        "no memory",
        "bad number of vertices",
        "bad number of edges",
        "bad vertex",
        "bad length",
        "bad number of lines",
        "no spanning tree"
};

typedef struct _graph Graph;

struct _graph {
    int vertices;
    int edges;
    int** table;
};

typedef struct _dsu DSU;

struct _dsu {
    int* parents;
};

void swap(int* p1, int* p2) {
    int temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

int find(DSU* dsu, int s) {
    if (s == dsu -> parents[s]) {
        return s;
    }

    return dsu -> parents[s] = find(dsu, dsu -> parents[s]);
}

void freeMemory(Graph* graph, DSU* dsu) {
    if (graph) {
        if (graph -> table) {
            for (int i = 0; i < graph -> edges; i++) {
                if (graph -> table[i]) {
                    free(graph -> table[i]);
                }
            }
            free(graph -> table);
        }
        free(graph);
    }

    if (dsu) {
        if (dsu -> parents) {
            free(dsu -> parents);
        }
        free(dsu);
    }
}

boolean unite(DSU* dsu, int s1, int s2) {
    s1 = find(dsu, s1);
    s2 = find(dsu, s2);
    if (s1 != s2) {
        if (s1 > s2) {
            swap(&s1, &s2);
        }

        dsu -> parents[s2] = s1;
        return true;
    } else {
        return false;
    }
}

boolean inRange(int val, int lb, int ub) {
    if (lb <= val && val <= ub) {
        return true;
    }

    return false;
}

void sortTable(int** table, int nSt, int nEnd) {
    if (nSt >= nEnd) {
        return;
    }
    int l = nSt, r = nEnd;
    int index = (l + r) / 2;
    int x = table[index][2];
    while (l <= r) {
        while (table[l][2] < x) l++;
        while (table[r][2] > x) r--;

        if (l >= r) break;
        swap(&table[l][0], &table[r][0]);
        swap(&table[l][1], &table[r][1]);
        swap(&table[l++][2], &table[r--][2]);
    }
    sortTable(table, l, index);
    sortTable(table, index + 1, r);
}

ExitCodes fillTable(Graph* graph) {
    int verticeFirst, verticeSecond;
    long long length;

    for (int i = 0; i < graph -> edges; i++) {
        if (scanf("%d%d%lli", &verticeFirst, &verticeSecond, &length) < 3) {
            return BAD_INPUT;
        }

        if (!inRange(verticeFirst, 1, graph -> vertices) || !inRange(verticeSecond, 1, graph -> vertices)) {
            return BAD_VERTEX;
        }

        if (length < 0 || length > MAX_LENGTH) {
            return BAD_LENGTH;
        }

        graph -> table[i][0] = verticeFirst;
        graph -> table[i][1] = verticeSecond;
        graph -> table[i][2] = (int)length;
    }

    return SUCCESS;
}

ExitCodes KruskalAlgorithm() {
    Graph* graph = (Graph*)calloc(1, sizeof(Graph));
    if (!graph) {
        freeMemory(graph, NULL);
        return OUT_OF_MEMORY;
    }

    if (scanf("%d", &graph -> vertices) == 0) {
        freeMemory(graph, NULL);
        return BAD_INPUT;
    } else if (!inRange(graph -> vertices, 0, MAX_VERTICES)) {
        freeMemory(graph, NULL);
        return BAD_NUMBER_VERTICES;
    }

    if (scanf("%d", &graph -> edges) == 0) {
        freeMemory(graph, NULL);
        return BAD_INPUT;
    } else if (!inRange(graph -> edges, 0, graph -> vertices * (graph -> vertices + 1) / 2)) {
        freeMemory(graph, NULL);
        return BAD_NUMBER_EDGES;
    }


    graph -> table = (int**)calloc((size_t)graph -> edges, sizeof(int*));
    if (!graph -> table) {
        freeMemory(graph, NULL);
        return OUT_OF_MEMORY;
    }

    for (int i = 0; i < graph -> edges; i++) {
        graph -> table[i] = (int*)calloc(3, sizeof(int));
        if (!graph -> table[i]) {
            freeMemory(graph, NULL);
            return OUT_OF_MEMORY;
        }
    }

    ExitCodes fillingTable;
    if ((fillingTable = fillTable(graph)) != SUCCESS) {
        freeMemory(graph, NULL);
        return fillingTable;
    }

    sortTable(graph -> table, 0, graph -> edges - 1);

    DSU* dsu = (DSU*)calloc(1, sizeof(DSU));
    if (!dsu) {
        freeMemory(graph, dsu);
        return OUT_OF_MEMORY;
    }
    dsu -> parents = (int*)calloc((size_t)graph -> vertices, sizeof(int));
    if (!dsu -> parents) {
        freeMemory(graph, dsu);
        return OUT_OF_MEMORY;
    }

    for (int i = 0; i < graph -> vertices; i++) {
        dsu -> parents[i] = i;
    }

    for (int i = 0; i < graph -> edges; i++) {
        if (!unite(dsu, graph -> table[i][0] - 1, graph -> table[i][1] - 1)) {
            graph -> table[i][0] = -1;
        }
    }

    if (graph -> vertices == 0) {
        freeMemory(graph, dsu);
        return NO_SPAN_TREE;
    }

    if (graph -> vertices != 1) {
        for (int i = 0; i < graph -> vertices; i++) {
            find(dsu, i);
            if (dsu -> parents[i] != 0) {
                freeMemory(graph, dsu);
                return NO_SPAN_TREE;
            }
        }
    }

    int counterOfEdges = 0;
    for (int i = 0; i < graph -> edges; i++) {
        if (graph -> table[i][0] > 0) {
            printf("%d %d\n", graph -> table[i][0], graph -> table[i][1]);
        }

        if (counterOfEdges == graph -> vertices - 1) {
            break;
        }
    }

    freeMemory(graph, dsu);

    return SUCCESS;
}

int main() {
    ExitCodes completingAlgorithm;
    if ((completingAlgorithm = KruskalAlgorithm()) != SUCCESS) {
        printf("%s", exitMessages[completingAlgorithm]);
    }

    return completingAlgorithm;
}
