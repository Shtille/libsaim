#ifndef __SAIM_STACK_H__
#define __SAIM_STACK_H__

/*
* Class that implements FIFO logics
*/

typedef struct saim_stack_node {
	void * data;
	struct saim_stack_node * next;
} saim_stack_node;

typedef struct {
	saim_stack_node * head;
	unsigned int length;
} saim_stack;

void saim_stack_create(saim_stack * stack);
void saim_stack_destroy(saim_stack * stack);

void saim_stack_push(saim_stack * stack, void * data);
void * saim_stack_pop(saim_stack * stack);

#endif