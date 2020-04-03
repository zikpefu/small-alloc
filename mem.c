/* mem.c A template
 * Zachary Ikpefua
 * C16803580
 * Lab4: Dynamic Memory Allocation
 * ECE 2230, Fall 2018
 */
#define PAGESIZE 4096
#define MAX 1000000
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "mem.h"

// Global variables required in mem.c only
static chunk_t Dummy = {&Dummy, 0};
static chunk_t * Rover = &Dummy;
//static chunk_t * Follower = &Dummy;
static int NumSbrkCalls;
static int num_pages;
// private function prototypes
void mem_validate(void);

/* function to request 1 or more pages from the operating system.
 *
 * new_bytes must be the number of bytes that are being requested from
 *           the OS with the sbrk command.  It must be an integer
 *           multiple of the PAGESIZE
 *
 * returns a pointer to the new memory location.  If the request for
 * new memory fails this function simply returns NULL, and assumes some
 * calling function will handle the error condition.  Since the error
 * condition is catastrophic, nothing can be done but to terminate
 * the program.
 */
chunk_t *morecore(int new_bytes)
{
    char *cp;
    chunk_t *new_p;
    // preconditions
    assert(new_bytes % PAGESIZE == 0 && new_bytes > 0);
    assert(PAGESIZE % sizeof(chunk_t) == 0);
    cp = sbrk(new_bytes);
    if (cp == (char *) -1)  /* no space available */
        return NULL;
    new_p = (chunk_t *) cp;
     NumSbrkCalls++;
     num_pages += new_bytes/ PAGESIZE;
    return new_p;
}

/*
Function: Mem_free
Inputs: q or p
Outputs: none
Purpose:
deallocates the space pointed to by return_ptr; it does nothing if
  return_ptr is NULL.

  This function assumes that the Rover pointer has already been
  initialized and points to some memory block in the free list.
  */
void Mem_free(void *return_ptr)
{
    if(return_ptr != NULL){
    assert(Rover != NULL && Rover->next != NULL);
    chunk_t* p = NULL;
    chunk_t* Follower = Rover;
    if(Coalescing == 0){
      p = (chunk_t*)return_ptr;
      p--;
      p->next = Rover->next;
      Rover->next = p;
    }
    else {
      //Step 1 place p inbetween leader and follower
      p = (chunk_t *)return_ptr;
      p--;
      Follower = Rover;
      Rover = Rover->next;
      assert(Rover == Follower->next);
      chunk_t*start = Rover;

      do {
        if(Rover > p && p >  Follower){
          p->next = Rover;
          Follower->next = p;
          break;
        }
        if(Follower > Rover && (p < Rover || p > Follower)){
          Follower->next = p;
          p->next = Rover;
          break;
        }
        Rover = Rover->next;
        Follower = Follower->next;
      } while(Rover != start);
      /*
      //If Rovoer and Follower are not at the dummy and you dont find a correct spot
      if(Rover->next != Follower && found != 1){
        printf("Hey!! there is something wrong!!\n");
        assert(0);
      }
      */
        if(p + p->size == Rover && Follower + Follower->size == p){
          //Case where p, Follower, Rover are next to each other
          Follower->size += Rover->size + p->size;
          p->size = -98765;
          p->next = NULL;
          Follower->next = Rover->next;
          Rover->next = NULL;
          Rover->size = -87654;
          Rover = Follower->next;
          p = Follower;
          Follower = Follower->next;
        }
        else if(Follower + Follower->size == p && p + p->size != Rover){
          //Where p and follower are next to each other
          Follower->size += p->size;
          Follower->next = Rover;
          p->next = NULL;
          p->size = -76543;
          Rover = Rover->next;
        }
        else if(Follower + Follower->size != p && p + p->size == Rover){
          //Case where p and rover are next to each other
          Follower->next = p;
          p->size += Rover->size;
          p->next = Rover->next;
          Rover->next = NULL;
          Rover->size = -12345678;
          Rover = p->next;
          Follower = p;
        }
        else{
          p->next = Rover;
          Follower->next = p;
      }
        }
    }
    }
