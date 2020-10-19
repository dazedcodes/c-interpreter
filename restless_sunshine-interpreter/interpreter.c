
#include "linkedlist.h"
#include "tokenizer.h"
#include "talloc.h"
#include "interpreter.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

/* Instantiate a null global frame */
Frame *globalFrame = NULL;

/* Takes in the name, function, and frame. Adds primitive functions to top-level bindings list*/
void bind(char *name, Value *(*function)(struct Value *), Frame *frame) {
  // Store primitive function as primitive type
  Value *ptrFunction = talloc(sizeof(Value));
  ptrFunction->type = PRIMITIVE_TYPE;
  ptrFunction->pf = function;

  // Store name of primitive function as a SYMBOL_TYPE
  Value *primName = talloc(sizeof(Value));
  primName->type = SYMBOL_TYPE;
  primName->s = name;

  // Create new binding cons cell with name of primitive function as car &
  // closure to function as the cdr.
  Value *newBinding = cons(primName, ptrFunction);
  frame->bindings = cons(newBinding, frame->bindings);
}

/* Take in arguments, return the result of the modulo of the arguments */
Value *primitiveMod(Value *args){
  // Error check: Number of arguments for must be two.
  if (length(args) != 2){
    printf("Evaluation Error: Invalid number of arguments for equality. Expected 2, Received: %i\n", length(args));
    texit(1);
  }

  int var1;
  int var2;

  // Set var1 as integer or perform error check.
  if (car(args)->type == INT_TYPE){
    var1 = car(args)->i;
  } else {
    printf("Evaluation Error: Invalid type for comparison. Expected INT_TYPE, Received: %i\n", car(args)->type);
    texit(1);
  }
  // Set var2 as integer or perform error check.
  if (car(cdr(args))->type == INT_TYPE){
    var2 = car(cdr(args))->i;
  } else {
    printf("Evaluation Error: Invalid type for comparison. Expected INT_TYPE, Received: %i\n", car(cdr(args))->type);
    texit(1);
  }

  // Compute modulo and set result as INT_TYPE.
  int answer = (var1 % var2);
  Value *result = talloc(sizeof(Value));
  result->type = INT_TYPE;
  result->i = answer;
  return result;
}


/* Takes in arguments and returns boolean #t or #f. */
Value *primitiveEqual(Value *args) {
  // Error check: Number of arguments for must be two.
  if (length(args) != 2){
    printf("Evaluation Error: Invalid number of arguments for equality. Expected 2, Received: %i\n", length(args));
    texit(1);
  }

  float var1;
  float var2;

  // Set var1 as integer, double or perform error check.
  if (car(args)->type == INT_TYPE){
    var1 = car(args)->i;
  } else if (car(args)->type == DOUBLE_TYPE){
    var1 = car(args)->d;
  } else {
    printf("Evaluation Error: Invalid type for comparison. Expected INT or DOUBLE type, Received: %i\n", car(args)->type);
    texit(1);
  }
  // Set var2 as integer, double or perform error check.
  if (car(cdr(args))->type == INT_TYPE){
    var2 = car(cdr(args))->i;
  } else if (car(cdr(args))->type == DOUBLE_TYPE){
    var2 = car(cdr(args))-> d;
  } else {
    printf("Evaluation Error: Invalid type for comparsion. Expected INT or DOUBLE type, Received: %i\n", car(args)->type);
    texit(1);
  }
  // Compute equality check and set result as BOOL_TYPE.
  int answer = (var1 == var2);
  Value *result = talloc(sizeof(Value));
  result->type = BOOL_TYPE;
  result->i = answer;
  return result;
}

/* Takes in arguments and returns boolean #t or #f. */
Value *primitiveGreater(Value *args) {
  // Error check: Number of arguments for must be two.
  if (length(args) != 2){
    printf("Evaluation Error: Invalid number of arguments for comparison. Expected 2, Received: %i\n", length(args));
    texit(1);
  }

  float var1;
  float var2;

  // Set var1 as integer, double or perform error check.
  if (car(args)->type == INT_TYPE){
    var1 = car(args)->i;
  } else if (car(args)->type == DOUBLE_TYPE){
    var1 = car(args)->d;
  } else {
    printf("Evaluation Error: Invalid type for comparison. Expected INT or DOUBLE type, Received: %i\n", car(args)->type);
    texit(1);
  }
  // Set var2 as integer, double or perform error check.
  if (car(cdr(args))->type == INT_TYPE){
    var2 = car(cdr(args))->i;
  } else if (car(cdr(args))->type == DOUBLE_TYPE){
    var2 = car(cdr(args))-> d;
  } else {
    printf("Evaluation Error: Invalid type for comparsion. Expected INT or DOUBLE type, Received: %i\n", car(args)->type);
    texit(1);
  }
  // Compute comparison and set result as BOOL_TYPE.
  int answer = var1 > var2;
  Value *result = talloc(sizeof(Value));
  result->type = BOOL_TYPE;
  result->i = answer;
  return result;
}

