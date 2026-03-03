#include "CLI.h"

cli::Options cli::parseArgs(int argc, char** argv) {
    cli::Options opts{};

    // Store args[1:] in vector;
    std::vector<std::string> args(argv + 1, argv + argc);

    int flagCount = 0;
    for(auto& arg : args) {
        if(arg[0] != '-') {  // Treat as file
            if(opts.filename.empty()) opts.filename = std::string(arg);
            else { 
                opts.error = "ERROR: Too many files passed";  
                opts.mode = Mode::Invalid;
            }
        }

        // process flags
        if(arg[0] == '-' && opts.mode != Mode::Invalid) {
            // Warning for extra flags
            if(flagCount > 1) std::cout << "WARNING: Too many flags, choosing higest priority\n";
            flagCount++;
            
            switch(arg[1]) {
                case 'h': opts.mode = (Mode::Help > opts.mode)  ? Mode::Help   : opts.mode; break;
                case 'x': opts.mode = (Mode::Rename > opts.mode)? Mode::Rename : opts.mode; break;
                case 'r': opts.mode = (Mode::IR > opts.mode)    ? Mode::IR     : opts.mode; break;
                case 'p': opts.mode = (Mode::Parse > opts.mode) ? Mode::Parse  : opts.mode; break;
                case 's': opts.mode = (Mode::Scan > opts.mode)  ? Mode::Scan   : opts.mode; break;
                default : opts.mode = (Mode::Invalid > opts.mode)  ? Mode::Invalid   : opts.mode; break;
            }
        }
    }

    // Post Processing of Arguments...

    // No flag was provided --> parse mode
    if(opts.mode == Mode::Initial) opts.mode = Mode::Parse;

    // General Error 
    if(opts.mode == Mode::Invalid) opts.error = "ERROR: Invalid Flag Passed";

    // Flag but no file
    if(opts.filename.empty() && opts.mode != Mode::Help)  {
        opts.error = "ERROR: No file paseed";
        opts.mode = Mode::Invalid;
    }

    return opts;
}

void cli::help() {
    std::cout << "CSCE434 Compiler Design Lab 1\n";
    std::cout << "Compiler front end for an ILOC language to produce the IR of the program.\n\n";

    std::cout << "Usage: 434e [OPTION]...\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h               print this help message\n";

    std::cout << "  -s <name>        executes only the scanner functionality to produce a list of tokens found in the input file\n";

    std::cout << "  -p <name>        scans and parses the input to generate the Intermediate Representation of the input. Reports success or failure.\n";

    std::cout << "  -r <name>        generates the Intermediate Representation and then prints it in a human-readable format\n";
    std::cout << "  -x <name>        scans/parses input, performs register renaming, and prints renamed IR\n";

    std::cout << "\n";

    std::cout << "If no flag is provided, the default behavior is as if the -p flag was specified\n";
    std::cout << "Flags are intended to be mutually exclusive, providing multiple will follow the flag priority: -h, -x, -r, -p, -s\n";
}
