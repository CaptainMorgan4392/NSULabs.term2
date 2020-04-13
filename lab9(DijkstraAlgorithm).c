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
} ExitCodes;

const char* exitMessages[] = {
        "success",
        "out of memory",
        "bad number of vertices",
        "bad number of edges",
        "bad vertex",
        "bad length",
        "bad number of lines",
};

typedef struct _context Context;

struct _context {
    int vertices;
    int edges;
    int start;
    int destination;
};

void freeMem(Context *ctx, int** g, ll* distances, int* parents, bool* used) {
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

    if (distances) {
        free(distances);
    }

    if (parents) {
        free(parents);
    }

    if (used) {
        free(used);
    }
}

ExitCodes getInputAndCheck(Context* ctx) {
    int vertices, edges, start, destination;
    if (scanf("%d%d%d%d", &vertices, &start, &destination, &edges) < 4) {
        return BAD_INPUT;
    }

    if (vertices < 0 || vertices > 5000) {
        return BAD_NUMBER_VERTICES;
    }

    if (start < 1 || destination < 1 || start > vertices || destination > vertices) {
        return BAD_VERTEX;
    }

    if (edges < 0 || edges > vertices * (vertices + 1) / 2) {
        return BAD_NUMBER_EDGES;
    }

    ctx -> vertices = vertices;
    ctx -> start = start;
    ctx -> destination = destination;
    ctx -> edges = edges;

    return SUCCESS;
}

ExitCodes fillGraph(Context* ctx, int** g) {
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
    }

    return SUCCESS;
}

ExitCodes printOutput(Context* ctx, ll* distances, const int* parents, bool manyPaths) {
    for (int i = 0; i < ctx -> vertices; i++) {
        if (distances[i] <= INT_MAX) {
            printf("%lli ", distances[i]);
        } else if (distances[i] < LLONG_MAX) {
            printf("INT_MAX+ ");
        } else {
            printf("oo ");
        }
    }
    printf("\n");

    if (manyPaths) {
        printf("overflow");
    } else if (distances[ctx -> destination - 1] == LLONG_MAX) {
        printf("no path");
    } else {
        int cur = ctx -> destination - 1;
        printf("%d ", cur + 1);
        do {
            cur = parents[cur];
            printf("%d ", cur + 1);
        } while (cur != parents[cur]);
    }

    return SUCCESS;
}

ExitCodes DijkstraAlgo(Context* ctx, int** g) {
    ll* distances = (ll*)calloc((size_t)ctx -> vertices, sizeof(ll));
    if (!distances) {
        freeMem(ctx, g, NULL, NULL, NULL);
        return OUT_OF_MEMORY;
    }
    int* parents = (int*)calloc((size_t)ctx -> vertices, sizeof(int));
    if (!parents) {
        freeMem(ctx, g, distances, NULL, NULL);
        return OUT_OF_MEMORY;
    }

    bool* used = (bool*)calloc((size_t)ctx -> vertices, sizeof(bool));
    if (!used) {
        freeMem(ctx, g, distances, parents, NULL);
        return OUT_OF_MEMORY;
    }

    for (int i = 0; i < ctx -> vertices; i++) {
        if (i != ctx -> start - 1) {
            distances[i] = LLONG_MAX;
        }
        parents[i] = i;
        used[i] = false;
    }
    distances[ctx -> start - 1] = 0;

    bool manyPaths = false;
    int cur = ctx -> start - 1;
    for (int i = 0; i < ctx -> vertices; i++) {
        used[cur] = true;

        for (int j = 0; j < ctx -> vertices; j++) {
            if (distances[j] >= distances[cur] + (ll)g[cur][j] && g[cur][j] != 0 && !used[j]) {
                if (distances[j] > INT_MAX && distances[j] != LLONG_MAX && j == ctx -> destination - 1) {
                    manyPaths = true;
                }

                distances[j] = distances[cur] + (ll)g[cur][j];
                parents[j] = cur;
            }
        }

        int index = 0;
        ll min = LLONG_MAX;
        for (int j = 0; j < ctx -> vertices; j++) {
            if (distances[j] < min && !used[j]) {
                min = distances[j];
                index = j;
            }
        }

        cur = index;
    }

    ExitCodes print;

    if ((print = printOutput(ctx, distances, parents, manyPaths)) != SUCCESS) {
        freeMem(ctx, g, distances, parents, used);
        return print;
    }

    freeMem(ctx, g, distances, parents, used);

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

    if ((currentAction = fillGraph(ctx, g)) != SUCCESS) {
        freeMem(ctx, g, NULL, NULL, NULL);
        return currentAction;
    }

    if ((currentAction = DijkstraAlgo(ctx, g)) != SUCCESS) {
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
