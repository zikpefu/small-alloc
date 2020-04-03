/* datatypes.h 
 * Lab4: Dynamic Memory Allocation
 * ECE 2230, Fall 2018
 *
 * The list ADT is needed to support the equilibrium test driver.
 * 
 * We need to set up two definitions to use the list ADT defined in list.c and
 * list.h
 *
 * data_t: The type of data that we want to store in the list
 */

/* include the definitions of the data we want to store in the list */

/* a dummy definition as MP4 creates arrays with a dynamic size */
typedef struct int_tab {
    int i;
} int_t;

/* the list ADT works on data of this type */
typedef int_t data_t;

/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
