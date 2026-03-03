#pragma once

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <limits.h>

#include "ILOC.h"

// For each operand
struct Operand {
    int SR = -1; 
    int VR = -1; 
    int PR = -1;
    int NU = -1;
};

// Actual IR Node for the linkedlist
struct IRNode {
    uint32_t line = 0;
    ILOC::Opcode opcode = ILOC::Opcode::INVALID;
    
    // Three generic operands to cover all ILOC instructions
    Operand op1; 
    Operand op2; 
    Operand op3; 

    // Doubly Linked List Pointers
    IRNode* prev = nullptr;
    IRNode* next = nullptr;
};
 
class IR {
    private:
        IRNode* head;
        IRNode* tail;
        
        int maxSR;
        int opCount;

    public:
        // Class Things
        IR();
        ~IR();

        // IR Methods
        IRNode* appendIR(IRNode* node);
        IRNode* getHead();
        IRNode* getTail();
        int getMaxSR();
        int getOpCount();

        // Printing 
        void printOperand(Operand& op);
        void printIR();
        void printRenamedILOC();
};