/*
Function: Mem_alloc
Inputs: nbytes, size [B] that user wants to declare
Output: pointer to data (q)
Purpose: returns a pointer to space for an object of size nbytes, or NULL if the
  request cannot be satisfied.  The memory is uninitialized.

  This function assumes that there is a Rover pointer that points to
  some item in the free list.  The first time the function is called,
  Rover is null, and must be initialized with a dummy block whose size
  is one, but set the size field to zero so this block can never be
  removed from the list.  After the first call, the Rover can never be null
  again.
 */
void *Mem_alloc(const int nbytes)
{
    // precondition
    assert(nbytes > 0);
    assert(Rover != NULL && Rover->next != NULL);
    chunk_t *p, *q, *Follower;
    int new_bytes = 0;
    int nunits = 0;
    if(nbytes  %  sizeof(chunk_t) == 0){
       nunits = nbytes / sizeof(chunk_t) + 1;
    }
    else{
       nunits = nbytes / sizeof(chunk_t) + 2;
    }
    assert((nunits - 1) * sizeof(chunk_t) >= nbytes);
    assert((nunits - 1) * sizeof(chunk_t) < nbytes + sizeof(chunk_t));
    int addPages = 0;
    int FoundSpot = 0;
    Follower = Rover;
    Rover = Rover->next;
    chunk_t*start = Rover;
    if(SearchPolicy == FIRST_FIT){
      //First Fit Search Policy
    do {
          if(Rover->size >= nunits && Rover->size != 0){
          FoundSpot = 1;
          break; //found canidate
        }
        Rover = Rover->next;
        Follower = Follower->next;
      } while(Rover != start);
    }
    else{
      //Best fit
      int Canidate = MAX;
      chunk_t *bestchunk = Rover;
      chunk_t *bestfollower = Follower;
      do {
        if(Rover->size >= nunits){
          if(Rover->size == nunits){
          FoundSpot = 1;
          break;
        }
          if(Rover->size < Canidate && nunits < Rover->size){
            FoundSpot = 1;
            Canidate = Rover->size;
            bestchunk = Rover;
            bestfollower = Follower;
        }
      }
        Rover = Rover->next;
        Follower = Follower->next;
      } while(Rover != start);
      if(Rover->size != nunits){
      Rover = bestchunk;
      Follower = bestfollower;
    }
  }
      //Make sure follower is behind rover
      assert(Rover == Follower->next);
      if(FoundSpot != 0){
        //FoundSpot function
        if(Rover->size == nunits){
          chunk_t* temp = NULL;
          temp = Rover;
          Follower->next = Rover->next;
          Rover = Follower;
          temp->next = NULL;
          p = temp;
        }
        else{
        p = Rover;
      }
      }
      else{
          if(nbytes >= PAGESIZE){
            //Pages required is greater, add additional page
            //just to be safe
            addPages = nbytes / PAGESIZE + 2;
          }
          else{
            addPages = (nbytes / PAGESIZE) + 1;
          }
          new_bytes = addPages * PAGESIZE;
          p = morecore(new_bytes);
          p->next = NULL;
          p->size = new_bytes / sizeof(chunk_t);//size in bytes
          q = p + 1;
          Mem_free(q);
          //Call mem alloc to prevent loss of blocks
          return Mem_alloc(nbytes);
        }
        if(p->size < 0){
          q = Rover + Rover->size -nunits;
          q->next = NULL;
          q->size = nunits;
          Rover->size -=nunits;
          assert(q == Rover+ Rover->size);
        }
        else{
        q = p + p->size - nunits;
        q->next = NULL;
        q->size = nunits;
      if(p->size != nunits){
        p->size -= nunits;
        assert(q == p + p->size);
      }
      else{
        p = NULL;
      }
    }
    assert((q->size - 1)*sizeof(chunk_t) >= nbytes);
    assert((q->size - 1)*sizeof(chunk_t) < nbytes + sizeof(chunk_t));
    assert(q->next == NULL);
    return q + 1;
}
/*
Function Mem Stats
Inputs/Outputs: Nonne
Purpose: prints stats about the current free list
*
 * -- number of items in the linked list including dummy item
 * -- min, max, and average size of each item (in bytes)
 * -- total memory in list (in bytes)
 * -- number of calls to sbrk and number of pages requested
 *
 * A message is printed if all the memory is in the free list
 */