/* Takes in arguments and returns a sum of arguments*/
Value *primitiveLess(Value *args) {
  if (length(args) != 2){
    printf("Evaluation Error: Invalid number of arguments for comparison. Expected 2, received: %i\n", length(args));
    texit(1);
  }
  float var1;
  float var2;

  // Set var1 as integer, double or perform error check.
  if (car(args)->type == INT_TYPE){
    var1 = car(args)->i;
  } else if (car(args)->type == DOUBLE_TYPE){
    var1 = car(args)->d;
  } else {
    printf("Evaluation Error: Invalid type for comparison. Expected INT or DOUBLE type, Received: %i\n", car(args)->type);
    texit(1);
  }
  // Set var2 as integer, double or perform error check.
  if (car(cdr(args))->type == INT_TYPE){
    var2 = car(cdr(args))->i;
  } else if (car(cdr(args))->type == DOUBLE_TYPE){
    var2 = car(cdr(args))-> d;
  } else {
    printf("Evaluation Error: Invalid type for comparsion. Expected INT or DOUBLE type, Received: %i\n", car(args)->type);
    texit(1);
  }
  // Compute comparison and set result as BOOL_TYPE.
  int answer = var1 < var2;
  Value *result = talloc(sizeof(Value));
  result->type = BOOL_TYPE;
  result->i = answer;
  return result;
}

/* Helper function that recursively multiplies list of arguments. */
float primMultHelper(Value *args){
  if (args->type == NULL_TYPE){
    return 1;
  } else if (car(args)->type != INT_TYPE && car(args)->type != DOUBLE_TYPE){
    printf("Evaluation error: not a valid type to multiply. Expected INT or DOUBLE type, Recieved: %i\n", car(args)->type);
    texit(1);
    return 0;
  } else if (car(args)->type == NULL_TYPE){
    printf("Evaluation error: Missing a procedure, not a valid type to multiply.\n");
    texit(1);
    return 0;
  } else if (car(args)->type == DOUBLE_TYPE){
    return (car(args)->d * primMultHelper(cdr(args)));
  } else {
    return (car(args)->i * primMultHelper(cdr(args)));
  }
}

/* Takes in arguments. Returns the products of arguments*/
Value *primitiveMult(Value *args) {
  float answer = primMultHelper(args);
  Value *sum = talloc(sizeof(Value));
  sum->type = DOUBLE_TYPE;
  sum->d = answer;
  return sum;
}

/* Takes in arguments. Returns the quotient of arguments. */
Value *primitiveDivide(Value *args) {
  // Error check: arguments must either be an integer or double.
  if (car(cdr(args))->type != INT_TYPE && car(cdr(args))->type != DOUBLE_TYPE){
      printf("Evaluation Error: Invalid type for division. Expected INT or DOUBLE type, Received: %i\n", car(args)->type);
      texit(1);
  }
  // Error check: Number of arguments must be two.
  if (length(args) != 2){
    printf("Evaluation Error: Invalid number of arguments for division. Expected 2, received: %i\n", length(args));
    texit(1);
  }

  float var1;
  float var2;

  // Set var1 as integer, double or perform error check.
  if (car(args)->type == INT_TYPE){
    var1 = car(args)->i;
  } else if (car(args)->type == DOUBLE_TYPE){
    var1 = car(args)->d;
  } else {
    printf("Evaluation Error: Invalid type for division. Expected INT or DOUBLE type, Received: %i\n", car(args)->type);
    texit(1);
  }
  // Set var2 as integer, double or perform error check.
  if (car(cdr(args))->type == INT_TYPE){
    var2 = car(cdr(args))->i;
  } else if (car(cdr(args))->type == DOUBLE_TYPE){
    var2 = car(cdr(args))-> d;
  } else {
    printf("Evaluation Error: Invalid type for division. Expected INT or DOUBLE type, Received: %i\n", car(args)->type);
    texit(1);
  }
  // Error check: you cannot divide by zero.
  if (var2 == 0 || var2 == 0.0){
    printf("Evaluation error: You cannot divide by 0\n");
    texit(1);
  }
  // Compute division and set result as DOUBLE_TYPE.
  float answer = var1 / var2;
  Value *quotient = talloc(sizeof(Value));
  quotient->type = DOUBLE_TYPE;
  quotient->d = answer;
  return quotient;
}

