#pragma once

#include "IR.h"

class RegAlloc {
    private:
        // peep the pseduo code for data members
        
    public:
        RegAlloc();
        ~RegAlloc();

        bool renameReg(IR& ir);
};
