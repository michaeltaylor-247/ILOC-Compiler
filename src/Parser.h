#pragma once
#include <iostream>
#include <vector>

#include "ILOC.h"
#include "IR.h"
#include "Scanner.h"

class Parser {
    private:
        bool hadError = false; // simple flag to mark invalid iloc program

        // Non-owning phase inputs (set during parse call)
        Scanner* scanner_ = nullptr;
        IR* ir_ = nullptr;

        // Context for the line currently being parsed
        uint32_t currentLine = 0;
        ILOC::Opcode currentOpcode = ILOC::Opcode::INVALID;

        // heleprs
        void skipToEOL();
        
        // Grammar Rule specific parsing
        void parseMemop();   // load, store
        void parseLoadI();   // loadI
        void parseArithop(); // add, sub, mult, lshift, rshift
        void parseOutput();  // output
        void parseNop();     // nop

    public:
        Parser();
        ~Parser();

        bool parse(Scanner& scanner, IR& ir);
};