/* Takes in arguments. Returns the subtration of arguments. */
Value *primitiveSub(Value *args) {
  // Error check: arguments must either be an integer or double.
  if (car(cdr(args))->type != INT_TYPE && car(cdr(args))->type != DOUBLE_TYPE){
      printf("Evaluation Error: Invalid type for subtraction. Expected INT or DOUBLE type, Received: %i\n", car(args)->type);
      texit(1);
  }
  // Error check: Number of arguments for must be two.
  if (length(args) != 2){
    printf("Evaluation Error: Invalid number of arguments for subtraction. Expected 2, received: %i\n", length(args));
    texit(1);
  }

  float var1;
  float var2;

  // Set var1 as integer, double or perform error check.
  if (car(args)->type == INT_TYPE){
    var1 = car(args)->i;
  } else if (car(args)->type == DOUBLE_TYPE){
    var1 = car(args)->d;
  } else {
    printf("Evaluation Error: Invalid type for subtraction. Expected INT or DOUBLE type, Received: %i\n", car(args)->type);
    texit(1);
  }
  // Set var2 as integer, double or perform error check.
  if (car(cdr(args))->type == INT_TYPE){
    var2 = car(cdr(args))->i;
  } else if (car(cdr(args))->type == DOUBLE_TYPE){
    var2 = car(cdr(args))-> d;
  } else {
    printf("Evaluation Error: Invalid type for subtraction. Expected INT or DOUBLE type, Received: %i\n", car(args)->type);
    texit(1);
  }
  // Compute subtraction and set result as DOUBLE_TYPE.
  float answer = var1 - var2;
  Value *difference = talloc(sizeof(Value));
  difference->type = DOUBLE_TYPE;
  difference->d = answer;
  return difference;
}

/* Helper function that recursively adds list of arguments. */
float primAddHelper(Value *args){
  if (args->type == NULL_TYPE){
    return 0;
  } else if (car(args)->type != INT_TYPE && car(args)->type != DOUBLE_TYPE){
    printf("Evaluation error: not a valid type to add. Expected INT or DOUBLE type, Recieved: %i\n", car(args)->type);
    texit(1);
    return 0;
  } else if (car(args)->type == NULL_TYPE){
    return 0;
  } else if (car(args)->type == DOUBLE_TYPE){
    return (car(args)->d + primAddHelper(cdr(args)));
  } else{
    return (car(args)->i + primAddHelper(cdr(args)));
  }
}

/* Takes in arguments and returns a sum of arguments*/
Value *primitiveAdd(Value *args) {
  float answer = primAddHelper(args);
  Value *sum = talloc(sizeof(Value));
  sum->type = DOUBLE_TYPE;
  sum->d = answer;
  return sum;
}

/* Takes in arguments. Returns boolean #t or #f for 'null?' */
Value *primitiveNull(Value *args){
  // Error check: Number of arguments must be one.
  if (length(args) != 1){
    printf("Evaluation error: the procedure 'null?' expected in one argument; Recieved: %i arguments.\n", length(args));
    texit(1);
  }
  // Check to move one layer down in nested list.
  if (car(args)->type == CONS_TYPE){
    args = car(args);
  }
  // Return either #t or #f whether arguments are null or not.
  if (car(args)->type == NULL_TYPE){
    Value *answer = talloc(sizeof(Value));
    answer->type = BOOL_TYPE;
    answer->i = 1;
    return answer;
  } else {
    Value *answer = talloc(sizeof(Value));
    answer->type = BOOL_TYPE;
    answer->i = 0;
    return answer;
  }
}

