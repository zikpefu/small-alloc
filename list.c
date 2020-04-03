/* list.c
 * Zachary Ikpefua
 * zikpefu
 * ECE 2230 Fall 2018
 * MP3
 *
 * Propose: A private-esque template for exporting functions to the other c programs
 *
 * Assumptions: Cannot go past data_t block
                Added [one] new prototype in list.h
 *
 * Bugs: none
 */


#include <stdlib.h>
#include <assert.h>

#include "datatypes.h"   // defines data_t
#include "list.h"        // defines public functions for list ADT

// definitions for private constants used in list.c only
#define SORTED_LIST   0xCACACAC
#define UNSORTED_LIST 0x3535353

// prototypes for private functions used in list.c only
void list_debug_validate(list_t *L);
void insertionSort(list_t *);
void SelectionSort(list_t*, list_node_t*,list_node_t*);
IteratorPtr FindMax(list_t*, list_node_t*, list_node_t*);
void IterativeSelectionSort(list_t*, list_node_t*, list_node_t*);
void MergeSort(list_t*);
void PartitionList(list_t*, list_t*, list_t*);
void Merge(list_t*, list_t *, list_t*,data_t*);
/* ----- below are the functions  ----- */

/* Allocates a new, empty list
 *
 * By convention, the list is initially assumed to be sorted.  The field sorted
 * can only take values SORTED_LIST or UNSORTED_LIST
 *
 * The inital empty list must have
 *  1. current_list_size = 0
 *  2. list_sorted_state = SORTED_LIST
 *
 * Use list_destruct to remove and deallocate all elements on a list
 * and the header block.
 */
list_t *list_construct(int (*fcomp)(const data_t *, const data_t *))
{
        list_t *L;

        L = (list_t *) malloc(sizeof(list_t));
        L->head = NULL;
        L->tail = NULL;
        L->current_list_size = 0;
        L->list_sorted_state = SORTED_LIST;
        L->comp_proc = fcomp;
        // the last line of this function must call validate
        //list_debug_validate(L);
        return L;
}

/* Purpose: return the count of number of elements in the list.
 *
 * list_ptr: pointer to list-of-interest.
 *
 * Return: the number of elements stored in the list.
 */
int list_size(list_t *list_ptr)
{
        assert(NULL != list_ptr);
        assert(list_ptr->current_list_size >= 0);
        return list_ptr->current_list_size;
}

/* Deallocates the contents of the specified list, releasing associated memory
 * resources for other purposes.
 *
 * Free all elements in the list and the header block.
    Input: list_ptr
 */
void list_destruct(list_t *list_ptr)
{
        if(list_ptr == NULL) {
                free(list_ptr);
        }
        else if(list_ptr->head == NULL && list_ptr->tail == NULL) {
                free(list_ptr);
                list_ptr = NULL;
        }
        // the first line must validate the list
        else {
                //list_debug_validate(list_ptr);
                list_node_t *junk = list_ptr->tail;
                list_node_t *rover = list_ptr->tail->prev;
                while(rover != NULL) {
                        junk->next = NULL;
                        free(junk->data_ptr);
                        junk->data_ptr = NULL;
                        junk->prev = NULL;
                        rover->next = NULL;
                        free(junk);
                        junk = rover;
                        rover = rover->prev;
                }
                free(junk->data_ptr);
                junk->data_ptr = NULL;
                free(junk);
                junk = NULL;
                list_ptr->tail = NULL;
                list_ptr->head = NULL;
                list_ptr->current_list_size = 0;
                free(list_ptr);
                list_ptr = NULL;
        }
}

/* Return an Iterator that points to the last list_node_t. If the list is empty
 * then the pointer that is returned is NULL.
    Input: list_ptr
    Output: list_ptr->tail
 */
list_node_t * list_iter_back(list_t *list_ptr)
{
        assert(list_ptr != NULL);
        if(list_ptr->head == NULL && list_ptr->tail == NULL) {
                return NULL;
        }
        //list_debug_validate(list_ptr);
        return list_ptr->tail;
}

/* Return an Iterator that points to the first element in the list.  If the
 * list is empty the value that is returned in NULL.
    Input: list_ptr
    Output: list_ptr->head
 */
