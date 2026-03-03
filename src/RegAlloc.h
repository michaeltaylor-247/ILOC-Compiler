#pragma once

#include <vector>
#include <climits>

#include "IR.h"

class RegAlloc {
    private:
        int vrName;

        // Renaming Source Registers to Virtual
        std::vector<int> SRtoVR;    // SR @ i maps to VR at SRtoVR[i]
        std::vector<int> LU;        // SR @ i was last used at LU[i]

        // Allocating Physical Registers from Virtual
        std::vector<int> VRtoPR;
        std::vector<int> PRtoVR;
        std::vector<int> VRtoSpillLoc;  // VR spillover into RAM 
        std::vector<int> PRNU;          // Physical Reg's Next Use 
        int spillReg;
        

    public:
        RegAlloc();
        RegAlloc(int maxSR);
        ~RegAlloc();

        bool renameReg(IR& ir);
        void allocate();
        

    private:
        // Helpers for Rename 
        bool isArithOp(ILOC::Opcode op);
        void addDefOperands(IRNode* node, std::vector<Operand*>& defs);
        void addUseOperands(IRNode* node, std::vector<Operand*>& uses);

        // Helpers for Allocate
        int getPR();
};
