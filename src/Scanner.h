#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <cctype> 

#include "CLI.h"
#include "ILOC.h"

/**
 * Token Object to produce by the scanner
 *  - Uses ILOC:: objects
*/
struct Token {
    ILOC::Category category;    // syntactic cateogry
    uint32_t lexeme = 0;        // lexeme (specifies the actual word)
    uint32_t lineNumber = 0;    // line number
};

class Scanner {
    private:
        // Data Members
        std::ifstream file;
        std::string line;       // "buffer"
        uint32_t lineNumber;
        size_t pos;
        bool hasLine;

        // Helpers
        char peek();
        char consume();
        void skipWhiteSpace();
        bool refillBuffer();

        // EOF flags
        bool eofDetected;

        // Per Word Protocol
        void handleComma(Token& token);
        void handleRegister(Token& token);
        void handleInto(Token& token);
        void handleComment(Token& token);
        void handleEOL(Token& token);
        void handleConstant(Token& token, char c);

        void handleWord(Token& token, char c);


    public:
        Scanner(std::string filename);
        ~Scanner(); 

        // Interface for Parser
        Token getToken();
};