list_node_t * list_iter_front(list_t *list_ptr)
{
        assert(list_ptr != NULL);
        if(list_ptr->head == NULL && list_ptr->tail == NULL) {
                return NULL;
        }
        //list_debug_validate(list_ptr);
        return list_ptr->head;
}

/* Advance the Iterator to the next item in the list.
 * If the iterator points to the last item in the list, then
 * this function returns NULL to indicate there is no next item.
 *
 * It is a catastrophic error to call this function if the
 * iterator, idx_ptr, is null.
   Input: Iterator ptr
   Output Next iteration
 */
list_node_t * list_iter_next(list_node_t * idx_ptr)
{
        assert(idx_ptr != NULL);
        return idx_ptr->next;
}

/* Finds an element in a list and returns a pointer to it.
 *
 * list_ptr: pointer to list-of-interest.
 *
 * elem_ptr: element against which other elements in the list are compared.
 *           Note: uses the comp_proc function pointer found in the list_t
 *           header block.
 *
 * The function returns an Iterator pointer to the list_node_t that contains
 * the first matching element if a match if found.  If a match is not found
 * the return value is NULL.
 *
 * Note: to get a pointer to the matching data_t memory block pass the return
 *       value from this function to the list_access function.
 */
list_node_t * list_elem_find(list_t *list_ptr, data_t *elem_ptr)
{
        //  list_debug_validate(list_ptr);
        IteratorPtr rover = list_ptr->head;
        int comp = 0;

        while(rover != NULL) {
                comp = list_ptr->comp_proc(rover->data_ptr, elem_ptr);
                if(comp == 0) {
                        return rover;
                }
                rover = rover->next;
        }
        //element not found
        return NULL;
}
/*
   FUNCTION: list_sort
   INPUTS: wating list, sort type
   OUTPUTS: None
   PUURPOSE: Sorts the list in 4 ways with sort inputs:

   1: Insertion sort.
   We can use the other functions in list.cto implement a very simple sort function.
   To sort the list, use list_removeto take the first (or head) item from the list and list_insert_sortedto put the item into a second list that is sorted.
   When all the elements have been inserted into the sorted list, adjust the pointers in list_ptrto point to the newly sorted list.
   Note this is a simple variation of the priority queue sort described byStandish in Section 4.3 of the book.

    2: Recursive Selection Sort.
   Implement the recursive version of the selection sort as defined by program 5.19 on page 152 in the book by Standish.
    Except, change the code so the sorted list is increasing.
    Update the algorithm so that it properly handles our two-way linked list (as opposed to the implementation for an array in program 5.19).
    The calculations of the sort algorithm should not change; just change the algorithm to work with pointers instead of indexes into an array.
    You will also need to implement Standish’s FindMaxalgorithm defined in program 5.20 on page 152.  The source code from Standish is available on Canvas.
    See also the note at the end of this document.

     3: Iterative Selection Sort.
    Implement the iterative version of the selection sort as defined by program 5.35 on page 171 in the book by Standish.
     Read Section 5.4 for an explanation of how the recursive version of the program is transformed into the iterative version.

     4: Merge Sort.
    Implement the recursive version of merge sort as defined by the program 6.19 on page 237 of the book by Standish.
    Note that you will need to implement two support functions.
    The first is a function to partition a list into two half-lists (this is easy to implement as you just step through the linked list until half the list size and then break the list into two lists).
    This second function merges two lists that are sorted into a single list.
    Read the paragraph on page 237 that discusses how to merge two lists, and note that it is a simple process of using list_removeat
          the head of either the left or right listandlist_insertat the tail of the merged list.
    See also the MergeSort powerpoint file for an illustration of the algorithm in the textbook.

 */
void list_sort(ListPtr list, int sort_type){
        if(list != NULL) {
                if(sort_type > 4 || sort_type < 0) {
                }
                else{
                        if(sort_type == 1) {
                                insertionSort(list);
                        }
                        else if(sort_type == 2) {
                                SelectionSort(list,list->head, list->tail);
                        }
                        else if(sort_type == 3) {
                                IterativeSelectionSort(list,list->head, list->tail);
                        }
                        else{
                                MergeSort(list);
                        }

                }

                list->list_sorted_state = SORTED_LIST;
                list_debug_validate(list);
        }
}

