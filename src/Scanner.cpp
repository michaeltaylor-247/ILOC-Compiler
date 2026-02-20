#include "Scanner.h"

Scanner::Scanner(std::string filename) : line(""), lineNumber(0), pos(0), eofDetected(false) {
    // Open file on the member stream so scanning uses the same handle.
    file.open(filename);

    if(!file) {
        std::cerr << "ERROR: Scanner couldn't open file\n";
        exit(1);
    }

    // Fill the buffer for intial read
    refillBuffer();
}

Scanner::~Scanner() {}

// ----------------------
// Helper Functions
// ----------------------
bool Scanner::refillBuffer() {
    // if getline fails, we've reached eof
    if(!std::getline(file, line)) {
        return false;
    }
    lineNumber++;
    return true;
}

// looks ahead in the buffer
char Scanner::peek() {
    return (pos >= line.size()) ? '\0' : line[pos];
}

// consumes a single character
char Scanner::consume() {
    return (pos >= line.size()) ? '\0' : line[pos++];
}

void Scanner::skipWhiteSpace() {
    // Checks also for the \r on windows
    while(pos < line.size() && (line[pos] == ' ' || line[pos] == '\t' || line[pos] == '\r')) pos++;
}

// ----------------------
// Token Category Processing
// ----------------------
void Scanner::handleComma(Token& token) {
    token.category = ILOC::Category::COMMA;
    token.lexeme = 0;
    token.lineNumber = lineNumber;
}

void Scanner::handleInto(Token& token) {
    char c = peek();
    if(c == '>') {
        consume(); // consume '>'
        token.category = ILOC::Category::INTO;
    }
    else {
        token.category = ILOC::Category::INVALID;
    }
    token.lexeme = 0;
    token.lineNumber = lineNumber;
}

void Scanner::handleComment(Token& token) {
    if(peek() == '/') {
        pos = line.size(); 
        token.category = ILOC::Category::ENDLINE;
    }
    else {
        token.category = ILOC::Category::INVALID;
    }
    token.lexeme = 0;
    token.lineNumber = lineNumber;
}

void Scanner::handleEOL(Token& token) {
    token.category = ILOC::Category::ENDLINE;
    token.lexeme = 0;
    token.lineNumber = lineNumber;
}

void Scanner::handleConstant(Token& token, char c) {
    token.category = ILOC::Category::CONSTANT;
    token.lineNumber = lineNumber;

    // Calculate value directly without string
    int num = c - '0';
    while(std::isdigit(peek())) {
        num = num * 10 + (consume() - '0');
    }
    token.lexeme = num;
}