void Mem_stats(void)
{
    chunk_t *p = Rover;
    chunk_t *start = p;
    int M = 0;
    int numItems = 0;
    int min = MAX;
    int max = 0;
    do {
        M += p->size * sizeof(chunk_t);
        if(p->size < min && p->size != 0){
          min = p->size;
        }
        if(p->size > max && p->size != 0){
          max = p->size;
        }
        numItems++;
        p = p->next;
    } while (p != start);
    max *= sizeof(chunk_t);
    min *= sizeof(chunk_t);

    printf("There are %d total items in the list\n",numItems - 1);
    printf("The Min size is: %d B\nThe Max size is: %d B\nThe Average size is: %d B\n",min ,max, M / numItems);
    printf("Total memory now: %d B\n",M);
    printf("Calls to sbrk: %d\t", NumSbrkCalls);
    printf("Pages requested/created: %d\n",num_pages);
    if(M == num_pages * PAGESIZE){
    printf("all memory is in the heap -- no leaks are possible\n");
  }
  else{
    printf("The M %d does not match up with %d, there are memory leaks.\n",M,num_pages * PAGESIZE);
  }
}

/* print table of memory in free list
 *
 * The print should include the dummy item in the list
 */
void Mem_print(void)
{
    assert(Rover != NULL && Rover->next != NULL);
    chunk_t *p = Rover;
    chunk_t *start = p;
    //int csize = sizeof(chunk_t);
    do {
        printf("p=%p, size=%d, end=%p, next=%p %s\n",
                p, p->size, p + p->size, p->next, p->size!=0?"":"<-- dummy");
        p = p->next;
    } while (p != start);
    //mem_validate();
}

/* This is an experimental function to attempt to validate the free
 * list when coalescing is used.  It is not clear that these tests
 * will be appropriate for all designs.  If your design utilizes a different
 * approach, that is fine.  You do not need to use this function and you
 * are not required to write your own validate function.
 */
void mem_validate(void)
{
    assert(Rover != NULL && Rover->next != NULL);
    assert(Rover->size >= 0);
    int wrapped = FALSE;
    int found_dummy = FALSE;
    int found_rover = FALSE;
    chunk_t *p, *largest, *smallest;

    // for validate begin at Dummy
    p = &Dummy;
    do {
        if (p->size == 0) {
            assert(found_dummy == FALSE);
            found_dummy = TRUE;
        } else {
            assert(p->size > 0);
        }
        if (p == Rover) {
            assert(found_rover == FALSE);
            found_rover = TRUE;
        }
        p = p->next;
    } while (p != &Dummy);
    assert(found_dummy == TRUE);
    assert(found_rover == TRUE);

    if (Coalescing) {
        do {
            if (p >= p->next) {
                // this is not good unless at the one wrap
                if (wrapped == TRUE) {
                    printf("validate: List is out of order, already found wrap\n");
                    printf("first largest %p, smallest %p\n", largest, smallest);
                    printf("second largest %p, smallest %p\n", p, p->next);
                    assert(0);   // stop and use gdb
                } else {
                    wrapped = TRUE;
                    largest = p;
                    smallest = p->next;
                }
            } else {
                assert(p + p->size < p->next);
            }
            p = p->next;
        } while (p != &Dummy);
        assert(wrapped == TRUE);
    }
}
/* vi:set ts=8 sts=4 sw=4 et: */
