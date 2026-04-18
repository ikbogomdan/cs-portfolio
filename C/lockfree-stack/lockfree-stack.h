#pragma once

#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct node {
  atomic_uintptr_t next;
  uintptr_t value;
} lfnode_t;

typedef struct lfstack {
  atomic_uintptr_t top;
} lfstack_t;

int lfstack_init(lfstack_t* stack) {
  stack->top = (atomic_uintptr_t)NULL;

  return 0;  // success
}

int lfstack_push(lfstack_t* stack, uintptr_t value) {
  lfnode_t* new_node = (lfnode_t*)malloc(sizeof(lfnode_t));

  if (new_node == NULL) {
    return -1;
  }

  new_node->value = value;

  atomic_uintptr_t old_top;
  do {
    old_top = stack->top;
    new_node->next = old_top;
  } while (!atomic_compare_exchange_strong(&stack->top, &old_top,
                                           (uintptr_t)new_node));

  return 0;
}

int lfstack_pop(lfstack_t* stack, uintptr_t* value) {
  atomic_uintptr_t top = stack->top;

  if (top == (uintptr_t)NULL) {
    *value = 0;

    return 0;
  }

  while (top != (uintptr_t)NULL &&
         !atomic_compare_exchange_strong(&stack->top, &top,
                                         ((lfnode_t*)top)->next)) {
  }

  *value = ((lfnode_t*)top)->value;
  free((lfnode_t*)top);

  return 0;
}

int lfstack_destroy(lfstack_t* stack) {
  stack->top = (uintptr_t)NULL;

  return 0;
}