/*
   FUNCTION: insertionSort
   INPUTS: listPtr
   OUTPUTS: NONE
   PURPOSE: This function is a function that goes through each element
            and removes it and calls the insert_sorted function to
            sort the elements, after function is complete it frees sorted
 */
void insertionSort(list_t* unsorted) {
        assert(unsorted != NULL);
        ListPtr sorted = list_construct(unsorted->comp_proc);
        data_t* elem = NULL;
        while (unsorted->head != NULL && unsorted->tail != NULL) {
                elem = list_remove(unsorted, NULL);
                list_insert_sorted(sorted, elem);
        }
        //set head tail ptrs
        unsorted->head = sorted->head;
        unsorted->tail = sorted->tail;
        unsorted->current_list_size = sorted->current_list_size;
        sorted->head = NULL;
        sorted->tail = NULL;
        //get rid of sorted
        free(sorted);
}
/*
   FUNCTION: SelectionSort
   INPUTS: listPtr, list->head, list->tail
   OUTPUTS: NONE
   PURPOSE: This function rovers through each node t block
            and calls the find max function to find the max and use
            that node block to swap the elements
 */
void SelectionSort(ListPtr A, IteratorPtr m, IteratorPtr n)
{
        IteratorPtr MaxNodePos; /* MaxPosition is the index of A's biggest item*/
        data_t* temp = NULL;      /* temp is used to exchange items in A                                */
        if (m != A->tail) {       /* if there is more than one number to sort */
                /* Find Max will return a node ptr to the correct max num */
                MaxNodePos = FindMax(A,m,n);
                /* Exchange data ptrs */
                temp = m->data_ptr;
                m->data_ptr = MaxNodePos->data_ptr;
                MaxNodePos->data_ptr = temp;
                /* SelectionSort the subarray A[m+1:n] */
                SelectionSort(A, m->next, n);
        }
}
/*
   FUNCTION: FindMax
   INPUTS: listPtr, m (rover), n (tail)
   OUTPUTS: j a IteratorPtr to max elem
   PURPOSE: This function will go through a while loop so that
            it will get the max from the entire list. It finds the max by
            calling comp proc which returns a -1 if elem a is < elem b
 */
IteratorPtr FindMax(ListPtr A, IteratorPtr m, IteratorPtr n)   /* assume m<n */
{
        IteratorPtr i = m; /* i is an index that visits all positions from m to n*/
        IteratorPtr j = m; /* j is an index that saves the position of the largest */

        do {
                i = i->next;      /* advance i to point to next number A[i]
                                   */
                //if (list_ptr->comp_proc(rover->data_ptr, rover->next->data_ptr))
                if (A->comp_proc(j->data_ptr, i->data_ptr) == -1) { /* if i largest previous j then*/
                        j = i; /* save the position, i, of the largest in j
                                */
                }
        } while (i != n);   /* stop when all i in m:n have been tested
                             */
        return j;               /* return j == position of the largest
                                 */
}
/*
   FUNCTION: IterativeSelectionSort
   INPUTS: listPtr, m(head ptr), n(tail_ptr)
   OUTPUTS: NONE
   PURPOSE: This function is very similar to the previous SelectionSort function
          as a whole. Bascially instead of calling a loop we rover through the entire
          list and if we find the element we are looking for we set the Maximum node to the rover
          then we swap elements.
 */
void IterativeSelectionSort(ListPtr A, IteratorPtr m, IteratorPtr n)
{
        IteratorPtr MaxNode, i;
        data_t* temp;
        while (m != A->tail) {
                i = m;
                MaxNode = m;

                do {
                        i = i->next;
                        if (A->comp_proc(MaxNode->data_ptr, i->data_ptr) == -1) MaxNode = i;
                } while (i != n);

                temp = m->data_ptr;
                m->data_ptr = MaxNode->data_ptr;
                MaxNode->data_ptr = temp;
                m = m->next;
        }
}
/*
   FUNCTION: MergeSort
   INPUTS: ListPtr (list)
   OUTPUTS: NONE
   PURPOSE: This function will create two separte lists and use these to repeativly
            partion the list by recursivly calling itself until there is only
            one element left then it will combine all the lists together in
            mainlist
 */
