#include "value.h"
#include "tokenizer.h"
#include "talloc.h"
#include "linkedlist.h"
#include <stdlib.h>
#include <stdio.h>


//Function to return a parse tree (in linked list form) of a linked list of tokens    
Value *parse(Value *tokens){
    Value *pstack = makeNull();
    Value *currToken = tokens;
    int depth = 0;

    
    while((*currToken).type != NULL_TYPE) {
        if ((*(*currToken).c.car).type == OPEN_TYPE) {
                    depth++;
                    pstack = cons((*currToken).c.car, pstack);
                    currToken = (*currToken).c.cdr;
        }
        else if ((*(*currToken).c.car).type == CLOSE_TYPE){
            if (depth == 0){
                printf("Syntax error: close parenthesis can't occur before an open parenthesis\n");
                texit(1);
            }
            Value *expr = makeNull();
            depth--;
            while((*pstack).type !=NULL_TYPE){
                if ((*(*pstack).c.car).type == OPEN_TYPE){
                    
                    pstack = (*pstack).c.cdr;  
                    pstack = cons(expr, pstack);    
                    break;
                }
                expr = cons((*pstack).c.car, expr);
                pstack = (*pstack).c.cdr;
               
            }
            
            currToken = (*currToken).c.cdr;
            }
             
        else {
            pstack = cons((*currToken).c.car, pstack);
            currToken = (*currToken).c.cdr;
        }
    }
    if (depth > 0){
        printf("Syntax error after iterating: too few close parentheses\n");
        texit(1);
    } else if (depth < 0) {
        printf("Syntax error: too many close parentheses\n");
        texit(1);
    }
    return reverse(pstack);
}


//Function to print parse tree
void printTree(Value *tree){
    int spaceAfter = 1;
    if ((*tree).type != NULL_TYPE){ //if null cap, do nothing

    if ((*cdr(tree)).type == NULL_TYPE){
        spaceAfter = 0; // If there's a null cap after this cons cell, don't print space after it (there will be a close paren or end of program instead)
    }
    switch ((*car(tree)).type) { 
        //Iterates recursively through tree, putting parenthesis around any s-expressions (or '()'s)    
        case CONS_TYPE:
            printf("(");
            printTree(car(tree)); 
            printf(")");
            if (spaceAfter){
                printf(" ");
            }
            printTree(cdr(tree));
            break;

        case INT_TYPE:
            printf("%d", (*car(tree)).i);
            if (spaceAfter){
                printf(" ");
            }
            printTree(cdr(tree));
            break;

        case DOUBLE_TYPE:
            printf("%f", (*car(tree)).d);
            if (spaceAfter){
                printf(" ");
            }
            printTree(cdr(tree));
            break;

        case STR_TYPE:
            printf("%s", (*car(tree)).s );
            if (spaceAfter){
                printf(" ");
            }
            printTree(cdr(tree));
            break;

        case BOOL_TYPE:
            if (spaceAfter){
                printf("#f");
            } else {
                printf("#t");
            }
            if (spaceAfter){
                printf(" ");
            }
            printTree(cdr(tree));
            break;

        case SYMBOL_TYPE:
            printf("%s", (*car(tree)).s );
            if (spaceAfter){
                printf(" ");
            }
            printTree(cdr(tree)); 
            break;

        case NULL_TYPE:
            //The car of a cons cell is only null when we encountered '()' in the list of tokens
            printf("()");
            if (spaceAfter){
                printf(" ");
            }
            printTree(cdr(tree));
            break;
            
        default:
            break;
        
    }
    }
    
    }



