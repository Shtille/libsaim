#include "saim_stack.h"

#include "saim_memory.h"

void saim_stack_create(saim_stack * stack)
{
	stack->head = NULL;
	stack->length = 0U;
}
void saim_stack_destroy(saim_stack * stack)
{
	void * data;
	while (stack->length != 0)
	{
		data = saim_stack_pop(stack);
		SAIM_FREE(data);
	}
}
void saim_stack_push(saim_stack * stack, void * data)
{
	saim_stack_node * node;
	node = (saim_stack_node *)SAIM_MALLOC(sizeof(saim_stack_node));
	node->data = data;
	node->next = stack->head;
	stack->head = node;
	++stack->length;
}
void * saim_stack_pop(saim_stack * stack)
{
	saim_stack_node * node;
	void * data;
	node = stack->head;
	if (node)
	{
		stack->head = node->next;
		--stack->length;
		data = node->data;
		SAIM_FREE(node);
		return data;
	}
	return NULL;
}