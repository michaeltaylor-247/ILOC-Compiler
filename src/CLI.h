#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace cli {
    // Execution flag precedence: -h, -r, -p, and -s
    enum class Mode : uint8_t {
        Initial = 0,    // for enum comparison to bubble up priority
        Invalid = 1,    // invlalid arg state
        Scan = 2,
        Parse = 3,
        IR = 4,
        Help = 5,
    };

    struct Options {
        Mode mode{};                // defualt behavior 
        std::string error = "";     // default no error message
        std::string filename = "";  // default no file                                    
    };


    // The main functionality
    Options parseArgs(int argc, char** argv);

    // Presents the help options text
    void help();


} // namespace cli