void MergeSort(ListPtr mainlist){
        ListPtr L = list_construct(mainlist->comp_proc);
        ListPtr R = list_construct(mainlist->comp_proc);
        data_t* elem = NULL;
        if(mainlist->current_list_size > 1) {
                PartitionList(mainlist, L, R);
                MergeSort(L);
                MergeSort(R);
				        Merge(L, R, mainlist,elem);
        }
        free(L);
        free(R);
}


void Merge(ListPtr L, ListPtr R,ListPtr mainlist,data_t* elem){
	           while((L->head != NULL && L->tail != NULL) || (R->head != NULL && R->tail != NULL)) {
                        if(R->head == NULL && R->tail == NULL) {
                                //remove only left
                                elem = list_remove(L, NULL);
                                list_insert(mainlist,elem, NULL);
                        }
                        else if(L->head == NULL && L->tail == NULL) {
                                //remove only right
                                elem = list_remove(R, NULL);
                                list_insert(mainlist, elem, NULL);
                        }
                        else{
                                if (mainlist->comp_proc(L->head->data_ptr, R->head->data_ptr) == 1) {
                                        elem = list_remove(L, NULL);
                                        list_insert(mainlist, elem, NULL);
                                }
                                else{
                                        elem = list_remove(R, NULL);
                                        list_insert(mainlist, elem, NULL);
                                }
                        }
                }
}
/*
   FUNCTION: PartitionList
   INPUTS: list, L(left), R(right)
   OUTPUTS: NONE
   PURPOSE: This function will split the list by half and set each side equal to
            the left and right ptrs
 */
void PartitionList(ListPtr list, ListPtr L, ListPtr R){
        IteratorPtr rover = list->head;
        L->head = list->head;
        R->tail = list->tail;
        int i = 0;
        int size = list->current_list_size;
        int remain = size - (size / 2);
        while(rover->next != NULL) {
                i++;
                if(i == size / 2) {
                        R->head = rover->next;
                        L->tail = rover;
                        R->head->prev = NULL;
                        L->tail->next = NULL;
                        L->current_list_size = i;
                        R->current_list_size = remain;
                        break;
                }
                rover = rover->next;

        }
        list->current_list_size = 0;
        list->head = NULL;
        list->tail = NULL;
}

/* Inserts the element into the specified sorted list at the proper position,
 * as defined by the comparison function, comp_proc.
 *
 * list_ptr: pointer to list-of-interest.
 *
 * elem_ptr: pointer to the element to be inserted into list.
 *
 * If you use list_insert_sorted, the list preserves its sorted nature.
 *
 * If you use list_insert, the list will be considered to be unsorted, even
 * if the element has been inserted in the correct position.
 *
 * If the list is not sorted and you call list_insert_sorted, this subroutine
 * must generate a system error and the program should immediately stop.
 *
 * The comparison procedure must accept two arguments (A and B) which are both
 * pointers to elements of type data_t.  The comparison procedure returns an
 * integer code which indicates the precedence relationship between the two
 * elements.  The integer code takes on the following values:
 *    1: A should be closer to the front of the list than B
 *   -1: B should be closer to the front of the list than A
 *    0: A and B are equal in rank
 *
 * Note: if the element to be inserted is equal in rank to an element already
 * in the list, the newly inserted element will be placed after all the
 * elements of equal rank that are already in the list.
 */
