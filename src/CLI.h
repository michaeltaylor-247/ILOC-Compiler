#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace cli {
    // Execution flag precedence: -h, -x, -r, -p, and -s
    enum class Mode : uint8_t {
        Initial = 0,    // Dummy Intial State
        Invalid = 1,    // invlalid arg state
        Scan    = 2,
        Parse   = 3,
        IR      = 4,
        Rename  = 5,
        Help    = 6,
    };

    struct Options {
        Mode mode{};                // Defualt behavior 
        std::string error = "";     // Error Message
        std::string filename = "";  // Filename
    };


    // Main Functionality
    Options parseArgs(int argc, char** argv);

    // Help Text
    void help();

} // namespace cli
