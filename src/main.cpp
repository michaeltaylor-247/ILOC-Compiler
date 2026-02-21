#include <iostream>
#include <fstream>

#include "CLI.h"
#include "ILOC.h"
#include "Scanner.h"
#include "Parser.h"
#include "RegAlloc.h"

int main(int argc, char* argv[]) {
    cli::Options options = cli::parseArgs(argc, argv);

    // Check if cmd args were valid or help
    switch(options.mode) {
        case cli::Mode::Invalid:
            std::cerr << options.error << "\n";
            cli::help();
            return 1;

        case cli::Mode::Help:
            cli::help();
            return 0;

        default:
            break;
    };

    // Initialize Compiler Components
    Scanner scanner(options.filename);
    Parser parser;


    // IR Init
    IR ir;
    


    // TODO: continue refacatoring of the pipeline....
    //  - manage to decouple everything so that main could stay fairly unchanged...
    //  - Next i think main should be changed to more elegantly orchestrate the pipelien
    //
    // Do compiler based on flag passed...
    // From part 1 (only frontend):
    //  -h : print usage
    //  -s : read file and print list of tokens (only scan)
    //  -p : read file, scan & parse, build IR, print errors if any
    //  -r : read file, scan & paruse, build IR, print IR
    //  no commands specified : behavir as -p
    //
    //  From part2 (frontend + new backend):
    //  -x : rename
    //  k : TODO
    //

    // TODO:
    // Refactor this to operate more as a pipeline of functionality....
    // --> Depending on the flag passed, do
    if(options.mode == cli::Mode::Scan) {
        while(true) {
            Token t = scanner.getToken();

            std::cout << t.lineNumber << " " << ILOC::categoryName(t.category) << " ";

            switch(t.category) {
                case ILOC::MEMOP:
                case ILOC::LOADI:
                case ILOC::ARITHOP:
                case ILOC::OUTPUT:
                case ILOC::NOP:
                    std::cout << ILOC::opcodeSpelling(t.lexeme);
                    break;
                case ILOC::REGISTER:
                    std::cout << "r" << t.lexeme;
                    break;
                case ILOC::CONSTANT:
                    std::cout << t.lexeme;
                    break;
                case ILOC::COMMA:
                    std::cout << ",";
                    break;
                case ILOC::INTO:
                    std::cout << "=>";
                    break;
                case ILOC::ENDLINE:
                    std::cout << "EOL";
                    break;
                case ILOC::ENDFILE:
                    std::cout << "EOF";
                    break;
                case ILOC::INVALID:
                    std::cout << (char)t.lexeme;
                    break;
                default:
                    std::cout << t.lexeme;
                    break;
            }

            std::cout << "\n";

            if(t.category == ILOC::ENDFILE) break;
        }
        return 0;
    }

    // by default, parse
    bool isValidILOC = parser.parse(scanner, ir);

    if(options.mode == cli::Mode::IR) {
        if(!isValidILOC) return 1;
        ir.printIR();
        std::cout << "MAX SR FOUDN: " << ir.getMaxSR() << std::endl;
        return 0;
    }

    if(options.mode == cli::Mode::Rename) {
        if(!isValidILOC) return 1;
        RegAlloc regAlloc(ir.getMaxSR());
        if(!regAlloc.renameReg(ir)) return 1;
        ir.printRenamedILOC();
        return 0;
    }

    // Parse mode output
    if(isValidILOC) {
        std::cout << "Valid ILOC\n";
        return 0;
    }
    return 1;
}