/* Takes in a cons cell. Returns the car of the cons cell */
Value *primitiveCar(Value *args){
  // Error check: Number of arguments must be one.
  if (length(args) != 1){
    printf("Evaluation error: function 'car' expected in one argument; Recieved: %i arguments.\n", length(args));
    texit(1);
  }
  // Error check: Argument must be CONS_TYPE.
  if (car(args)->type != CONS_TYPE){
    printf("Evaluation error: car expected cons type as argument; Received type:%i\n", car(args)->type);
    texit(1);
  }
  return car(car(args));
}

/* Takes in a cons cell. Returns the cdr of the cons cell */
Value *primitiveCdr(Value *args){
  // Error check: Number of arguments must be one.
  if (length(args) != 1){
    printf("Evaluation error: function 'cdr' takes in one argument; you passed in %i.\n", length(args));
    texit(1);
  }
  // Error check: Argument must be CONS_TYPE.
  if (car(args)->type != CONS_TYPE){
    printf("Evaluation error: cdr expected cons type as argument; Received type:%i\n", car(args)->type);
    texit(1);
  }
  return cdr(car(args));
}

/* Takes in two arguments, and returns a cons cell of two arguments */
Value *primitiveCons(Value *args){
  // Error check: Number of arguments must be two.
  if (length(args) != 2){
    printf("Evaluation error: function 'cons' expected two arguments; Recieved: %i arguments.\n", length(args));
    texit(1);
  }
    Value *newCar = car(args);
    Value *newCdr = car(cdr(args));
    Value *newAnswer = cons(newCar, newCdr);
    return newAnswer;
}

/* Takes in closure and args, applies latter to former. Returns the result. */
Value *defApply(Value *function, Value *args){
  Frame *newFrame = talloc(sizeof(Frame));
  newFrame->parent = function->cl.frame;
  newFrame->bindings = makeNull();
  Value *parameters = function->cl.paramNames;
  Value *functionBody = function->cl.functionCode;

  // Error check: the function parameters and arguments must match to apply.
  if (length(parameters) != length(args)){
      printf("Error evaluation: the function parameters and arguments given don't match. \n");
      printf("car(args) type: %i, car(parameters) type: %i, length(args) = %i, length(parameters) = %i\n", car(args)->type, car(parameters)->type, length(args), length(parameters));
      texit(1);
    }
    // Match function parameters to arguments.
    while (parameters->type != NULL_TYPE && args->type != NULL_TYPE){
      Value *matchArgParam = cons(car(parameters), car(args));
      newFrame->bindings = cons(matchArgParam, newFrame->bindings);
      parameters = cdr(parameters);
      args = cdr(args);
    }
    // Evaluate body of function.
    Value *result;
    while (functionBody->type != NULL_TYPE){
      result = eval(car(functionBody), newFrame);
      functionBody = cdr(functionBody);
    }
    return result;
  }

/* Takes in function and args, applies function to args. Returns the result. */
Value *primApply(Value *function, Value *args){
  Value *(*primFunction)(Value *) = *(function->pf);
  Value *result = (*primFunction)(args);
  return (*primFunction)(args);
}

/* Takes in function and arguments from function application.
   Returns the result of the applied function. */
Value *apply(Value *function, Value *args){
  if (function->type != PRIMITIVE_TYPE){
    return defApply(function, args);
  } else {
    return primApply(function, args);
  }
}

