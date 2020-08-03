#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bst.h"

// ****************************** build_bst ******************************
// Creates a heap-allocated new singleton node with root value n
ibst_t *new_bst_node(int n) {
  ibst_t *res = malloc(sizeof(ibst_t));
  if (!res) {
    fprintf(stderr, "malloc error in new_bst_node\n");
    exit(-1);
  }
  res->root = n;
  res->lsub = NULL;
  res->rsub = NULL;
  return res;
}

// Will insert the number n into the tree
// The output is a pointer to the same tree as the input
//      The only exception is when the input is NULL
ibst_t *bst_insert(ibst_t *tree, int n) {
  // If the tree is empty, then output a single node
  if (!tree) {
    tree = new_bst_node(n);
  }
  // Otherwise, perform an insert in the correct branch
  else if (n < tree->root) {
    tree->lsub = bst_insert(tree->lsub, n);
  }
  else if (n > tree->root) {
    tree->rsub = bst_insert(tree->rsub, n);
  }
  else {
    fprintf(stderr, "Tree cannot contain duplicates\n");
    exit(-1);
  }
  return tree;
}

// build a bst containing the numbers in array
// - the bst must not contain any duplicates
// - optional challenge: build a balanced bst (not lopsided)
ibst_t *build_bst(int arr[], int len) {
  int i;
  ibst_t *res = NULL;
  for(i = 0; i <= len; i++) {
    res = bst_insert(res, arr[i]);
  }
  return res;
}


// ****************************** well_formed ******************************
// Returns max int in tree
// Error to call with empty tree
int bst_max(ibst_t *tree) {
  if (!tree) {
    fprintf(stderr, "bst_max requires non-empty input tree\n");
    exit(-1);
  }
  if (!tree->rsub)
    return tree->root;
  else
    return bst_max(tree->rsub);
}

// Returns min int in tree
// Error to call with empty tree
int bst_min(ibst_t *tree) {
  if (!tree) {
    fprintf(stderr, "bst_min requires non-empty input tree\n");
    exit(-1);
  }
  if (!tree->lsub)
    return tree->root;
  else
    return bst_max(tree->lsub);
}

// Returns 0 when the tree is not well-formed
int well_formed(ibst_t *t) {
  if (!t)
    return 1;
  if (t->lsub && bst_max(t->lsub) > t->root)
    return 0;
  if (t->rsub && bst_min(t->rsub) < t->root)
    return 0;
  return well_formed(t->lsub) && well_formed(t->rsub);
}


// ****************************** sum ******************************
// A list of bst
// Can be used for either a stack or queue
struct tlist {
  ibst_t *first;
  struct tlist *rest;
};

typedef struct tlist tlist_t;

// Adds an item to the top of the stack
// Returns pointer to new stack
tlist_t *tlist_push(tlist_t *stack, ibst_t *tree) {
  tlist_t *new_node = malloc(sizeof(tlist_t));
  new_node->first = tree;
  new_node->rest = stack;
  return new_node;
}

// Simply returns the top tree on the stack
// Does not modify the stack
ibst_t *tlist_top(tlist_t *stack) {
  return stack->first;
}

// Removes an item from the top of the stack
// Returns pointer to new stack
tlist_t *tlist_pop(tlist_t *stack) {
  tlist_t *rest = stack->rest;
  return rest;
}

// compute the sum of the bst *without recursion*
int sum(ibst_t *t) {
  tlist_t *stack = NULL;
  int sum = 0;
  if(t)
    stack = tlist_push(stack, t);
  while (stack) {
    t = tlist_top(stack);
    sum += t->root;
    stack = tlist_pop(stack);
    if (t->lsub)
      stack = tlist_push(stack, t->lsub);
    if (t->rsub)
      stack = tlist_push(stack, t->rsub);
  }
  return sum;
}

// ****************************** path-to ******************************
// Inserts a new number in the beginning of a list
ilist_t *ilist_insert(ilist_t *list, int n) {
  ilist_t *new_node = malloc(sizeof(ilist_t));
  new_node->first = n;
  new_node->rest = list;
  return new_node;
}

// return the path to the sought number, root first
// if the sought number is absent, return NULL
ilist_t *path_to(int sought, ibst_t *t) {
  ilist_t *rest_of_path = NULL;
  if (!t) {
    return NULL;
  }
  if (sought < t->root) {
    rest_of_path = path_to(sought, t->lsub);
  }
  else if (sought > t->root) {
    rest_of_path = path_to(sought, t->rsub);
  }
  else {
    return ilist_insert(NULL, sought);
  }
  if (!rest_of_path)
    return NULL;
  return ilist_insert(rest_of_path, t->root);
}

