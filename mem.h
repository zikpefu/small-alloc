/* mem.h
 * Lab4: Dynamic Memory Allocation
 * Version: 1
 * ECE 2230, Fall 2018
 */

#define PAGESIZE 4096
#define FIRST_FIT 0xFFFF
#define BEST_FIT  0xBBBB
#define TRUE 1
#define FALSE 0

/* must be FIRST_FIT or BEST_FIT */
int SearchPolicy;

/* TRUE if memory returned to free list is coalesced */
int Coalescing;

/* deallocates the space pointed to by return_ptr; it does nothing if
 * return_ptr is NULL.
 */
void Mem_free(void *return_ptr);

/* returns a pointer to space for an object of size nbytes, or NULL if the
 * request cannot be satisfied.  The space is uninitialized.
 */
void *Mem_alloc(const int nbytes);

/* prints stats about the current free list
 *
 * number of items in the linked list
 * min, max, and average size of each item (bytes)
 * total memory in list (bytes)
 * number of calls to sbrk and number of pages requested
 */
void Mem_stats(void);

/* print table of memory in free list
 * example format
 *     chunk_t *p;
 *     printf("p=%p, size=%d, end=%p, next=%p\n",
 *              p, p->size, p + p->size, p->next);
 */
void Mem_print(void);
/* an example of a chunk_t definition.  You must modify this to
 * match your design
 *
 * We don't really need the definition of chunk_t in mem.h.  However,
 * for debugging it is nice to be able to print the size of chunk_t
 * in the drivers.
 *
 */
typedef struct chunk_tag {
    struct chunk_tag *next;
    int size;
} chunk_t;

/* vi:set ts=8 sts=4 sw=4 et: */
