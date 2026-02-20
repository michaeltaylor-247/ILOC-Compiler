//NAME: Michael Taylor
//NETID: 934004252

Within the tar file contains the following files:
```
.
├── Makefile
├── README.md
└── src
    ├── RegAlloc.cpp
    ├── RegAlloc.h
    ├── cli.cpp
    ├── cli.h
    ├── main.cpp
    ├── parser.cpp
    ├── parser.h
    ├── scanner.cpp
    └── scanner.h
```

In order to execute, run:
```
$ make clean
$ make build
$ ./434alloc <options> <file>
```

In order to see usage, pass the `-h` flag.

---
### Description
This directory contains the implementation of a compiler targeting the ILOC language. More precisely, this directory contains up to the **Register Allocation** portion of a compiler:
- Register Renaming (Source Reg --> Virtual Reg)
- Register Allocation (Virtual Reg --> Physical Reg)


----
#### Personal Notes
- [ ] Refactor to make IR first class
    - [ ] Move the IR struct to own file/class
    - [ ] Move the Opcode struct from scanner?
    - [ ] Add to main
    - [ ] Alter the Scanner Class to modify IR via reference (token stream atp)
    - [ ] Alter the Parser Class to modify IR via reference (actual IR)

> Check performance of the refactored code with ./timeer @ ssh
    - T128k.i was about 

- [ ] Implement the RegAlloc Class...
    - [ ] Renaming
    - [ ] CLI regalloc flags
        - [ ] Including the weird ones? Read the doc



- Other Refactoring...
    - [ ] Probably should decouple scanner and parser...
        - Right now i pass the scanner to the parser's constructor and it maintains it as a data member....
        - realistically i should use main as the orchestrator and instead be calling scanner.getToken() when the parser says it "needs" it main
        ---> truly lean into "main being a pipeline orchestrator approach"....
































