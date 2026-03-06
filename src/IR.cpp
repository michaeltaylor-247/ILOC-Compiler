#include "IR.h"

// -----------------
// class things
// -------------------
IR::IR() : head(nullptr), tail(nullptr), maxSR(-1), opCount(0) {}

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
    // Track maximum source register id only for register operands.
    switch(node->opcode) {
        case ILOC::Opcode::LOAD:
        case ILOC::Opcode::STORE:
            maxSR = std::max({maxSR, node->op1.SR, node->op3.SR});
            break;
        case ILOC::Opcode::LOADI:
            maxSR = std::max(maxSR, node->op3.SR);
            break;
        case ILOC::Opcode::ADD:
        case ILOC::Opcode::SUB:
        case ILOC::Opcode::MULT:
        case ILOC::Opcode::LSHIFT:
        case ILOC::Opcode::RSHIFT:
            maxSR = std::max({maxSR, node->op1.SR, node->op2.SR, node->op3.SR});
            break;
        default:
            break;
    }
    opCount++;
    return node;
}

IRNode* IR::insertBefore(IRNode* pos, IRNode* node) {
    if(pos == nullptr) {
        return appendIR(node);
    }

    node->next = pos;
    node->prev = pos->prev;

    if(pos->prev) {
        pos->prev->next = node;
    } else {
        head = node;
    }
    pos->prev = node;
    opCount++;
    return node;
}

IRNode* IR::getHead() { return head; }

IRNode* IR::getTail() { return tail; }

int IR::getMaxSR() { return maxSR; }

int IR::getOpCount() { return opCount; }

// --------------------
// Pretty Print IR
// --------------------
void IR::printOperand(Operand& op) {
    if (op.SR > -1) std::cout << std::setw(4) << op.SR; else std::cout << std::setw(4) << "-";
    if (op.VR > -1) std::cout << std::setw(4) << op.VR; else std::cout << std::setw(4) << "-";
    if (op.PR > -1) std::cout << std::setw(4) << op.PR; else std::cout << std::setw(4) << "-";
    if (op.NU > -1) std::cout << std::setw(4) << op.NU; else std::cout << std::setw(4) << "-";
    std::cout << " |";
}

// print the entire IR 
void IR::printIR() {
    // formatting....
    std::cout << "Line | Opcode |  SR  VR  PR  NU | SR  VR  PR  NU | SR  VR  PR  NU |\n";
    std::cout << "-------------------------------------------------------------------\n";
    
    for (IRNode* p = head; p != nullptr; p = p->next) {
        std::cout << std::setw(4) << p->line << " | "
            << std::setw(6) << ILOC::opcodeSpelling(static_cast<int>(p->opcode)) << " |"; 
        
        printOperand(p->op1);
        printOperand(p->op2);
        printOperand(p->op3);
        std::cout << "\n";
    }
}

void IR::printRenamedILOC() {
    for(IRNode* p = head; p != nullptr; p = p->next) {
        switch(p->opcode) {
            case ILOC::Opcode::LOAD:
                std::cout << "load r" << p->op1.VR << " => r" << p->op3.VR << "\n";
                break;
            case ILOC::Opcode::STORE:
                std::cout << "store r" << p->op1.VR << " => r" << p->op3.VR << "\n";
                break;
            case ILOC::Opcode::LOADI:
                std::cout << "loadI " << p->op1.SR << " => r" << p->op3.VR << "\n";
                break;
            case ILOC::Opcode::ADD:
                std::cout << "add r" << p->op1.VR << ", r" << p->op2.VR << " => r" << p->op3.VR << "\n";
                break;
            case ILOC::Opcode::SUB:
                std::cout << "sub r" << p->op1.VR << ", r" << p->op2.VR << " => r" << p->op3.VR << "\n";
                break;
            case ILOC::Opcode::MULT:
                std::cout << "mult r" << p->op1.VR << ", r" << p->op2.VR << " => r" << p->op3.VR << "\n";
                break;
            case ILOC::Opcode::LSHIFT:
                std::cout << "lshift r" << p->op1.VR << ", r" << p->op2.VR << " => r" << p->op3.VR << "\n";
                break;
            case ILOC::Opcode::RSHIFT:
                std::cout << "rshift r" << p->op1.VR << ", r" << p->op2.VR << " => r" << p->op3.VR << "\n";
                break;
            case ILOC::Opcode::OUTPUT:
                std::cout << "output " << p->op1.SR << "\n";
                break;
            case ILOC::Opcode::NOP:
                std::cout << "nop\n";
                break;
            default:
                break;
        }
    }
}