/* Takes in arguments and current frame. Returns the result of the last argument */
Value *evalBegin(Value *args, Frame *frame){
  Value *result = talloc(sizeof(Value));
  result->type = VOID_TYPE;
  while (args->type != NULL_TYPE){
    result = eval(car(args), frame);
    args = cdr(args);
  }
  return result;
}
/* Takes in arguments and looks up symbol. Returns new value of symbol. */
Value *evalSet(Value *args, Frame *frame){
  Value *var = car(args);
  // Error check: Must set values to only SYMBOL_TYPE.
  if (var->type != SYMBOL_TYPE){
    printf("Evaluation error: Cannot set value of non-symbol. Expected symbol type, Received: %i\n", var->type);
    texit(1);
    return var;
  }

  Value *newVal = eval(car(cdr(args)), frame);
  Frame *currFrame = frame;
  while (currFrame != NULL){
    Value *currBinding = currFrame->bindings;
    while (currBinding->type != NULL_TYPE){
      if (!strcmp(car(car(currBinding))->s,var->s)){
        car(currBinding)->c.cdr = newVal;
        Value *returnMe = talloc(sizeof(Value));
        returnMe->type = VOID_TYPE;
        return returnMe;
      } else {
        currBinding = cdr(currBinding);
      }
    }
    currFrame = currFrame->parent;
  }
  // Error check: Symbol not defined if while loop couldn't find symbol.
  printf("Error: could not find symbol %s\n", car(args)->s);
  texit(1);
  return var;
}
/* Takes in arguments. Returns the result of letrec. */
Value *evalLetRec(Value *args, Frame *frame){
  // Make tempFrame and pointer to currBinding.
  Frame *tempFrame = talloc(sizeof(Frame));
  tempFrame->parent = frame;
  tempFrame->bindings = makeNull();
  Value *currBinding = car(args);
  Value *listVars = makeNull();

  // Loop through bindings and sets the left-hand side to undefined.
  while (currBinding->type != NULL_TYPE) {
    Value *currVar = car(car(currBinding));
    Value *checkVar = listVars;
    while (checkVar->type != NULL_TYPE){
      if (!strcmp(car(checkVar)->s, currVar->s)){
          printf("Evaluation error: cannot assign variable multiple times in letrec\n");
          texit(1);
      }
      checkVar = cdr(checkVar);
    }
    listVars = cons(currVar, listVars);
    
    Value *undef = talloc(sizeof(Value));
    undef->type = UNDEFINED_TYPE;
    Value *newBinding = cons(currVar, undef);
    tempFrame->bindings = cons(newBinding, tempFrame->bindings);
    currBinding = cdr(currBinding);
  }

  // Makes new frame and reset what currBinding points to.
  Frame *newFrame = talloc(sizeof(Frame));
  newFrame->parent = frame;
  newFrame->bindings = makeNull();
  currBinding = car(args);

  Value *currVal;
  Value *currVar;

  // Evaluates the right-hand side of currBinding and defines currVar in newFrame
  while (currBinding->type != NULL_TYPE) {
    currVar = car(car(currBinding));
    currVal = eval(car(cdr(car(currBinding))), tempFrame);
    Value *newBinding = cons(currVar, currVal);
    newFrame->bindings = cons(newBinding, newFrame->bindings);
    currBinding = cdr(currBinding);
  }
  currBinding = car(args);
  while (currBinding->type != NULL_TYPE){
    currVar = car(car(currBinding));
    currVal = eval(currVar, newFrame);
    Value *setArgs = cons(currVal, makeNull());
    setArgs = cons(currVar, setArgs);
    evalSet(setArgs, tempFrame);
    currBinding = cdr(currBinding);
  }
  Value *result = evalBegin(cdr(args), newFrame);
  return result;
}
/* Takes in arguments and returns a boolean #t or #f. */
Value *evalCond(Value *args, Frame *frame){
  Value *result = talloc(sizeof(Value));
  result->type = VOID_TYPE;
  while (args->type != NULL_TYPE){
    if(car(args)->type != CONS_TYPE){
      printf("Evaluation error: bad syntax for cond\n");
      texit(1);
      return args;
    }
    Value *condition;
    if (car(car(args))->type == SYMBOL_TYPE) {
      if (!strcmp(car(car(args))->s,"else")){
        condition = talloc(sizeof(Value));
        condition->type = BOOL_TYPE;
        condition->i = 1;
      } else {
        condition = eval(car(car(args)), frame);
      }
    } else {
      condition = eval(car(car(args)), frame);
    }
    if (condition->type != BOOL_TYPE){
      printf("Evaluation error: Invalid type for cond. Expected boolean, received %i\n", condition->type);
      texit(1);
    }
    if (condition->i == 1){
      result = eval(car(cdr(car(args))), frame);
      return result;
    }
    args = cdr(args);
  }
  return result;
}

/* Takes in arguments and current frame. Returns boolean as result */
Value *evalOr(Value *args, Frame *frame){
  if (length(args) == 0){
    Value *result = talloc(sizeof(Value));
    result->type = BOOL_TYPE;
    result->i = 0;
    return result;
  }
  Value *tempResult;
  while (args->type != NULL_TYPE) {
    tempResult = eval(car(args), frame);
    if (tempResult->i == 1){
      return tempResult;
    } else {
      args = cdr(args);
    }
  }
  return tempResult;
}

