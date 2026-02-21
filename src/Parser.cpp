#include "Parser.h"

Parser::Parser() {}

Parser::~Parser() {}

// ----------------
// Parser Helpers
// -----------------
void Parser::skipToEOL() {
    Token t;
    do {
        t = scanner_->getToken();
    } while(t.category != ILOC::Category::ENDLINE && t.category != ILOC::Category::ENDFILE);
}

// ------------------------------------------------------------
// Instruction Parsers
// ------------------------------------------------------------
void Parser::parseMemop() {
    Token t1 = scanner_->getToken();
    if(t1.category != ILOC::Category::REGISTER) {
        std::cerr << "ERROR " << currentLine << ": expected register after memop\n";
        hadError = true; skipToEOL(); return;
    }

    Token into = scanner_->getToken();
    if(into.category != ILOC::Category::INTO) {
        std::cerr << "ERROR " << currentLine << ": expected '=>' in memop\n";
        hadError = true; skipToEOL(); return;
    }

    Token t2 = scanner_->getToken();
    if(t2.category != ILOC::Category::REGISTER) {
        std::cerr << "ERROR " << currentLine << ": expected target register after '=>'\n";
        hadError = true; skipToEOL(); return;
    }

    // IR Construction
    IRNode* node = new IRNode();
    node->line = currentLine;
    node->opcode = currentOpcode;
    
    node->op1.SR = (int)t1.lexeme; // Source
    node->op3.SR = (int)t2.lexeme; // Target

    ir_->appendIR(node);
}

void Parser::parseLoadI() {
    Token val = scanner_->getToken();
    if(val.category != ILOC::Category::CONSTANT) {
        std::cerr << "ERROR " << currentLine << ": expected constant for loadI\n";
        hadError = true; skipToEOL(); return;
    }

    if(scanner_->getToken().category != ILOC::Category::INTO) {
        std::cerr << "ERROR " << currentLine << ": missing '=>'\n";
        hadError = true; skipToEOL(); return;
    }

    Token dest = scanner_->getToken();
    if(dest.category != ILOC::Category::REGISTER) {
        std::cerr << "ERROR " << currentLine << ": expected destination register\n";
        hadError = true; skipToEOL(); return;
    }

    // build IR
    IRNode* node = new IRNode();
    node->line = currentLine;
    node->opcode = currentOpcode;
    node->op1.SR = (int)val.lexeme; // The Constant
    node->op3.SR = (int)dest.lexeme; // The Dest Register

    ir_->appendIR(node);
}

void Parser::parseArithop() {
    Token r1 = scanner_->getToken();
    if(r1.category != ILOC::Category::REGISTER) {
        std::cerr << "ERROR " << currentLine << ": expected source register 1\n";
        hadError = true; skipToEOL(); return;
    }

    if(scanner_->getToken().category != ILOC::Category::COMMA) {
        std::cerr << "ERROR " << currentLine << ": expected ','\n";
        hadError = true; skipToEOL(); return;
    }

    Token r2 = scanner_->getToken();
    if(r2.category != ILOC::Category::REGISTER) {
        std::cerr << "ERROR " << currentLine << ": expected source register 2\n";
        hadError = true; skipToEOL(); return;
    }

    if(scanner_->getToken().category != ILOC::Category::INTO) {
        std::cerr << "ERROR " << currentLine << ": expected '=>'\n";
        hadError = true; skipToEOL(); return;
    }

    Token r3 = scanner_->getToken();
    if(r3.category != ILOC::Category::REGISTER) {
        std::cerr << "ERROR " << currentLine << ": expected destination register\n";
        hadError = true; skipToEOL(); return;
    }

    // build IR 
    IRNode* node = new IRNode();
    node->line = currentLine;
    node->opcode = currentOpcode;
    node->op1.SR = (int)r1.lexeme;
    node->op2.SR = (int)r2.lexeme;
    node->op3.SR = (int)r3.lexeme;

    ir_->appendIR(node);
}

void Parser::parseOutput() {
    
    Token val = scanner_->getToken();
    if(val.category != ILOC::Category::CONSTANT) {
        std::cerr << "ERROR " << currentLine << ": expected constant for std::coutput\n";
        hadError = true; skipToEOL(); return;
    }

    // Build IR
    IRNode* node = new IRNode();
    node->line = currentLine;
    node->opcode = currentOpcode;
    node->op1.SR = (int)val.lexeme;

    ir_->appendIR(node);
}

void Parser::parseNop() {
    // Add to IR
    IRNode* node = new IRNode();
    node->line = currentLine;
    node->opcode = currentOpcode;
    ir_->appendIR(node);
}

// ------------------------------------------------------------
// The main parse
// ------------------------------------------------------------
bool Parser::parse(Scanner& scanner, IR& ir) {
    scanner_ = &scanner;
    ir_ = &ir;

    // assume iloc program is ok until we get an error
    hadError = false;
    Token token = scanner_->getToken();

    while(token.category != ILOC::Category::ENDFILE) {
        
        // Skip till an actual token with semantic meaning
        if(token.category == ILOC::Category::ENDLINE) {
            token = scanner_->getToken();
            continue;
        }

        currentLine = token.lineNumber;
        currentOpcode = static_cast<ILOC::Opcode>(token.lexeme);

        // Switch on first token's cateogory and evaluate the rest of expression as the grammar defines
        switch(token.category) {
            case ILOC::MEMOP:     parseMemop();   break;
            case ILOC::LOADI:     parseLoadI();   break;
            case ILOC::ARITHOP:   parseArithop(); break;
            case ILOC::OUTPUT:    parseOutput();  break;
            case ILOC::NOP:       parseNop();     break;
            default:
                std::cerr << "ERROR " << currentLine << ": expected opcode at start of line\n";
                hadError = true;
                skipToEOL();
                break;
        }

        // consume the ENDLINE token that follows each expression...
        Token check = scanner_->getToken();

        // The "parse category" methods dispatched above might not exahust the full line's worth
        // of tokens... --> the line has extra tokens outside of the grammar rule
        if(check.category != ILOC::Category::ENDLINE && check.category != ILOC::Category::ENDFILE) {

            // grammar rule might've be correct, but extra stuff after it
             if(!hadError) {
                 std::cerr << "ERROR " << currentLine << ": extra tokens at end of line\n";
                 hadError = true;
                 skipToEOL();
                 token = scanner_->getToken();     // Next line
             } 
             else {
                 token = check;     // proceed
             }
        } 
        else {
            // get the next token for the next ieration
            token = scanner_->getToken();
        }
    }

    scanner_ = nullptr;
    ir_ = nullptr;
    return !hadError;
}