void Scanner::handleWord(Token& token, char c) {
    token.lineNumber = lineNumber;
    token.lexeme = 0;

    // register: r[0-9]+
    if(c == 'r' && std::isdigit(peek())) {
        handleRegister(token);
        return;
    }

    // default to an invliad state unless we actually gfind a vliad match
    token.category = ILOC::Category::INVALID;

    // load / loadi
    if(c == 'l') {
        if(peek() == 'o') {
            consume(); // o
            if(peek() == 'a') {
                consume(); // a
                if(peek() == 'd') {
                    consume(); // d

                    // confirm its loadi
                    if(peek() == 'I') {
                        consume(); // i
                        token.category = ILOC::Category::LOADI;
                        token.lexeme = static_cast<uint32_t>(ILOC::Opcode::LOADI);
                        return;
                    }
                    // load
                    token.category = ILOC::Category::MEMOP;
                    token.lexeme = static_cast<uint32_t>(ILOC::Opcode::LOAD);
                    return;
                }
            }
        }
        else if(peek() == 's') {
            // lshift
            consume(); // s
            if(peek() == 'h') {
                consume(); // h
                if(peek() == 'i') {
                    consume(); // i
                    if(peek() == 'f') {
                        consume(); // f
                        if(peek() == 't') {
                            consume(); // t
                            token.category = ILOC::Category::ARITHOP;
                            token.lexeme = static_cast<uint32_t>(ILOC::Opcode::LSHIFT);
                            return;
                        }
                    }
                }
            }
        }
    }

    // store / sub
    if(c == 's') {
        if(peek() == 't') {
            // store
            consume(); // t
            if(peek() == 'o') {
                consume(); // o
                if(peek() == 'r') {
                    consume(); // r
                    if(peek() == 'e') {
                        consume(); // e
                        token.category = ILOC::Category::MEMOP;
                        token.lexeme = static_cast<uint32_t>(ILOC::Opcode::STORE);
                        return;
                    }
                }
            }
        }
        else if(peek() == 'u') {
            // sub
            consume(); // u
            if(peek() == 'b') {
                consume(); // b
                token.category = ILOC::Category::ARITHOP;
                token.lexeme = static_cast<uint32_t>(ILOC::Opcode::SUB);
                return;
            }
        }
    }

    // add
    if(c == 'a') {
        if(peek() == 'd') {
            consume(); // d
            if(peek() == 'd') {
                consume(); // d
                token.category = ILOC::Category::ARITHOP;
                token.lexeme = static_cast<uint32_t>(ILOC::Opcode::ADD);
                return;
            }
        }
    }

    // mult
    if(c == 'm') {
        if(peek() == 'u') {
            consume(); // u
            if(peek() == 'l') {
                consume(); // l
                if(peek() == 't') {
                    consume(); // t
                    token.category = ILOC::Category::ARITHOP;
                    token.lexeme = static_cast<uint32_t>(ILOC::Opcode::MULT);
                    return;
                }
            }
        }
    }

    // rshift  (note: register handled earlier by "r" + digit)
    if(c == 'r') {
        if(peek() == 's') {
            consume(); // s
            if(peek() == 'h') {
                consume(); // h
                if(peek() == 'i') {
                    consume(); // i
                    if(peek() == 'f') {
                        consume(); // f
                        if(peek() == 't') {
                            consume(); // t
                            token.category = ILOC::Category::ARITHOP;
                            token.lexeme = static_cast<uint32_t>(ILOC::Opcode::RSHIFT);
                            return;
                        }
                    }
                }
            }
        }
    }

    // output
    if(c == 'o') {
        if(peek() == 'u') {
            consume(); // u
            if(peek() == 't') {
                consume(); // t
                if(peek() == 'p') {
                    consume(); // p
                    if(peek() == 'u') {
                        consume(); // u
                        if(peek() == 't') {
                            consume(); // t
                            token.category = ILOC::Category::OUTPUT;
                            token.lexeme = static_cast<uint32_t>(ILOC::Opcode::OUTPUT);
                            return;
                        }
                    }
                }
            }
        }
    }

    // nop
    if(c == 'n') {
        if(peek() == 'o') {
            consume(); // o
            if(peek() == 'p') {
                consume(); // p
                token.category = ILOC::Category::NOP;
                token.lexeme = static_cast<uint32_t>(ILOC::Opcode::NOP);
                return;
            }
        }
    }

    while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') {
        consume();
    }

    token.category = ILOC::Category::INVALID;
    token.lexeme = 0;
}

void Scanner::handleRegister(Token& token) {
    // already consumed the 'r'...

    // check if next char is a digit
    if(!std::isdigit(peek())) {
        token.category = ILOC::Category::INVALID;
        token.lexeme = 0;
        token.lineNumber = lineNumber;
        return;
    }

    // First Digit
    int num = consume() - '0';
    while(std::isdigit(peek())) {
        num = num * 10 + (consume() - '0');
    }

    token.category = ILOC::Category::REGISTER;
    token.lexeme = num;
    token.lineNumber = lineNumber;
}


// ----------------------
// Parser Interface
// ----------------------
Token Scanner::getToken() {
    Token token;

    skipWhiteSpace();

    // Need to refill buffer if current pos is past line
    if (pos >= line.size()) {
        
        // if getline() in refill buffer fails, it indicates eof
        if (!refillBuffer()) { 
            token.category = ILOC::Category::ENDFILE;
            token.lineNumber = lineNumber; 
            return token;
        }

        pos = 0;
        token.category = ILOC::Category::ENDLINE;
        token.lineNumber = lineNumber - 1; // The line that just finished
        return token;
    }


    // Actually process the characters
    char c = consume();
    if (false);
    else if (c == ',')           handleComma(token);
    else if (c == '/') {
        
        // A comment should be skipped altogether --> doesn't even generate a token as there is no
        // semantic meaning.
        //
        // So.... set the category to something to signal its not the default cateogry
        // then check for the signal and make a recurse call the getToken() to just move on
        handleComment(token);
        if (token.category == ILOC::Category::ENDLINE) {
            return getToken();
        }
    }        
    else if (c == '=')           handleInto(token);
    else if (c == '\0')          handleEOL(token);
    else if (std::isdigit(c))    handleConstant(token, c);
    else if (std::isalpha(c))    handleWord(token, c);
    else {
        token.category = ILOC::Category::INVALID;
        token.lineNumber = lineNumber;
        token.lexeme = c; 
    }
    
    return token;
}
