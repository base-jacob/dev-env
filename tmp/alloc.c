/*
 * Interview exercise: a fixed-heap allocator.
 *
 * Build:   cc -Wall -Wextra -O0 -g alloc_exercise.c -o alloc_exercise
 * Run:     ./alloc_exercise
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* ------------------------------------------------------------------ */
/* Allocator                                                          */
/* ------------------------------------------------------------------ */

#define HEAP_BYTES 1024

static uint8_t pool[HEAP_BYTES] __attribute__((aligned(16)));

typedef struct blk {
    size_t   nbytes;   /* payload only, not including this struct */
    uint32_t in_use;
} blk_t;

#define BLK_HDR (sizeof(blk_t))

static int pool_ready = 0;

static void pool_setup(void) {
    blk_t *root = (blk_t *)pool;
    root->nbytes = HEAP_BYTES - BLK_HDR;
    root->in_use = 0;
    pool_ready = 1;
}

void *mem_alloc(size_t len) {
    if (!pool_ready) pool_setup();
    if (len == 0) return NULL;

    uint8_t *cursor = pool;
    while (cursor < pool + HEAP_BYTES) {
        blk_t *block = (blk_t *)cursor;
        if (!block->in_use && block->nbytes >= len) {
            block->in_use = 1;
            return cursor + BLK_HDR;
        }
        cursor += BLK_HDR + block->nbytes;
    }
    return NULL;
}

void mem_free(void *ptr) {
    if (!ptr) return;
    blk_t *block = (blk_t *)((uint8_t *)ptr - BLK_HDR);
    block->in_use = 0;
}

void pool_reset(void) {
    pool_ready = 0;
}

/* ------------------------------------------------------------------ */
/* Tests                                                              */
/* ------------------------------------------------------------------ */

static int test_1(void) {
    pool_reset();

    void *big = mem_alloc(900);
    mem_free(big);

    void *small = mem_alloc(17);
    void *more  = mem_alloc(200);

    (void)small;
    if (more == NULL) {
        printf("test 1: FAIL\n");
        return 1;
    }
    printf("test 1: ok\n");
    return 0;
}

static int test_2(void) {
    pool_reset();

    void *a = mem_alloc(71);
    void *b = mem_alloc(71);
    void *c = mem_alloc(71);
    void *d = mem_alloc(71);

    mem_free(b);
    mem_free(c);

    void *e = mem_alloc(130);

    (void)a; (void)d;
    if (e == NULL) {
        printf("test 2: FAIL\n");
        return 1;
    }
    printf("test 2: ok\n");
    return 0;
}

/*
 * Cycle a handful of blocks through many alloc/free rounds, varying the
 * free order. At the end the pool should be recoverable enough to satisfy
 * a large single allocation.
 */
static int test_3(void) {
    pool_reset();

    for (int i = 0; i < 40; i++) {
        void *x = mem_alloc(40);
        void *y = mem_alloc(56);
        void *z = mem_alloc(24);
        if (!x || !y || !z) { printf("test 3: FAIL\n"); return 1; }

        switch (i % 3) {
            case 0: mem_free(x); mem_free(y); mem_free(z); break;
            case 1: mem_free(z); mem_free(y); mem_free(x); break;
            case 2: mem_free(y); mem_free(x); mem_free(z); break;
        }
    }

    void *huge = mem_alloc(800);
    if (huge == NULL) {
        printf("test 3: FAIL\n");
        return 1;
    }
    printf("test 3: ok\n");
    return 0;
}

int main(void) {
    test_1();
    test_2();
    test_3();
    return 0;
}