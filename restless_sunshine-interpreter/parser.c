/* Program framework by Dave Musicant */
#include "value.h"
#include "linkedlist.h"
#include "tokenizer.h"
#include "talloc.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

/* Prints a syntax error depending on depth and exits out of program. */
void syntaxError(int depth) {
  if (depth > 0){
    printf("Syntax error: not enough close parentheses \n");
    texit(1);
  } else if (depth < 0){
    printf("Syntax error: too many close parentheses \n");
    texit(1);
  }
}
/*  A helper function that returns a completed parse tree. */
Value *addToParseTree(Value *subtree, int *updateDepth, Value *token) {
// Update depth if token type is a open parentheses
  if (token->type == OPEN_TYPE) {
    (*updateDepth)++;
  }
  if (token->type != CLOSE_TYPE) {
// Add token to the stack
    subtree = cons(token, subtree);
 } else if (token->type == CLOSE_TYPE){
// Update depth if token type is a close parentheses
   (*updateDepth) = (*updateDepth) - 1;

   Value *stackToken;
   stackToken = car(subtree);

   Value *miniTree = makeNull();
   subtree = cdr(subtree);
   while (stackToken->type != OPEN_TYPE && subtree->type != NULL_TYPE) {
     miniTree = cons(stackToken, miniTree);
     stackToken = car(subtree);
     subtree = cdr(subtree);
   }
   subtree = cons(miniTree, subtree);
 }
 return subtree;
}
/* Takes a list of tokens from a Racket program, and returns a pointer to a
   parse tree representing that program. */
Value *parse(Value *tokens) {

    Value *tree = makeNull();
    int depth = 0;

    Value *current = tokens;
    assert(current != NULL && "Error (parse): null pointer");
    while (current->type != NULL_TYPE) {
        Value *token = car(current);
        tree = addToParseTree(tree, &depth, token);
        current = cdr(current);
    }
    if (depth != 0) {
        syntaxError(depth);
    }
    Value *revTree = reverse(tree);
    return revTree;
}
/* Prints the tree to the screen in a readable fashion. */
void printTreeHelper(Value *tree){
  Value *currNode = tree;
  switch (currNode->type) {
    case CONS_TYPE:
      printf("(");
      while (currNode->type == CONS_TYPE){
        printTreeHelper(car(currNode));
        if (car(currNode)->type == NULL_TYPE){
            printf(" ()");
        }
        currNode = cdr(currNode);
      }
        if (currNode->type != NULL_TYPE){
          printf(".");
          printTreeHelper(currNode);
        } 
      printf(")");
      break;
    case INT_TYPE:
      printf(" %d", (*currNode).i);
      break;
    case DOUBLE_TYPE:
      printf(" %f", (*currNode).d);
      break;
    case STR_TYPE:
      printf("\"%s\" ", (*currNode).s);
      break;
    case BOOL_TYPE:
      if ((*currNode).i == 0){
        printf("#f ");
      } else {
        printf("#t ");
      }
      break;
    case SINGLEQUOTE_TYPE:
      printf("'");
      break;
    case SYMBOL_TYPE:
      printf("%s ", (*currNode).s );
      break;
    case NULL_TYPE:
      break;
    case CLOSURE_TYPE:
      printf("<#procedure>");
      break;
    default:
      printf(" Displaying something without a type. \n");
      break;
    }
}
/* Prints out the parse tree preventing extra parentheses from printing */
void printTree(Value *tree){
  Value *currNode = tree;
    if (currNode->type == CONS_TYPE || currNode->type == NULL_TYPE){
    while (currNode->type != NULL_TYPE){
      printTreeHelper(car(currNode));
      currNode = cdr(currNode);
    }
  } else {
    printf("This is not a legal scheme list. \n");
    texit(1);
  }
}