void list_insert_sorted(list_t *list_ptr, data_t *elem_ptr)
//assume list_ptr is pointing to sorted list
{
        assert(NULL != list_ptr);
        assert(SORTED_LIST == list_ptr->list_sorted_state);

        // insert your code here
        IteratorPtr NewNode = (list_node_t*)malloc(sizeof(list_node_t));
        list_node_t *rover = list_ptr->head; //set rover to head every time
        data_t *nodedata = NULL;
        int comp = 0;

        if(list_ptr->head == NULL && list_ptr->tail == NULL) {
                //No elements in the list, set the first and set it to head and tail
                list_ptr->head = NewNode;
                list_ptr->tail = NewNode;
                list_ptr->tail = list_ptr->head;
                NewNode->prev = NULL;
                NewNode->next = NULL;
                NewNode->data_ptr = elem_ptr;
                //FIrst element is both head and tail
                list_ptr->current_list_size++;
                elem_ptr = NULL;
        }
        else{
                //doesnt matter where you insert, is going to be sorted
                //Treat as if you are inserting a head
                rover->prev = NewNode;
                NewNode->next = rover; //saves block before moving backwards
                NewNode->prev = NULL; //Null behind first entry
                NewNode->data_ptr = elem_ptr;
                list_ptr->current_list_size++;
                list_ptr->head = NewNode; //New head set
                rover = NewNode;

                while (rover->next != NULL) { //update head and tail pointers
                        comp = list_ptr->comp_proc(rover->data_ptr, rover->next->data_ptr);
                        if(comp == -1) {
                                //  -1 element on left NEEDS to  be changed
                                //changing data_ptr
                                nodedata = rover->next->data_ptr;
                                rover->next->data_ptr = rover->data_ptr;
                                rover->data_ptr = nodedata;

                        }
                        // 1 element does not need to be swapped
                        rover = rover->next;
                }
        }
        // the last line of this function must be the following
        //  list_debug_validate(list_ptr);
}

/* Inserts the data element into the list in front of the iterator
 * position.
 *
 * list_ptr: pointer to list-of-interest.
 *
 * elem_ptr: pointer to the memory block to be inserted into list.
 *
 * idx_ptr: pointer to a list_node_t.  The element is to be inserted as a
 *          member in the list at the position that is immediately in front
 *          of the position of the provided Iterator pointer.
 *
 *          If the idx_ptr pointer is NULL, the the new memory block is
 *          inserted after the last element in the list.  That is, a null
 *          iterator pointer means make the element the new tail.
 *
 * If idx_ptr is set using
 *    -- NULL - New BACK end of the list
 *    -- list_iter_front, then the new element becomes the FIRST item in
 *       the list. - if idx_ptr == list_head
 *    -- list_iter_back, then the new element is inserted BEFORE the last item
 *       in the list. if idx_ptr == list_tail
 *    -- for any other idx_ptr, the new element is insert BEFORE the
 *       Iterator
 *
 * For example, to insert at the tail of the list do
 *      list_insert(mylist, myelem, NULL)
 * to insert at the front of the list do
 *      list_insert(mylist, myelem, list_iter_front(mylist))
 *
 * Note that use of this function results in the list to be marked as unsorted,
 * even if the element has been inserted in the correct position.  That is, on
 * completion of this subroutine the list_ptr->list_sorted_state must be equal
 * to UNSORTED_LIST.
 */
