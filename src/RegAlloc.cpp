#include "RegAlloc.h"

// ----------------------
// Class Things
// ----------------------
RegAlloc::RegAlloc() : vrName(0) {}

RegAlloc::RegAlloc(int maxSR, int numReg) : 
    vrName(0), SRtoVR((maxSR + 1), -1), LU((maxSR + 1),-1), 
    k(numReg), maxLive(0), VRtoPR(0), PRtoVR(0), VRtoSpillLoc(0), PRNU(0),
    spillReg(-1), nextSpillLoc(32768) {}

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
void RegAlloc::renameReg(IR& ir) {
    // implement the pseudo code for renaming algorithm
    int liveNow = 0;
    maxLive = 0;
    
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
                liveNow++;
                maxLive = std::max(maxLive, liveNow);
            }
            op->VR = SRtoVR[sr];
            op->NU = LU[sr];

            SRtoVR[sr] = -1;
            LU[sr] = -1;
            liveNow--;
        }

        for(Operand* op : uses) {
            const int sr = op->SR;
            if(sr < 0 || static_cast<size_t>(sr) >= SRtoVR.size()) {
                continue;
            }

            if(SRtoVR[sr] == -1) {
                SRtoVR[sr] = vrName++;
                liveNow++;
                maxLive = std::max(maxLive, liveNow);
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
}

// ----------------------------------------------------------

// Returns availabel PR; if none, returns PR whose NU is furthest in future
int RegAlloc::getPR(IR& ir, IRNode* at, const std::vector<char>& blockedPR) {
    const bool reserveSpillReg = (maxLive > k);
    const int allocLimit = reserveSpillReg ? (k - 1) : k;

    for(int i = 0; i < allocLimit; i++) {
        if(i < (int)blockedPR.size() && blockedPR[i]) continue;
        if(PRtoVR[i] == -1) return i;
    }

    int victimPR = -1;
    int farthestNU = -1;
    for(int i = 0; i < allocLimit; i++) {
        if(i < (int)blockedPR.size() && blockedPR[i]) continue;
        const int nuScore = (PRNU[i] == -1) ? INT_MAX : PRNU[i];
        if(nuScore > farthestNU) {
            farthestNU = nuScore;
            victimPR = i;
        }
    }

    if(victimPR >= 0) {
        spill(ir, at, victimPR);
        return victimPR;
    }

    return 0;
}

void RegAlloc::spill(IR& ir, IRNode* at, int pr) {
    if(pr < 0 || pr >= (int)PRtoVR.size()) return;

    const int vr = PRtoVR[pr];
    if(vr < 0) return;
    if(spillReg < 0) return;

    if(vr >= 0 && vr < (int)VRtoSpillLoc.size() && VRtoSpillLoc[vr] == -1) {
        VRtoSpillLoc[vr] = nextSpillLoc;
        nextSpillLoc += 4;
    }

    IRNode* loadAddr = new IRNode();
    loadAddr->line = at ? at->line : 0;
    loadAddr->opcode = ILOC::Opcode::LOADI;
    loadAddr->op1.SR = VRtoSpillLoc[vr];
    loadAddr->op3.PR = spillReg;
    ir.insertBefore(at, loadAddr);

    IRNode* storeVal = new IRNode();
    storeVal->line = at ? at->line : 0;
    storeVal->opcode = ILOC::Opcode::STORE;
    storeVal->op1.PR = pr;
    storeVal->op3.PR = spillReg;
    ir.insertBefore(at, storeVal);

    VRtoPR[vr] = -1;
    PRtoVR[pr] = -1;
    PRNU[pr] = -1;
}

void RegAlloc::restore(IR& ir, IRNode* at, int vr, int pr) {
    if(vr < 0 || vr >= (int)VRtoPR.size()) return;
    if(pr < 0 || pr >= (int)PRtoVR.size()) return;
    if(spillReg < 0) return;

    const int spillLoc = VRtoSpillLoc[vr];
    if(spillLoc < 0) return;

    IRNode* loadAddr = new IRNode();
    loadAddr->line = at ? at->line : 0;
    loadAddr->opcode = ILOC::Opcode::LOADI;
    loadAddr->op1.SR = spillLoc;
    loadAddr->op3.PR = spillReg;
    ir.insertBefore(at, loadAddr);

    IRNode* loadVal = new IRNode();
    loadVal->line = at ? at->line : 0;
    loadVal->opcode = ILOC::Opcode::LOAD;
    loadVal->op1.PR = spillReg;
    loadVal->op3.PR = pr;
    ir.insertBefore(at, loadVal);

    VRtoPR[vr] = pr;
    PRtoVR[pr] = vr;
}


void RegAlloc::allocate(IR& ir) {
    // Rename
    renameReg(ir);

    // Set up Allocation Structures (now that maxVR is known post renaming)
    VRtoPR.resize(vrName, -1);
    PRtoVR.resize(k, -1);
    VRtoSpillLoc.resize(vrName, -1);
    PRNU.resize(k, -1); 
    spillReg = (maxLive > k) ? (k - 1) : -1;

    // Alloc
    for(IRNode* node = ir.getHead(); node != nullptr; node = node->next) {
        std::vector<Operand*> defs;
        std::vector<Operand*> uses;
        defs.reserve(1);
        uses.reserve(2);
        std::vector<char> blockedPR(k, 0);

        addDefOperands(node, defs);
        addUseOperands(node, uses);

        // Allocate PR if needed; maintain mappings
        for(Operand* op : uses) {
            if(op->VR < 0) continue;
            if(VRtoPR[op->VR] == -1) {
                int pr = getPR(ir, node, blockedPR);
                if(VRtoSpillLoc[op->VR] != -1) {
                    restore(ir, node, op->VR, pr);
                } else {
                    VRtoPR[op->VR] = pr;
                    PRtoVR[pr] = op->VR;
                }
                op->PR = pr;
                PRNU[pr] = op->NU;
                blockedPR[pr] = 1;
            } else {
                op->PR = VRtoPR[op->VR];
                PRNU[op->PR] = op->NU;
                blockedPR[op->PR] = 1;
            }
        }
        // Free a "use" operand if its the last use of it -- thereby freeing the PR associated with it
        for(Operand* op : uses) {
            if(op->NU == -1) {
                if(op->PR < 0) continue;
                const int pr = op->PR;
                PRNU[pr] = -1;
                PRtoVR[pr] = -1;
                VRtoPR[op->VR] = -1;
            }
        }


        // For definitions, allocate a PR
        for(Operand* op : defs) {
            if(op->VR < 0) continue;
            int pr = getPR(ir, node, blockedPR);
            op->PR = pr;
            VRtoPR[op->VR] = pr;
            PRtoVR[pr] = op->VR;
            PRNU[pr] = op->NU;
        }
    }

}
