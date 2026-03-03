#include "RegAlloc.h"

// ----------------------
// Class Things
// ----------------------
RegAlloc::RegAlloc() : vrName(0) {}

RegAlloc::RegAlloc(int maxSR)
    : vrName(0), SRtoVR((maxSR + 1), -1),
      LU((maxSR + 1), INT_MIN) {}

RegAlloc::~RegAlloc() {}


// -------------------------
// Helper Functions
// -------------------------
bool RegAlloc::isArithOp(ILOC::Opcode op) {
    return op == ILOC::Opcode::ADD || op == ILOC::Opcode::SUB ||
           op == ILOC::Opcode::MULT || op == ILOC::Opcode::LSHIFT ||
           op == ILOC::Opcode::RSHIFT;
}

// The 3rd register for all opcodes except "store" is a defined register
// --> "store" actually has no defined registers
void RegAlloc::addDefOperands(IRNode* node, std::vector<Operand*>& defs) {
    if(node->opcode == ILOC::Opcode::LOAD || node->opcode == ILOC::Opcode::LOADI ||
       isArithOp(node->opcode)) {
        defs.push_back(&node->op3);
    }
}

// R1 and R2 for all opcodes are "used" registers... except the "store"
// opcode which "uses" both R1 and R3
void RegAlloc::addUseOperands(IRNode* node, std::vector<Operand*>& uses) {
    if(node->opcode == ILOC::Opcode::LOAD) {
        uses.push_back(&node->op1);
        return;
    }

    if(node->opcode == ILOC::Opcode::STORE) {
        uses.push_back(&node->op1);
        uses.push_back(&node->op3);
        return;
    }

    if(isArithOp(node->opcode)) {
        uses.push_back(&node->op1);
        uses.push_back(&node->op2);
    }
}

// ----------------------
// Functionality
// ----------------------
bool RegAlloc::renameReg(IR& ir) {
    // implemetn the pseudo code for renaming algorithm
    
    int index = ir.getOpCount();
    for(IRNode* node = ir.getTail(); node != nullptr; node = node->prev) {
        std::vector<Operand*> defs;
        std::vector<Operand*> uses;
        defs.reserve(1);
        uses.reserve(2);

        
        // Following the pseudo code, each IR Node should have a set of 
        // "Defined" and "used" registers.... ILOC is small and these will
        // always be less than 3 registers depending on the operation
        addDefOperands(node, defs);
        addUseOperands(node, uses);

        // The actual algorithm
        for(Operand* op : defs) {
            const int sr = op->SR;
            if(sr < 0 || static_cast<size_t>(sr) >= SRtoVR.size()) {
                continue;
            }

            if(SRtoVR[sr] == -1) {
                SRtoVR[sr] = vrName++;
            }
            op->VR = SRtoVR[sr];
            op->NU = LU[sr];

            SRtoVR[sr] = -1;
            LU[sr] = INT_MIN;
        }

        for(Operand* op : uses) {
            const int sr = op->SR;
            if(sr < 0 || static_cast<size_t>(sr) >= SRtoVR.size()) {
                continue;
            }

            if(SRtoVR[sr] == -1) {
                SRtoVR[sr] = vrName++;
            }
            op->VR = SRtoVR[sr];
            op->NU = LU[sr];
        }

        for(Operand* op : uses) {
            const int sr = op->SR;
            if(sr < 0 || static_cast<size_t>(sr) >= LU.size()) {
                continue;
            }
            LU[sr] = index;
        }

        index--;
    }

    return true;
}

// ----------------------------------------------------------

// Returns availabel PR; if none, returns PR whose NU is furthest in future
int RegAlloc::getPR() {

}

void RegAlloc::allocate() {
}

