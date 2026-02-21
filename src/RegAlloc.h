#pragma once

#include <vector>
#include <climits>

#include "IR.h"

class RegAlloc {
    private:
        int vrName;

        // The SR specified at the index, i, maps to the VR at SRtoVR[i]
        std::vector<int> SRtoVR;

        // The SR specified at the index,i , was Last used at the LU[i]
        std::vector<int> LU;
        

    public:
        RegAlloc();
        RegAlloc(int maxSR);
        ~RegAlloc();

        bool renameReg(IR& ir);
};
