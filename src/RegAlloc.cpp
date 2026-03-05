#include "RegAlloc.h

// ----------------------
// Class Things
// ----------------------
RegAlloc::RegAlloc() : vrName(0) {}

RegAlloc::RegAlloc(int maxSR, int numReg) : 
    vrName(0), SRtoVR((maxSR + 1), -1), LU((maxSR + 1),-1), 
    k(numReg), maxLive(0), VRtoPR(0), PRtoVR(0), VRtoSpillLoc(0), PRNU(0) {}

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
            LU[sr] = -1;
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
}

// ----------------------------------------------------------

// Returns availabel PR; if none, returns PR whose NU is furthest in future
int RegAlloc::getPR() {
    // Can use all k registers
    if(maxLive <= k) {
        for(int i = 0; i < k; i++) {
            if(PRtoVR[i] == -1) return i;
        }
    }
    // Must reserver a register for spill
    else {
        // free "furthest" next used PR -- idk say pry
        //  - Linear Scan across PRNU[]? to find max()
        int spillReg = std::max(PRNU.begin(), PRNU.end());

        spill(spillReg); // --> store memory somewhere
    }
}

spill() {
    // loadl -- to load the spill location address
    // store -- to store the to be spilled value at the specifed address
    //
    //
    // IF LOADI is what is spilling, you don't need to "spill it", simply
    // "rematerialize it"... however you can't possibly know this (NP-complete)
    

    // Where we handle the insertion of the spill code
}

restore() {
    // loadI -- to put the spill locatio into address
    // store -- to retrieve the spilled value to its PR


    // Insert restore code
} 


void RegAlloc::allocate(IR& ir) {
    // Rename
    renameReg(ir);

    // Set up Allocation Structures (now that maxVR is known post renaming)
    VRtoPR.resize(vrName, -1);
    PRtoVR.resize(k, -1);
    VRtoSpillLoc.resize(vrName, -1);
    PRNU.resize(k, -1); 

    // Alloc
    for(IRNode* node = ir.getHead(); node != nullptr; node = node->next) {
        std::vector<Operand*> defs;
        std::vector<Operand*> uses;
        defs.reserve(1);
        uses.reserve(2);

        addDefOperands(node, defs);
        addUseOperands(node, uses);

        // Allocate PR if needed; maintain mappings
        for(Operand* op : uses) {
            if(VRtoPR[op->VR] == -1) {
                int pr = getPR();
                op->PR = pr;
                VRtoPR[op->VR] = pr;
                PRtoVR[pr] = op->VR;
                PRNU[pr] = op->NU;
            }
        }
        // Free a "use" operand if its the last use of it -- thereby freeing the PR associated with it
        for(Operand* op : uses) {
            if(op->NU == -1) {
                PRNU[VRtoPR[op->PR]] = -1;
                op->PR = -1;
                PRtoVR[op->VR] = -1;
                VRtoPR[op->VR] = -1;
            }
        }


        // For definitions, allocate a PR
        for(Operand* op : defs) {
            int pr = getPR();
            op->PR = pr;
            VRtoPR[op->VR] = pr;
            PRtoVR[pr] = op->VR;
            PRNU[pr] = op->NU;
        }
    }

}

