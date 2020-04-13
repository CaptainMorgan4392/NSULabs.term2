#include <stdio.h>
#include <mm_malloc.h>
#include <stdbool.h>
#include <limits.h>

#define ll long long

typedef enum {
    SUCCESS,
    OUT_OF_MEMORY,
    BAD_NUMBER_VERTICES,
    BAD_NUMBER_EDGES,
    BAD_VERTEX,
    BAD_LENGTH,
    BAD_INPUT,
    NO_SPANNING_TREE
} ExitCodes;

const char* exitMessages[] = {
        "success",
        "out of memory",
        "bad number of vertices",
        "bad number of edges",
        "bad vertex",
        "bad length",
        "bad number of lines",
        "no spanning tree"
};

typedef struct _context Context;

struct _context {
    int vertices;
    int edges;
};

void freeMem(Context *ctx, int** g, bool* hasEdge, ll* pQueue, int* from) {
    if (g) {
        if (ctx) {
            for (int i = 0; i < ctx -> vertices; i++) {
                if (g[i]) {
                    free(g[i]);
                }
            }
        }
        free(g);
    }

    if (ctx) {
        free(ctx);
    }

    if (hasEdge) {
        free(hasEdge);
    }

    if (pQueue) {
        free(pQueue);
    }

    if (from) {
        free(from);
    }
}

ExitCodes getInputAndCheck(Context* ctx) {
    int vertices, edges;
    if (scanf("%d%d", &vertices, &edges) < 2) {
        return BAD_INPUT;
    }

    if (vertices < 0 || vertices > 5000) {
        return BAD_NUMBER_VERTICES;
    }

    if (edges < 0 || edges > vertices * (vertices + 1) / 2) {
        return BAD_NUMBER_EDGES;
    }

    ctx -> vertices = vertices;
    ctx -> edges = edges;

    return SUCCESS;
}

ExitCodes fillGraph(Context* ctx, int** g, bool* hasEdge) {
    for (int i = 0; i < ctx -> edges; i++) {
        int first, second;
        ll length;
        if (scanf("%d%d%lli", &first, &second, &length) < 3) {
            return BAD_INPUT;
        }

        if (first < 1 || second < 1 || first > ctx -> vertices || second > ctx -> vertices) {
            return BAD_VERTEX;
        }

        if (length < 0 || length > INT_MAX) {
            return BAD_LENGTH;
        }

        g[first - 1][second - 1] = (int)length;
        g[second - 1][first - 1] = (int)length;
        hasEdge[first - 1] = true;
        hasEdge[second - 1] = true;
    }

    return SUCCESS;
}

ExitCodes PrimAlgo(Context* ctx, int** g, bool* hasEdge) {
    if (ctx -> vertices == 0) {
        freeMem(ctx, g, hasEdge, NULL, NULL);
        return NO_SPANNING_TREE;
    } else if (ctx -> vertices == 1 && ctx -> edges == 0) {
        freeMem(ctx, g, hasEdge, NULL, NULL);
        return SUCCESS;
    }

    for (int i = 0; i < ctx -> vertices; i++) {
        if (!hasEdge[i]) {
            freeMem(ctx, g, hasEdge, NULL, NULL);
            return NO_SPANNING_TREE;
        }
    }
    ll* pQueue = (ll*)calloc((size_t)ctx -> vertices, sizeof(ll));
    if (!pQueue) {
        freeMem(ctx, g, hasEdge, NULL, NULL);
        return OUT_OF_MEMORY;
    }
    for (int i = 1; i < ctx -> vertices; i++) {
        pQueue[i] = (ll)INT_MAX + 1;
    }

    int* from = (int*)calloc((size_t)ctx -> vertices, sizeof(int));
    if (!from) {
        freeMem(ctx, g, hasEdge, pQueue, NULL);
        return OUT_OF_MEMORY;
    }

    int cur = 0;
    for (int i = 0; i < ctx -> vertices - 1; i++) {
        for (int j = 0; j < ctx -> vertices; j++) {
            if (g[cur][j] < pQueue[j] && g[cur][j] != 0 && pQueue[j] != LLONG_MAX) {
                pQueue[j] = g[cur][j];
                from[j] = cur;
            }
        }

        int index = -1;
        pQueue[cur] = LLONG_MAX;
        ll min = pQueue[0];
        for (int j = 1; j < ctx -> vertices; j++) {
            if (pQueue[j] < min) {
                min = pQueue[j];
                index = j;
            }
        }

        cur = index;
        printf("%d %d\n", from[index] + 1, index + 1);
    }

    freeMem(ctx, g, hasEdge, pQueue, from);

    return SUCCESS;
}

ExitCodes start() {
    Context* ctx = (Context*)calloc(1, sizeof(Context));
    if (!ctx) {
        return OUT_OF_MEMORY;
    }

    ExitCodes currentAction;

    if ((currentAction = getInputAndCheck(ctx)) != SUCCESS) {
        freeMem(ctx, NULL, NULL, NULL, NULL);
        return currentAction;
    }

    int** g = (int**)calloc((size_t)ctx -> vertices, sizeof(int*));
    if (!g) {
        freeMem(ctx, NULL, NULL, NULL, NULL);
        return OUT_OF_MEMORY;
    }
    for (int i = 0; i < ctx -> vertices; i++) {
        g[i] = (int*)calloc((size_t)ctx -> vertices, sizeof(int));
        if (!g[i]) {
            freeMem(ctx, g, NULL, NULL, NULL);
            return OUT_OF_MEMORY;
        }
    }

    bool* hasEdge = (bool*)calloc((size_t)ctx -> vertices, sizeof(bool));
    if (!hasEdge) {
        freeMem(ctx, g, NULL, NULL, NULL);
        return OUT_OF_MEMORY;
    }


    if ((currentAction = fillGraph(ctx, g, hasEdge)) != SUCCESS) {
        freeMem(ctx, g, hasEdge, NULL, NULL);
        return currentAction;
    }

    if ((currentAction = PrimAlgo(ctx, g, hasEdge)) != SUCCESS) {
        return currentAction;
    }

    return SUCCESS;
}

int main() {
    ExitCodes exec;
    if ((exec = start()) != SUCCESS) {
        printf("%s", exitMessages[exec]);
    }

    return exec;
}