void list_insert(list_t *list_ptr, data_t *elem_ptr, list_node_t * idx_ptr)
{
        assert(NULL != list_ptr);
        IteratorPtr NewNode = (list_node_t *)malloc(sizeof(list_node_t));
        // insert your code after this line
        //CASE 1 NO LIST CREATED
        if(list_ptr->head == NULL && list_ptr->tail == NULL) {
                //No elements in the list, set the first and set it to head and tail
                list_ptr->head = NewNode;
                list_ptr->tail = NewNode;
                idx_ptr = list_ptr->head;
                list_ptr->tail = list_ptr->head;
                idx_ptr->prev = NULL;
                idx_ptr->next = NULL;
                idx_ptr->data_ptr = elem_ptr;
                //FIrst element is both head and tail
                list_ptr->current_list_size++;
                elem_ptr = NULL;
        }
        else{
                //CASE 2 CREATE NEW TAIL IN LIST
                if(idx_ptr == NULL) {
                        //inserting to back of list
                        //tail and idx ptr cannot be equal in this case
                        list_ptr->tail->next = NewNode;  // malloc new space for created node t
                        NewNode->prev = list_ptr->tail;
                        list_ptr->tail = NewNode;
                        NewNode->next= NULL; //last entr
                        NewNode->data_ptr = elem_ptr;
                        list_ptr->current_list_size++;

                }
                //CASE 3 CREATE NEW HEAD IN LIST
                else if(idx_ptr == list_ptr->head) { //insert at the front
                        idx_ptr->prev = NewNode;
                        idx_ptr->prev->next = idx_ptr; //saves block before moving backwards
                        idx_ptr->prev = idx_ptr; //prev ptr (of next block) points to cuurent block
                        idx_ptr->prev->prev = NULL; //Null behind first entry
                        idx_ptr->prev->data_ptr = elem_ptr;
                        list_ptr->current_list_size++;
                        list_ptr->head = list_ptr->head->prev; //New head set
                }
                //CASE 4 INSERT BEFORE LAST ELEMENT
                else if(idx_ptr == list_ptr->tail) {
                        idx_ptr = idx_ptr->prev;
                        list_ptr->tail->prev = NewNode;
                        list_ptr->tail->prev->prev = idx_ptr;
                        idx_ptr->next = list_ptr->tail->prev;
                        list_ptr->tail->prev->next = list_ptr->tail;
                        idx_ptr->next->data_ptr = elem_ptr;
                        list_ptr->current_list_size++;
                }
                //FINAL CASE INSERT BEFORE IDX (insert before block that is pointer)
                else{
                        idx_ptr->prev->next = NewNode;
                        NewNode->next = idx_ptr;
                        NewNode->prev = idx_ptr->prev;
                        idx_ptr->prev = NewNode;
                        NewNode->data_ptr = elem_ptr;
                        list_ptr->current_list_size++;
                }

        }
        // the last two lines of this function must be the following
        if (list_ptr->list_sorted_state == SORTED_LIST)
                list_ptr->list_sorted_state = UNSORTED_LIST;
        //  list_debug_validate(list_ptr);
}

/* Removes the element from the specified list that is found at the
 * iterator pointer.  A pointer to the data element is returned.
 *
 * list_ptr: pointer to list-of-interest.
 *
 * idx_ptr: pointer to position of the element to be accessed.  This pointer
 *          must be obtained from list_elem_find, list_iter_front, or
 *          list_iter_next, or list_iter_back.
 *
 *          If the idx_ptr is null, then assume that the first item
 *          at the head is to be removed.
 *
 * If the list is empty, then the function should return NULL.  Note: if
 *    the list is empty, then the iterator should be a NULL pointer.
 *
 * Note to remove the element at the front of the list use either
 *     list_remove(mylist, list_iter_front(mylist))
 *  or
 *     list_remove(mylist, NULL);
 *
 * Note: a significant danger with this function is that once
 * an element is removed from the list, the idx_ptr is dangling.  That
 * is, the idx_ptr now points to memory that is no longer valid.
 * You should not call list_iter_next on an iterator after there
 * has been a call to list_remove with the same iterator.
 *
 * When you remove the list_node_t in this function, you should null the next
 * and prev pointers before you free the memory block to avoid accidental use
 * of these now invalid pointers.
 */
data_t * list_remove(list_t *list_ptr, list_node_t * idx_ptr)
{
        assert(NULL != list_ptr);
        IteratorPtr junk = idx_ptr;
        data_t *junkdata = NULL;
        if(list_ptr == NULL && idx_ptr == NULL) {
                return NULL;
        }
        if (0 == list_ptr->current_list_size)
                assert(idx_ptr == NULL);
        if(list_ptr->current_list_size == 1) {
                junkdata = list_ptr->head->data_ptr;
                list_ptr->current_list_size = 0;
                junk = list_ptr->head;
                list_ptr->head = NULL;
                list_ptr->tail = NULL;
                free(junk);
        }
        else if(idx_ptr == NULL || idx_ptr == list_ptr->head) {
                junkdata = list_ptr->head->data_ptr;
                if(idx_ptr == NULL) {
                        idx_ptr = list_ptr->head;
                        junk = list_ptr->head;
                        junkdata = junk->data_ptr;
                }
                list_ptr->head = idx_ptr->next; //set head before nulling
                list_ptr->current_list_size--;
                list_ptr->head->prev = NULL;
                junk->next = NULL;
                free(junk);
        }
        else if(idx_ptr == list_ptr->tail) {
                junkdata = idx_ptr->data_ptr;
                list_ptr->tail = idx_ptr->prev; //set tail before nulling
                list_ptr->current_list_size--;
                idx_ptr = idx_ptr->prev;
                junk->prev = NULL;
                idx_ptr->next = NULL;
                free(junk);
        }
        else{
                junkdata = idx_ptr->data_ptr;
                list_ptr->current_list_size--;
                idx_ptr->prev->next = idx_ptr->next;
                idx_ptr->next->prev = idx_ptr->prev;
                idx_ptr->next = NULL;
                idx_ptr->prev = NULL;
                free(idx_ptr);
        }
        // the last line should verify the list is valid after the remove
        //  list_debug_validate(list_ptr);
        return junkdata;
}

