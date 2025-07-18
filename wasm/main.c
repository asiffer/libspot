#include "../dist/spot.h"

// Define NULL pointer
#define NULL ((void *)0x0)

// Structure representing a memory block in the heap
typedef struct block {
    __SIZE_TYPE__ size; // Size of the memory block
    struct block *next; // Pointer to the next block in the list
    int free;           // Flag indicating if the block is free (1) or used (0)
} block_t;

// Size of the block metadata
#define BLOCK_SIZE sizeof(block_t)

// WebAssembly heap base symbol
extern char __heap_base;
// Pointer to the current end of the heap
static char *heap_end = &__heap_base;

// Returns the size of the Spot structure
__SIZE_TYPE__ spot_size(void) { return sizeof(struct Spot); }

// Simple sbrk implementation to extend the heap
void *sbrk(int increment) {
    char *prev = heap_end;
    heap_end += increment;
    return (void *)prev;
}

// Head of the linked list of memory blocks
static block_t *head = NULL;

// Simple malloc implementation using a linked list of blocks
void *malloc(unsigned long size) {
    block_t *curr = head;
    block_t *prev = NULL;

    // Align requested size to the word boundary
    size = (size + sizeof(void *) - 1) & ~(sizeof(void *) - 1);

    // First-fit search for a free block
    while (curr) {
        if (curr->free && curr->size >= size) {
            curr->free = 0;
            return (
                void *)(curr +
                        1); // Return pointer to memory after block metadata
        }
        prev = curr;
        curr = curr->next;
    }

    // No suitable block found, allocate a new one using sbrk
    block_t *new_block = (block_t *)sbrk(BLOCK_SIZE + size);
    if (new_block == (void *)-1)
        return NULL;

    new_block->size = size;
    new_block->free = 0;
    new_block->next = NULL;

    if (prev)
        prev->next = new_block;
    else
        head = new_block;

    // Return pointer to memory after block metadata
    return (void *)(new_block + 1);
}

// Simple free implementation: marks the block as free
void free(void *ptr) {
    if (!ptr)
        return;

    block_t *block =
        (block_t *)ptr - 1; // Get block metadata from user pointer
    block->free = 1;
}

// Initializes libspot with the defined malloc and free functions
void libspot_init(void) { set_allocators(malloc, free); }
