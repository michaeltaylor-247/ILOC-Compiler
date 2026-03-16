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
Take the source registers and map to virtual registers based on the "lifespan"
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

##### Part 3: Optimizing Register Allocation
- In bottom up allocation, there are times where naively spilling or restoring by using a `loadI` and `store` is not needed. 

Amongst other things, this is a key optimization you can make as the difference in cycle count between opcodes that involve accessing memory and those that don't is HUGE ~ 100x more cycles in application. 

> Refer to page 688 in the textbook for some detail on bottom up compilation. 

==== Actual optimziation notes! ====
When optimizing the spill/restore cost and code, there are really 3 cases to consider:
- "Dirty" --> a value that exists on in a physical register; it was computed. Thus to spill it means to save it in memory. Otherwise its lost
    - so a loadI and store

- "Clean" ---> a value that exists in memory. It was either loaded directly from memory in PR or someone else had already spilled it. Thus there is no need to store it... its already there.
    - Still need to restore tho

- "Re-materializable" ---> values that are cheaper to recompute rather than actually spil
    -  The example here is `loadI`. If you you load a constant into a register, you don't need to spill the defined register from loadi. 
    - There are others... results from an `add`
    - Thus you don't need to store (spill) OR restore


Clean values are complex... zoran says that an entire pass before allocation would most likely be necessary. The idae behind clean values is that the value might already be sitting in memory, no need to respill it. In the general case where the ILOC program has defined a VR with a `load`, there is a chance that the value is still in memory... hwoever the user ILOC program might've also overwritten that memory location.

However a subset of clean values are more manageable. This is when the allocator has created a clean value due to its spilling -- the value @ memory is guaranteed to be valid since the allocator is the one writing to spill memory (memory > 32768).


The other type of optimziation is "which register to spill"? 
We've been using the heuristic, "spill the Register whose next use is the furthest". However when we take into consideration the costs of differen types of values, it might make sense to spill a different register because its restoring cost (or rematerialization) would actually be less than the furthest next use one. 

The optimziation is to considering the following cases:
- Find the max NU for 2 or 3 registers and then check then check which are...
    - dirty & rematerializable
    - dirty, already spilled, and rematerialzable.

The allocator can track rematerialiable and already spilled values pretty easily because at definiition with `loadI`, you know its rematerializable and at the time of spilling you know the VR is already spilled. Then can choose which register to spill based off this.

You can also add other hueristics like taking into accout "distance" vs "cost"... idk if that makes sense. 
- "if the operations are this close, then do that". Otherwise choose the other. 


One more optimization to reconsider is the reserved spill register. That register is idle for potentially a large number of operations... only needed for spilling and restoring. So can try to find some method to take advantage of the reserved register.


