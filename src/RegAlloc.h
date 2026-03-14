#pragma once

#include <vector>
#include <climits>

#include "IR.h"

class RegAlloc {
    private:
        // Renaming Source Registers to Virtual
        int vrName;
        std::vector<int> SRtoVR;    // SR @ i maps to VR at SRtoVR[i]
        std::vector<int> LU;        // SR @ i was last used at LU[i]

        // Allocating Physical Registers from Virtual
        int k;
        int maxLive;
        std::vector<int> VRtoPR;
        std::vector<int> PRtoVR;
        std::vector<int> VRtoSpillLoc;  // VR spillover into RAM 
        std::vector<int> VRRematValue;  // Constant value for rematerializable VRs
        std::vector<int> PRNU;          // Physical Reg's Next Use 
        std::vector<char> VRHasSpillCopy;
        std::vector<char> VRIsRematerializable;
        int spillReg;
        int nextSpillLoc;
        

    public:
        RegAlloc();
        RegAlloc(int maxSR, int k);
        ~RegAlloc();

        void renameReg(IR& ir);
        void allocate(IR& ir);
        

    private:
        // Helpers for Rename 
        bool isArithOp(ILOC::Opcode op);
        void addDefOperands(IRNode* node, std::vector<Operand*>& defs);
        void addUseOperands(IRNode* node, std::vector<Operand*>& uses);

        // Helpers for Allocate
        int getPR(IR& ir, IRNode* at, const std::vector<char>& blockedPR);
        int getVictimClass(int pr);
        void spill(IR& ir, IRNode* at, int pr);
        void restore(IR& ir, IRNode* at, int vr, int pr);
        void rematerialize(IR& ir, IRNode* at, int vr, int pr);
};