/* Return a pointer to an element stored in the list, at the Iterator position
 *
 * list_ptr: pointer to list-of-interest.  A pointer to an empty list is
 *           obtained from list_construct.
 *
 * idx_ptr: pointer to position of the element to be accessed.  This pointer
 *          must be obtained from list_elem_find, list_iter_front,
 *          list_iter_back, or list_iter_next.
 *
 * return value: pointer to the data_t element found in the list at the
 * iterator position. A value NULL is returned if
 *     a:  the idx_ptr is NULL.
 *     b:  the list is empty
 */
data_t * list_access(list_t *list_ptr, list_node_t * idx_ptr)
{
        assert(NULL != list_ptr);
        if (idx_ptr == NULL || (list_ptr->head == NULL && list_ptr->tail == NULL))
                return NULL;

        // debugging function to verify that the structure of the list is valid
        //    list_debug_validate(list_ptr);

        return idx_ptr->data_ptr;
}

/* This function verifies that the pointers for the two-way linked list are
 * valid. It checks if the list size equals the number of items in the list.
 *
 * YOU MUST NOT CHANGE THIS FUNCTION.  WE USE IT DURING GRADING TO VERIFY THAT
 * YOUR LIST IS CONSISTENT.
 *
 * No output is produced if the two-way linked list is correct.
 * The program terminates and prints a line beginning with "Assertion
 * failed:" if an error is detected.
 *
 * The checks are not exhaustive. An error may still exist in the list even
 * if these checks pass.
 *
 * If the linked list is sorted it also checks that the elements in the list
 * appear in the proper order.
 *
 */
void list_debug_validate(list_t *L)
{
        assert(NULL != L);
        assert(SORTED_LIST == L->list_sorted_state || UNSORTED_LIST == L->list_sorted_state);
        if (0 == L->current_list_size) assert(NULL == L->head && L->tail == NULL);
        if (NULL == L->tail)
                assert(NULL == L->head && 0 == L->current_list_size);
        else
                assert(NULL == L->tail->next);
        if (NULL == L->head)
                assert(NULL == L->tail && 0 == L->current_list_size);
        else
                assert(NULL == L->head->prev);
        if (NULL != L->tail && L->tail == L->head) assert(1 == L->current_list_size);
        if (1 == L->current_list_size) {
                assert(L->head == L->tail && NULL != L->tail);
                assert(NULL != L->tail->data_ptr);
                assert(NULL == L->tail->next && NULL == L->head->prev);
        }
        if (1 < L->current_list_size) {
                assert(L->head != L->tail && NULL != L->tail && NULL != L->head);
                list_node_t *R = L->head;
                int tally = 0;
                while (NULL != R) {
                        if (NULL != R->next) assert(R->next->prev == R);
                        else assert(R == L->tail);
                        assert(NULL != R->data_ptr);
                        ++tally;
                        R = R->next;
                }
                assert(tally == L->current_list_size);
        }
        if (NULL != L->head && SORTED_LIST == L->list_sorted_state) {
                list_node_t *R = L->head;
                while (NULL != R->next) {
                        assert(-1 != L->comp_proc(R->data_ptr, R->next->data_ptr));
                        R = R->next;
                }
        }
}
/* commands for vim. ts: tabstop, sts: softtabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