/* Takes in arguments and current frame. Returns boolean as result */
Value *evalAnd(Value *args, Frame *frame){
  if (length(args) == 0){
    Value *result = talloc(sizeof(Value));
    result->type = BOOL_TYPE;
    result->i = 1;
    return result;
  }
  Value *tempResult;
  while (args->type != NULL_TYPE) {
    tempResult = eval(car(args), frame);
    if (tempResult->i == 0){
      return tempResult;
    } else {
      args = cdr(args);
    }
  }
  return tempResult;
}

/* Take in a let but make a nested let frame for each binding */
Value *evalLetStar(Value *args, Frame *frame){
  Frame *currFrame = frame;
  Value *framesList = car(args);
  Value *body = cdr(args);
  // Error check: Bindings must be in nested list.
  if (framesList->type != CONS_TYPE && framesList->type != NULL_TYPE) {
    printf("Evaluation error: bindings for let expression must be in nested list\n");
    texit(1);
  } else if (framesList->type != NULL_TYPE){
    if (car(framesList)->type != CONS_TYPE && car(framesList)->type != NULL_TYPE){
      printf("Evaluation error: bindings for let expression must be in nested list\n");
      texit(1);
    }
  }
  // Error check: Let expression must have body.
  if (body->type == NULL_TYPE){
    printf("Evaluation error: no body in let expression.\n");
    texit(1);
  }
  while (framesList->type != NULL_TYPE){
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->bindings = makeNull();
    newFrame->parent = currFrame;
    if (framesList->type != CONS_TYPE || car(framesList)->type != CONS_TYPE){
      printf("Evaluation Error: invalid types in let bindings\n");
      texit(1);
    }
    Value *var = car(car(framesList));
    Value *varVal = car(cdr(car(framesList)));
    Value *varAndVal = cons(var, varVal);
    newFrame->bindings = cons(varAndVal, newFrame->bindings);

    // currFrame now points to newFrame just created
    currFrame = newFrame;
    framesList = cdr(framesList);
  }
  // All frames have been made! Evaluate the body accordingly.
  Value *result;
  while (body->type != NULL_TYPE){
    result = eval(car(body), currFrame);
    body = cdr(body);
  }
  return result;
}

/* Takes in a list of arguments and frame. Returns evaluated list of listArgs. */
Value *evalEach(Value *listArgs, Frame *frame){
  Value *evaledList = makeNull();
  while (listArgs->type != NULL_TYPE){
    Value *result = eval(car(listArgs), frame);
    evaledList = cons(result, evaledList);
    listArgs = cdr(listArgs);
  }
  evaledList = reverse(evaledList);
  return evaledList;
}

/* Takes in arguments and current frame.
   Returns closure containing frame, parameters, and function body.  */
Value *evalLambda(Value *args, Frame *frame){
    Value *newClosure = talloc(sizeof(Value));
    newClosure->type = CLOSURE_TYPE;
    newClosure->cl.frame = frame;
    newClosure->cl.paramNames = car(args);
    newClosure->cl.functionCode = cdr(args);
    return newClosure;
}

/* Takes in arguments. Adds definition to bindings and returns void type  */
Value *evalDefine(Value *args){
  if (car(args)->type != SYMBOL_TYPE){
    printf("Evaluation error: not a valid symbol type to define\n");
    texit(1);
  }
  Value *var = cons(car(args), eval(car(cdr(args)), globalFrame));
  Value *returnMe = talloc(sizeof(Value));
  returnMe->type = VOID_TYPE;
  globalFrame->bindings = cons(var, globalFrame->bindings);
  return returnMe;
}

/* Takes in arguments and current frame. Returns arguments unchanged.  */
Value *evalQuote(Value *args){
  Value *result = car(args);
  return result;
}

