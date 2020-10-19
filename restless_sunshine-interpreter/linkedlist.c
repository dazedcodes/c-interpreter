#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "linkedlist.h"
#include "talloc.h"

// Create a new NULL_TYPE value node.
 Value *makeNull(){
     Value *myPointer = talloc(sizeof(Value));
     (*myPointer).type = NULL_TYPE;
     return myPointer;
 }
// Create a new CONS_TYPE value node.
Value *cons(Value *newCar, Value *newCdr){
    Value *consCell = talloc(sizeof(Value));
    (*consCell).c.car = newCar;
    (*consCell).c.cdr = newCdr;
    (*consCell).type = CONS_TYPE;

    return consCell;
}

// Display the contents of the linked list to the screen.
void display(Value *list){
    Value *currNode = list;
    int j = 0;
    switch ((*currNode).type) {
        case INT_TYPE:
            printf("%i \n", (*currNode).i);
            break;
        case DOUBLE_TYPE:
            printf("%f \n", (*currNode).d);
            break;
        case STR_TYPE:
            printf("%s \n", (*currNode).s);
            break;
        case CONS_TYPE:
            display(car(currNode));
            display(cdr(currNode));
            break;
        case NULL_TYPE:
            printf("'()\n");
            break;
        default:
            printf("Invalid display type");
   }
}
// Return a new list that is the reverse of the one that is passed in.
Value *copy(Value *item){
    Value *copy = talloc(sizeof(Value));
    if ((*item).type == STR_TYPE){
       (*copy).s =  talloc(sizeof(char) * (1 + strlen((*item).s)));
       strcpy((*copy).s, (*item).s);
       (*copy).type = STR_TYPE;
    } else {
        *copy = *item;
    }
    return copy;
}
// Reverses the input list.
Value *reverse(Value *list){
    Value *reversedList = makeNull();
    while ((*list).type != NULL_TYPE){
        reversedList = cons(car(list), reversedList);
        list = cdr(list);
    }
      return reversedList;
}
// Utility to make it less typing to get car value.
Value *car(Value *list){
    assert(list != NULL);
    assert((*list).type = CONS_TYPE);
    return (*list).c.car ;
}
// Utility to make it less typing to get cdr value.
Value *cdr(Value *list){
    assert(list != NULL);
    assert ((*list).type == CONS_TYPE || (*list).type == FAKECONS_TYPE);
    return (*list).c.cdr;
}
bool isNull(Value *value){
    return ((*value).type == NULL_TYPE);
}
// Measure length of list.
int length(Value *value) {
    assert ((*value).type == CONS_TYPE || (*value).type == NULL_TYPE);
    int acc = 0;
    while ((*value).type != NULL_TYPE){
        value = (*value).c.cdr;
        acc++;
    }
    return acc;
}
