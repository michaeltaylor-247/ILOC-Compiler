#pragma once

#include <cstdint>

/*
 * ILOC related things...
 *  - Syntactic Categories Enum
 *  - Opcode Enum
 *  - Opcode Enum <--> String Spelling
 *
*/
namespace ILOC {

// "Parts of Speech" (categories)
enum Category : uint8_t {
    MEMOP,      // load, store
    LOADI,      // loadi
    ARITHOP,    // add, sub, mult, lshift, rshift
    OUTPUT,     // output
    NOP,        // nop
    CONSTANT,   // 1, 2, 3...N
    REGISTER,   // r1, r2, r3, rN
    COMMA,      // ','
    INTO,       // =>
    ENDFILE,    // EOF
    ENDLINE,    // EOL
    INVALID     // for invalid tokens
};

// Determines the specific opcode within the category
enum class Opcode : uint32_t {
    LOAD,
    LOADI,
    STORE,
    ADD,
    SUB,
    MULT,
    LSHIFT,
    RSHIFT,
    OUTPUT,
    NOP,
    INVALID
};


// Mapping Category enumeration for a token to a string for printing
inline const char* categoryName(Category c) {
    switch(c) {
        case MEMOP:    return "MEMOP";
        case LOADI:    return "LOADI";
        case ARITHOP:  return "ARITHOP";
        case OUTPUT:   return "OUTPUT";
        case NOP:      return "NOP";
        case CONSTANT: return "CONSTANT";
        case REGISTER: return "REGISTER";
        case COMMA:    return "COMMA";
        case INTO:     return "INTO";
        case ENDFILE:  return "EOF";
        case ENDLINE:  return "EOL";
        case INVALID:  return "ERROR";
        default:       return "UNKNOWN";
    }
}

// Mapping Opcode category for a token's lexeme to a string for printing
inline const char* opcodeSpelling(uint32_t id) {
    switch(id) {
        case 0: return "load";
        case 1: return "loadI";
        case 2: return "store";
        case 3: return "add";
        case 4: return "sub";
        case 5: return "mult";
        case 6: return "lshift";
        case 7: return "rshift";
        case 8: return "output";
        case 9: return "nop";
        default: return "<?>";
    }
}

}; // ILOC namespace
