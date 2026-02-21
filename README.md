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

##### Part 1 (rename regsiter)
Take the source registers and map to virtual registers based on the "lifespace"
- Live Range --> Virtual Register Mapping

##### part 2 (register allocation)
The act of assigning physical registers to be used...
We are explicitly not allowed to do any optimizations! The only thing we can do is remove NOP and assign registers. 

**Cool Bit on Register Allocation**:
As Zoran's video describes, the lab2 part2 register allocation will have "spill code". This is instructions that must be added to the program in order to move value's from the finite number of registers to memory and back. There is no "solved" solution to this problem -- it's considered an NP-hard problem and is synonymous with the *minimun graph coloring** problem. 


**Use** vs. **define**:
- For most opcodes, there will be registers that are said to be *used* and some that are *defined*
- **Used** - a register (operand) in a opcode that is being *used* in computation
- **Defined** - a register that is being assigned some value, defining it

EX:
`add r1, r2 => r3`
- `r1` is used
- `r2` is used
- `r3` is defined

The unique opcode is `store`... the right hand side of `=>` is a *use*. For other opcodes, the operand to the right of `=>` is a define

----
#### TODO
- [x] Refactor to make IR first class
    - [x] Move the IR struct to own file/class
    - [x] Move the Opcode struct from scanner?
    - [x] Add to main
    - [x] Alter the Scanner Class to modify IR via reference (token stream atp)
    - [x] Alter the Parser Class to modify IR via reference (actual IR)

- [ ] Implement the RegAlloc Class...
    - [ ] Renaming
    - [ ] CLI regalloc flags
        - [ ] Including the weird ones? Read the doc



- Other Refactoring...
    - [ ] The refactor from earlier was nice and was mvp...but Parser still needs more. 
        - First off the def consturctor aint even doing anything, i just hardcode default value. 
        - the internal IR pointer we maintain is init when we call `.parse()` not when we init the parser
            - this is where we also pass in the reference to a scanner...
        - etc.
        - idk, the thing reads odd. I feel like Parser should not necesarrily own a reference to the Scanner or the IR.

