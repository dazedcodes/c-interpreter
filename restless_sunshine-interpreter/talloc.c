#include <stdlib.h>
#include <stdio.h>
#include "value.h"

Value *allocatedList;//store all talloc'd pointers in a globally accessible list
int memoryCount = 0;

void initializeList(){
    allocatedList = malloc(sizeof(Value));
    (*allocatedList).type = NULL_TYPE;
    memoryCount += sizeof(Value); 
}
// Replacement for malloc that stores the pointers allocated.
void *talloc(size_t size){
    if (allocatedList == NULL){
        initializeList();
    }
    void *newValue = malloc(size); 
    memoryCount += size; 
    Value *valCell = malloc(sizeof(Value));
    memoryCount += sizeof(Value); 
    (*valCell).type = CONS_TYPE;
    (*valCell).c.car = newValue;
    (*valCell).c.cdr = allocatedList;
    allocatedList = valCell;    
    return newValue;
}

// Free all pointers allocated by talloc, as well as whatever memory you
// allocated in lists to hold those pointers.
void tfree(){
    Value *currNode = allocatedList;
    Value *nextNode = NULL;
    while ((*currNode).type == CONS_TYPE){
        nextNode = (*currNode).c.cdr;
        free((*currNode).c.car);
        free(currNode);
        currNode = nextNode;
    }
    free(currNode);
    allocatedList = NULL;
    memoryCount = 0; //Dave's instructions were unclear on if he wanted freed memory 
                    //included in this, but delete this memoryCount = 0 line if so 
}
// Returns the amount of memory that has been 'tallocated' and not freed yet 
int tallocMemoryCount(){
    return memoryCount;
}
// Replacement for the C function "exit", that frees the allocatedList and exits the program.
void texit(int status){
    tfree();
    exit(status);
}
