/*
 * Interview exercise: a custom heap allocator. (SOLUTION)
 *
 * Build:   cc -Wall -Wextra -O0 -g alloc_exercise_solution.c -o alloc_exercise_solution
 * Run:     ./alloc_exercise_solution
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* ------------------------------------------------------------------ */
/* Allocator                                                          */
/* ------------------------------------------------------------------ */

#define HEAP_BYTES 1024
#define ALIGN_TO   16

static uint8_t pool[HEAP_BYTES] __attribute__((aligned(ALIGN_TO)));

typedef struct blk {
    size_t   nbytes;   /* payload only, not including this struct */
    uint32_t in_use;
} blk_t;

#define BLK_HDR   (sizeof(blk_t))
#define MIN_SPLIT (BLK_HDR + ALIGN_TO)

static int pool_ready = 0;

static void pool_setup(void) {
    blk_t *root = (blk_t *)pool;
    root->nbytes = HEAP_BYTES - BLK_HDR;
    root->in_use = 0;
    pool_ready = 1;
}

static size_t align_up(size_t len) {
    return (len + ALIGN_TO - 1) & ~(size_t)(ALIGN_TO - 1);
}

void *mem_alloc(size_t len) {
    if (!pool_ready) pool_setup();
    if (len == 0) return NULL;

    /* Round the request up so the block that follows also lands on an
     * aligned boundary. BLK_HDR is a multiple of ALIGN_TO, and the pool
     * base is aligned, so this is sufficient. */
    len = align_up(len);

    uint8_t *cursor = pool;
    while (cursor < pool + HEAP_BYTES) {
        blk_t *block = (blk_t *)cursor;
        if (!block->in_use && block->nbytes >= len) {
            /* Split off the remainder only if it can hold a header
             * plus at least one aligned allocation. */
            if (block->nbytes >= len + MIN_SPLIT) {
                blk_t *new_block = (blk_t *)(cursor + BLK_HDR + len);
                new_block->nbytes = block->nbytes - len - BLK_HDR;
                new_block->in_use = 0;
                block->nbytes = len;
            }
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

    /* Merge with the next block if it exists and is free. */
    uint8_t *next = (uint8_t *)block + BLK_HDR + block->nbytes;
    if (next < pool + HEAP_BYTES) {
        blk_t *next_block = (blk_t *)next;
        if (!next_block->in_use) {
            block->nbytes += BLK_HDR + next_block->nbytes;
        }
    }

    /* Merge with the previous block if it exists and is free. Implicit
     * layout means we walk from the start to find it. */
    if ((uint8_t *)block != pool) {
        uint8_t *cursor = pool;
        while (cursor < (uint8_t *)block) {
            blk_t *prev_block = (blk_t *)cursor;
            uint8_t *step = cursor + BLK_HDR + prev_block->nbytes;
            if (step == (uint8_t *)block) {
                if (!prev_block->in_use) {
                    prev_block->nbytes += BLK_HDR + block->nbytes;
                }
                break;
            }
            cursor = step;
        }
    }
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
