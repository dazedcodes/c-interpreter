#include "value.h"
#include "tokenizer.h"
#include "linkedlist.h"
#include "talloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Initialize charRead as a global variable.
char charRead;
int charASCII;

// Checks if the subsequent is a legal input. Returns true or false.
int isSubsequentIllegal(mychar){
  int contains = 0;
  char subsequents[12] = "#(),;{}`[]'|";
  for (int i = 0; i < 12; i++){
    if (subsequents[i] == mychar){
      contains = 1;
      break;
    }
  } if (mychar == '"' || mychar == '\\'){
    contains = 1;
  }
  return contains;
}

// Checks if charRead is a valid symbol start. Returns contains as valid symbol.
int isSymbolStart(mychar){
  int contains = 0;
  char startString[18] = "!$%&*/:<>=?~-_^";
  for (int i =0; i < 17; i++){
    if (startString[i] == mychar){
      contains = 1;
      break;
    }
  }
  return contains;
}

// Checks if the starting number is a symbol or number and tokenizes it. Returns pointer to value with token type.
Value *tokenizeSymbol() {
  int acc = 0;
  char *tempStr = talloc(sizeof(char) * 300);
  Value *newCar = talloc(sizeof(Value));
  (*newCar).type = SYMBOL_TYPE;
  (*newCar).s = tempStr;
  while (charRead != ' ' && charRead != '\n' && charRead != ')' && charRead != '(' && charRead != EOF){
    if (isSubsequentIllegal(charRead)) {
      printf("Error: token invalid (symbol contains illegal character)");
      texit(1);
    }
    if (acc > 298) {
      *(tempStr + acc) = '\0';
      printf("Symbol character number is over 300! \n");
      texit(1);
    }
    *(tempStr + acc) = charRead;
    charRead = (char)fgetc(stdin);
    charASCII = (int)(charRead);
    acc++;
  }
  *(tempStr + acc) = '\0';
  return newCar;
}
// Tokenizes numbers. Returns pointer to value of type int or double, with number stored as .i or .d
Value *tokenizeNumber() {
  int isDouble = 0;
  if (charRead == '.'){
    isDouble = 1;
  }

  int acc = 1;
  char *tempStr = talloc(sizeof(char) * 11);
  char *endStr = talloc(sizeof(char));
  *tempStr = charRead;
  charRead = (char)fgetc(stdin);
  charASCII = (int)(charRead);

 // Checks if charRead is just a minus or plus symbol and logs it as a symbol if so.
  if ((charRead == ' ' || charRead == '\n' || charRead == EOF || charRead == ')') && (*tempStr == '-'|| *tempStr == '+') ) {
    Value *newCar = talloc(sizeof(Value));
    *(tempStr + 1) = '\0';
  	(*newCar).type = SYMBOL_TYPE;
  	(*newCar).s = tempStr;
  	return newCar;
  }
 // Label the value as double or interger.
  Value *newCar = talloc(sizeof(Value));
  while (charRead != ' ' && charRead != '\n' && charRead != ')' && charRead != '(' && charRead != EOF){
    *(tempStr + acc) = charRead;
    if ((charASCII > 47 ) && (charASCII < 58) ) {
      charRead = (char)fgetc(stdin);
      charASCII = (int)(charRead);
    } else if (charRead == '.') {
      if (isDouble) {
        printf("Error: Number cannot have two decimal points. \n");
        texit(1);
      } else {
        isDouble = 1;
        *(tempStr + acc) = charRead;
        charRead = (char)fgetc(stdin);
        charASCII = (int)(charRead);
      }
    } else {
      printf("Error: Invalid character in number: %c \n", charRead);
      texit(1);
    }
    acc++;
  }
  *(tempStr + acc) = '\0';
  endStr = tempStr + acc;

 // Add the value type of array of charRead.
    if (isDouble){
        (*newCar).d = (strtod(tempStr, &endStr));
        (*newCar).type = DOUBLE_TYPE;
    } else {
        (*newCar).i = (strtol(tempStr, &endStr, 10));
        (*newCar).type = INT_TYPE;
    }
    return newCar;
  }

