#include "RegAlloc.h"

namespace {
bool isArithOp(ILOC::Opcode op) {
    return op == ILOC::Opcode::ADD || op == ILOC::Opcode::SUB ||
           op == ILOC::Opcode::MULT || op == ILOC::Opcode::LSHIFT ||
           op == ILOC::Opcode::RSHIFT;
}

void addDefOperands(IRNode* node, std::vector<Operand*>& defs) {
    if(node->opcode == ILOC::Opcode::LOAD || node->opcode == ILOC::Opcode::LOADI ||
       isArithOp(node->opcode)) {
        defs.push_back(&node->op3);
    }
}

void addUseOperands(IRNode* node, std::vector<Operand*>& uses) {
    if(node->opcode == ILOC::Opcode::LOAD || node->opcode == ILOC::Opcode::STORE) {
        uses.push_back(&node->op1);
        uses.push_back(&node->op3);
        return;
    }

    if(isArithOp(node->opcode)) {
        uses.push_back(&node->op1);
        uses.push_back(&node->op2);
    }
}
} // namespace

// ----------------------
// Class Things
// ----------------------
RegAlloc::RegAlloc() : vrName(0) {}

RegAlloc::RegAlloc(int maxSR)
    : vrName(0), SRtoVR((maxSR + 1), -1),
      LU((maxSR + 1), INT_MAX) {}

RegAlloc::~RegAlloc() {}

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

        // just creating the list of defined and uses per IR node; this also handles
        // the per opcode caes
        addDefOperands(node, defs);
        addUseOperands(node, uses);

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
            LU[sr] = INT_MAX;
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