/* Takes in arguments and current frame. Returns the result of let expression. */
Value *evalLet(Value *args, Frame *frame) {
  Frame *newFrame = talloc(sizeof(Frame));
  newFrame->bindings = makeNull();
  newFrame->parent = frame;
  Value *listBindings = car(args);
  Value *listVars = makeNull();

  // Error check: Bindings for let expression must be in nested list.
  if (listBindings->type != CONS_TYPE && listBindings->type != NULL_TYPE) {
    printf("Evaluation error: bindings for let expression must be in nested list\n");
    texit(1);
  } else if (listBindings->type != NULL_TYPE){
    if (car(listBindings)->type != CONS_TYPE && car(listBindings)->type != NULL_TYPE){
      printf("Evaluation error: bindings for let expression must be in nested list\n");
      texit(1);
    }
  }
  Value *body = cdr(args);
  if (body->type == NULL_TYPE){
    printf("Evaluation error: no body in let expression.\n");
    texit(1);
  }
  // Error check: Cannot bind value to non-symbol.
  while (listBindings->type != NULL_TYPE) {
    Value *var = car(car(listBindings));
    if (var->type != SYMBOL_TYPE){
      printf("Evaluation error: cannot bind value to non-symbol.\n");
      printf("You tried to bind %i to value.\n", var->type);
      texit(1);
    }
    Value *checkVar = listVars;
    while (checkVar->type != NULL_TYPE){
      if (!strcmp(car(checkVar)->s, var->s)){
          printf("Evaluation error: cannot assign variable multiple times in let\n");
          texit(1);
      }
      checkVar = cdr(checkVar);
    }
    listVars = cons(var, listVars);
    Value *varValue = car(cdr(car(listBindings)));
    printf("varValue type is: %i\n", varValue->type);
    varValue = eval(varValue, frame);
    Value *varAndVal = cons(var, varValue);
    newFrame->bindings = cons(varAndVal, newFrame->bindings);
    listBindings = cdr(listBindings);
  }
  Value *result;
  while (body->type != NULL_TYPE){
    result = eval(car(body), newFrame);
    body = cdr(body);
  }
  return result;
}

/* Takes in arguments and current frame. Returns the evaluation of if. */
Value *evalIf(Value *args, Frame *frame){
  if (length(args) != 3){
    printf("Evaluation error: if statement must have three arguments. You had %i arguments.\n", length(args));
    texit(1);
  }
  Value *condition = eval(car(args), frame);
  if (condition->type != BOOL_TYPE){
    printf("Evaluation error: condition must be boolean (this condition is type %i)\n", condition->type);
    texit(1);
  }
  if (condition->i == 1){
    return eval(car(cdr(args)), frame);
  } else {
    return eval(car(cdr(cdr(args))), frame);
  }
}

/* Takes in symbol and current frame. Returns the binding of symbol if present. */
Value *lookUpSymbol(Value *currSymbol, Frame *frame){
  if (frame == NULL){
    printf("Evaluation error: symbol is not defined: %s\n", currSymbol->s);
    texit(1);
  }
  Value *listBindings = frame->bindings;

  while (listBindings->type != NULL_TYPE){
    if (!strcmp((car(car(listBindings)))->s, currSymbol->s)){
      Value *symbolVal = cdr(car(listBindings));
      if (symbolVal->type == SYMBOL_TYPE){
        return lookUpSymbol(symbolVal, frame->parent);
      }
      return cdr(car(listBindings));
    } else {
      listBindings = cdr(listBindings);
    }
  }
  return lookUpSymbol(currSymbol,frame->parent);
}

/*Helper function to print out S-expression. */
void printInterp(Value *tree){
  Value *currNode = tree;
    if (currNode->type == NULL_TYPE){
      printf("()");
  } else if (currNode->type != VOID_TYPE){
    printTreeHelper(currNode);
  }
}

/* Takes in an S-expression and evaluates it. */
void interpret(Value *tree) {
    globalFrame = talloc(sizeof(Frame));
    globalFrame->parent = NULL;
    globalFrame->bindings = makeNull();

    bind("+", primitiveAdd, globalFrame);
    bind("null?", primitiveNull, globalFrame);
    bind("car", primitiveCar, globalFrame);
    bind("cdr", primitiveCdr, globalFrame);
    bind("cons", primitiveCons, globalFrame);
    bind("*", primitiveMult, globalFrame);
    bind("-", primitiveSub, globalFrame);
    bind("<", primitiveLess, globalFrame);
    bind(">", primitiveGreater, globalFrame);
    bind("/", primitiveDivide, globalFrame);
    bind("=", primitiveEqual, globalFrame);
    bind("modulo", primitiveMod, globalFrame);

  while (tree->type != NULL_TYPE) {
    Value *currToken = car(tree);
    Value *result = eval(currToken, globalFrame);
    printInterp(result);
    if (result->type != VOID_TYPE){
      printf("\n");
    }
    tree = cdr(tree);
  }
}

/* Takes in a pointer to a parsed tree and empty frame. Returns an evaluation of
   parse tree. */
