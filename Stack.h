/*
Our personal Stack class
*/

// MAKE SURE TO CHANGE TO FLOAT from signed integers
#ifndef Stack_h
#define Stack_h

#include "Arduino.h"

class Stack
{
  public:
    Stack();                 
    stack(uint8_t size);//
    void push(float operand);//
    float pop();//
    float peek();//
    boolean isFull();//
    boolean isEmpty();//


  private:
    int8_t _top;
    uint8_t _capacity;
    float _stack[];
};

#endif