// ****************************** preorder ******************************
// Takes as input the pointer to the LAST item in the list!
// Returns a pointer to the LAST item in the list!
ilist_t *ilist_append(ilist_t *tail, int n) {
  tail->rest = malloc(sizeof(ilist_t));
  tail->rest->first = n;
  tail->rest->rest = NULL;
  return tail->rest;
}

// Takes a pointer to the LAST item in a list and adds to that list in preorder
ilist_t *preorder_helper(ilist_t *tail, ibst_t *t) {
  if (!t)
    return tail;
  tail = ilist_append(tail, t->root);
  tail = preorder_helper(tail, t->lsub);
  tail = preorder_helper(tail, t->rsub);
  return tail;
}

// return the preorder traversal of the tree
ilist_t *preorder(ibst_t *t) {
  ilist_t dummy;
  dummy.rest = NULL;
  preorder_helper(&dummy, t);
  return dummy.rest;
}

// ****************************** inorder ******************************
// Takes a pointer to the LAST item in a list and adds in inorder
ilist_t *inorder_helper(ilist_t *tail, ibst_t *t) {
  if (!t)
    return tail;
  tail = inorder_helper(tail, t->lsub);
  tail = ilist_append(tail, t->root);
  tail = inorder_helper(tail, t->rsub);
  return tail;
}

// return the inorder traversal of the tree
ilist_t *inorder(ibst_t *t) {
  ilist_t dummy;
  dummy.rest = NULL;
  inorder_helper(&dummy, t);
  return dummy.rest;
}

// ****************************** postorder ******************************
// Takes a pointer to the LAST item in a list and adds in postorder
ilist_t *postorder_helper(ilist_t *tail, ibst_t *t) {
  if (!t)
    return tail;
  tail = postorder_helper(tail, t->lsub);
  tail = postorder_helper(tail, t->rsub);
  tail = ilist_append(tail, t->root);
  return tail;
}

// return the postorder traversal of the tree
ilist_t *postorder(ibst_t *t) {
  ilist_t dummy;
  dummy.rest = NULL;
  postorder_helper(&dummy, t);
  return dummy.rest;
}

// ****************************** levelorder ******************************
// Add an item to a queue (to the back of a list)
// Input should be a pointer to the LAST item in a list
// This is similar to ilist_append
tlist_t *tlist_enqueue(tlist_t *tail, ibst_t *tree) {
  tail->rest = malloc(sizeof(tlist_t));
  tail->rest->first = tree;
  tail->rest->rest = NULL;
  return tail->rest;
}

// return the level-order traversal of the tree
ilist_t *levelorder(ibst_t *t) {
  tlist_t *queue = NULL;
  tlist_t *queue_tail;
  ilist_t dummy, *list_tail;

  queue = tlist_push(queue, t);
  queue_tail = queue;
  dummy.rest = NULL;
  list_tail = &dummy;
  while (queue) {
    t = tlist_top(queue);
    if (t) {
      list_tail = ilist_append(list_tail, t->root);
      queue_tail = tlist_enqueue(queue_tail, t->lsub);
      queue_tail = tlist_enqueue(queue_tail, t->rsub);
    }
    queue = tlist_pop(queue);
  }
  return dummy.rest;
}

// ****************************** bst_tos ******************************
// build a string representation of the tree
char *bst_tos(ibst_t *t) {
  char *lstr, *rstr, *out;
  int size;

  if (!t)
    return "empty";
  lstr = bst_tos(t->lsub);
  rstr = bst_tos(t->rsub);

  // Decimal representation of an int is at most sizeof(int) * 3 + 1 digits
  size = strlen(lstr) + strlen(rstr) + 1;
  out = malloc(size);
  sprintf(out, "(%d %s %s)", t->root, lstr, rstr);
  return out;
}
  
// build a string representation of a list
char *ilist_tos(ilist_t *l) {
  char *rest_str, *out;
  int size;
  if (!l)
    return "empty";
  rest_str = ilist_tos(l->rest);
  size = strlen(rest_str) + 1;
  out = malloc(size);
  sprintf(out, "%d -> %s", l->first, rest_str);
  return out;
}