Value *eval(Value *tree, Frame *frame) {
   switch (tree->type)  {
     case INT_TYPE: {
        return tree;
        break;
     }
     case DOUBLE_TYPE: {
         if (tree->d == (int)(tree->d)){
           tree->type = INT_TYPE;
           tree->i = (int)(tree->d);
         }
         return tree;
         break;
     }
     case BOOL_TYPE: {
        return tree;
        break;
     }
     case STR_TYPE: {
        return tree;
        break;
     }
     case CLOSURE_TYPE: {
        return tree;
        break;
     }
     case UNDEFINED_TYPE: {
        printf("Evaluation error: variable undefined.\n");
        texit(1);
        return tree;
        break;
     }
     case SYMBOL_TYPE: {
        return lookUpSymbol(tree, frame);
        break;
     }
     case CONS_TYPE: {
        Value *first = car(tree);
        Value *args = cdr(tree);
        Value *result;
         if (first->type != SYMBOL_TYPE){
           if(first->type == CONS_TYPE){
              // do nothing.
            } else {
              printf("Evaluation error: first argument in s-expression must be procedure. first argument was type %i\n", first->type);
              texit(1);
            }
          }
          if (!strcmp(first->s,"if")) {
            result = evalIf(args, frame);
          } else if(!strcmp(first->s,"let*")){
            if (length(args) < 2){
              printf("Evaluation error: invalid number of arguments for let*. Expected at least 2, Recieved: %i\n", length(args));
              texit(1);
            }
            result = evalLetStar(args, frame);
          } else if(!strcmp(first->s,"begin")){
            result = evalBegin(args, frame);
          } else if (!strcmp(first->s,"let")) {
            result = evalLet(args, frame);
          } else if (!strcmp(first->s,"quote")) {
            if (length(args) != 1) {
                printf("Evaluation error: invalid number of arguments for quote. Expected 1, Recieved: %i\n", length(args));
                texit(1);
              }
              result = evalQuote(args);
            } else if (!strcmp(first->s,"define")) {
              if (length(args) != 2){
                printf("Evaluation error: invalid number of arguments for define. Expected 2, Recieved: %i\n", length(args));
                texit(1);
              }
              result = evalDefine(args);
            } else if (!strcmp(first->s,"lambda")) {
              if (length(args) < 2){
                printf("Evaluation error: invalid number of arguments for lambda. Expected  at least 2, Recieved: %i\n", length(args));
                texit(1);
              }
              if (car(args)->type == NULL_TYPE){
                result = evalLambda(args, frame);
                } else if (car(args)->type != CONS_TYPE){
                  printf("Evaluation error: lambda requires parameters in parentheses. Your parameter was type %i\n", car(args)->type);
                  texit(1);
                } else if (car(car(args))->type != SYMBOL_TYPE){
                  printf("Evaluation error: for lambda(x), x must be a symbol or empty parentheses.\n");
                  texit(1);
                } else {
                  result = evalLambda(args, frame);
                }
              } else if (!strcmp(first->s,"set!")) {
                if (length(args) != 2){
                  printf("Evaluation error: Invalid number of arguments. Expected 2, received %i\n", length(args));
                  texit(1);
                }
                result = evalSet(args, frame);
              } else if (!strcmp(first->s,"and")) {
                result = evalAnd(args, frame);
              } else if (!strcmp(first->s,"or")) {
                result = evalOr(args, frame);
              } else if (!strcmp(first->s,"cond")) {
                result = evalCond(args, frame);
              } else if (!strcmp(first->s,"letrec")) {
                if (length(args) < 2){
                  printf("Evaluation error: Invalid number of arguments. Expected at least 2, received %i\n", length(args));
                  texit(1);
                }
                result = evalLetRec(args, frame);
              } else {
                // If not a special form, evaluate the first, evaluate the args, then
                // apply the first to the args.
                Value *evaledOperator = eval(first, frame);
                if (evaledOperator->type != PRIMITIVE_TYPE && evaledOperator->type != CLOSURE_TYPE){
                  printf("Evaluation error: first argument in s-expression must be procedure. First argument was type %i \n", evaledOperator->type);
                  texit(1);
                }
                Value *evaledArgs = evalEach(args, frame);
                return apply(evaledOperator,evaledArgs);
              }
              return result;
              break;
            } default: {
              printf("Evaluation error in eval: invalid type %i\n", tree->type);
              texit(1);
            }
          }
          printf("Evaluation error \n");
          texit(1);
          return 0;
        }