// Read all of the input from stdin, and return a linked list consisting of the
// tokens.
Value *tokenize() {
// Initialize empty list
   Value *list = makeNull();

// Checks if charRead is one of the following cases.
   charRead = (char)fgetc(stdin);
   charASCII = (int)(charRead);
   while (charRead != EOF) {
     if (charRead == '(' ) {
       Value *newCar = talloc(sizeof(Value));
       (*newCar).type = OPEN_TYPE;
       list = cons(newCar, list);
       charRead = (char)fgetc(stdin);
     } else if (charRead == ')' ) {
       Value *newCar = talloc(sizeof(Value));
       (*newCar).type = CLOSE_TYPE;
       list = cons(newCar , list);
       charRead = (char)fgetc(stdin);
     } else if (charRead == '[') {
       Value *newCar = talloc(sizeof(Value));
       (*newCar).type = OPENBRACKET_TYPE;
       list = cons(newCar, list);
       charRead = (char)fgetc(stdin);
     } else if (charRead == ']') {
       Value *newCar = talloc(sizeof(Value));
       (*newCar).type = CLOSEBRACKET_TYPE;
       list = cons(newCar, list);
       charRead = (char)fgetc(stdin);
     } else if (charASCII == 39){
       Value *newCar = talloc(sizeof(Value));
       (*newCar).type = SINGLEQUOTE_TYPE;
       list = cons(newCar, list);
       charRead = (char)fgetc(stdin);
     } else if (charRead == '#'){
       char *mybool = talloc(sizeof(char));
       charRead = (char)fgetc(stdin);
       if (charRead == 'f') {
         Value *newCar = talloc(sizeof(Value));
         (*newCar).type = BOOL_TYPE;
         (*newCar).i = 0;
         list = cons(newCar , list);
       } else if (charRead == 't') {
         Value *newCar = talloc(sizeof(Value));
         (*newCar).type = BOOL_TYPE;
         (*newCar).i = 1;
         list = cons(newCar , list);
       } else {
         printf("Error: only #t and #f are valid boolean values");
         texit(1);
       }
       charRead = (char)fgetc(stdin);
     }
     // If charRead begins with ", insert following characters as a string until the final "
      else if (charRead =='"'){
        Value *newCar = talloc(sizeof(Value));
        (*newCar).type = STR_TYPE;
         char *string = talloc(300 * (sizeof(char)));
         charRead = (char)fgetc(stdin);
         int c = 0;
         while(charRead != '"' && c < 300){
           *(string + c) = charRead;
           charRead = (char)fgetc(stdin);
           c++;
         }
         *(string + c) = '\0';
         (*newCar).s = string;
         list = cons(newCar, list);
         charRead = (char)fgetc(stdin);
       }
     // If charRead is a number or valid start to a number, run tokenizeNumber().
       else if ((charASCII > 47) && (charASCII < 58)){
       list = cons(tokenizeNumber(), list);
     } else if (charRead == '+' || charRead == '-' || charRead == '.') {
       list = cons(tokenizeNumber(), list);
     }
     // If charRead is a space, check if followed by . and second space.
     // make DOT_TYPE token if so or move to next char if not
       else if(charRead == ' '){
         char nextChar = (char)fgetc(stdin);
         char afterNextChar = (char)fgetc(stdin);
         if (nextChar == '.' && afterNextChar == ' ') {
           Value *newCar = talloc(sizeof(Value));
           (*newCar).type = DOT_TYPE;
           list = cons(newCar, list);
           ungetc(afterNextChar, stdin);
         } else {
           ungetc(afterNextChar, stdin);
           ungetc(nextChar, stdin);
         }
         charRead = (char)fgetc(stdin);
         charASCII = (int)(charRead);
     } else if(charRead == ';'){
       while (charRead != '\n' && charRead != EOF){
         charRead = (char)fgetc(stdin);
       }
       charRead = (char)fgetc(stdin);
     } else if (charRead == '\n'){
       charRead = (char)fgetc(stdin);
     }
     // If charRead is a letter or valid symbol start, run the function tokenizeSymbol()
       else if (((charASCII > 64) && (charASCII < 91)) || ((charASCII > 96) && (charASCII < 123)) || (isSymbolStart(charRead))){
       list = cons(tokenizeSymbol(), list);
     } else {
       printf("Error: invalid character %c \n", charRead);
       texit(1);
     }
     charASCII = (int)(charRead);
   }
   Value *revList = reverse(list);
   return revList;
}
// Display the contents of the linked list as tokens, with type information
void displayTokens(Value *list){
  Value *currNode = list;
  int j = 0;
  switch ((*currNode).type) {
    case CONS_TYPE:
      displayTokens(car(currNode));
      displayTokens(cdr(currNode));
      break;
    case INT_TYPE:
      printf("%d: integer \n", (*currNode).i);
      break;
    case DOUBLE_TYPE:
      printf("%f: float \n", (*currNode).d);
      break;
    case STR_TYPE:
      printf("%s: string \n", (*currNode).s );
      break;
    case BOOL_TYPE:
      if ((*currNode).i == 0){
        printf("false: boolean");
      } else {
        printf("true: boolean");
      }
      break;
    case SINGLEQUOTE_TYPE:
      printf("': single quote \n");
      break;
    case OPEN_TYPE:
      printf("(: open \n");
      break;
    case CLOSE_TYPE:
      printf("): close \n");
      break;
    case OPENBRACKET_TYPE:
      printf("[: open bracket \n");
      break;
    case CLOSEBRACKET_TYPE:
      printf("]: close bracket  \n");
      break;
    case SYMBOL_TYPE:
      printf("%s: symbol \n", (*currNode).s );
      break;
    case DOT_TYPE:
      printf(".: dot\n");
      break;
    case NULL_TYPE:
      printf("\n");
      break;
    default:
      printf("Displaying something without a type. \n");
      break;
    }
  }