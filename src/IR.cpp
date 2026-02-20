#include "IR.h"

// -------------
// class things
// -------------
IR::IR() : head(nullptr), tail(nullptr), opCount(0) {}

IR::~IR() {
    IRNode* p = head;
    while(p) {
        IRNode* nxt = p->next;
        delete p;
        p = nxt;
    }
    head = tail = nullptr;
    opCount = 0;
}

// -------------
// IR Methods
// -------------
IRNode* IR::appendIR(IRNode* node) {
    if(!head) {
        head = tail = node;
    } 
    else {
        tail->next = node;
        node->prev = tail;
        tail = node;
    }
    opCount++;
    return node;
}

// --------------------
// Pretty Print IR
// --------------------
void IR::printOperand(Operand& op) {
    if (op.SR != -1) std::cout << std::setw(4) << op.SR; else std::cout << std::setw(4) << "-";
    if (op.VR != -1) std::cout << std::setw(4) << op.VR; else std::cout << std::setw(4) << "-";
    if (op.PR != -1) std::cout << std::setw(4) << op.PR; else std::cout << std::setw(4) << "-";
    if (op.NU != -1) std::cout << std::setw(4) << op.NU; else std::cout << std::setw(4) << "-";
    std::cout << " |";
}

// print the entire IR 
void IR::printIR() {
    // formatting....
    std::cout << "Line | Opcode | SR  VR  PR  NU | SR  VR  PR  NU | SR  VR  PR  NU |\n";
    std::cout << "------------------------------------------------------------------\n";
    
    // TODO: i probably should move the opcode enumeration to string table from main to the scanner or just some other
    // namespace so that i cna access it here... 
    for (IRNode* p = head; p != nullptr; p = p->next) {
        std::cout << std::setw(4) << p->line << " | "
            << std::setw(6) << ILOC::opcodeSpelling(static_cast<int>(p->opcode)) << " |"; 
        
        printOperand(p->op1);
        printOperand(p->op2);
        printOperand(p->op3);
        std::cout << "\n";
    }
}


