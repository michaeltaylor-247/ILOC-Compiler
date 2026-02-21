This is the provided pseduo code for the renaming PR to VR. 
```
VRName ← 0 // start VR names at 0
for i ← 0 to max SR number
SRToVR[i] ← invalid
LU[i] ← ∞
index ← block length
for each Op in the block, bottom to top
for each operand O that OP defines
if SRToVR[O.SR] = invalid // Unused DEF
then SRToVR[O.SR] ← VRName++
O.VR ← SRToVR[O.SR]
O.NU ← LU[O.SR]
SRToVR[O.SR] ← invalid // kill OP3
LU[O.SR] ← ∞
for each operand O that OP uses
if SRToVR[O.SR] = invalid // Last USE
then SRToVR[O.SR] ← VRName++
O.VR ← SRToVR[O.SR]
O.NU ← LU[O.SR]
for each operand O that OP uses
LU[O.SR] ← index
```


Testing outputs:
λ /mnt/class_files/Classes/CSCE-434/Class/lab2/CodeChecks/CodeCheck1  ./434alloc                        
CSCE434, Lab 2, Code Check 1

NetID: michaeltaylor247         csce-compute.engr.tamu.edu
Run on 02-20-26 at 20:42:09

File cc1.i :            Wrong output value
failed
File cc2.i :            Wrong output value
failed
File cc3.i :            Wrong output value
failed
File cc4.i :    passed
File cc5.i :            Wrong output value
failed

./434alloc fails Code Check 1

                                                                                                        



cc1.i:
```
//NAME: John "Jack" Joseph OConnor IV
//NETID: jjo1
//SIM INPUT:
//OUTPUT: 96
//
// check1.i is a slightly modified version of a
// block submitted by Jack O'Connor in Fall 2014.
//
// It checks to see that the scanner and parser
// correctly accept all of the ILOC subset operations.
//
// It also checks that the scanner correctly handles
// an input line that is terminated by EOF (end of file)
// in lieu of a new line.
//
        loadI   1024    =>      r0
        loadI   128     =>      r0001
        loadI   32      =>      r2
        loadI   1028    =>      r3
        mult    r2,r02  =>      r4
        rshift  r2,r04  =>      r4
        lshift  r2,r004 =>      r05
        add     r5,r4   =>      r5
        sub     r2,r05  =>      r06
        store   r6      =>      r3
        load    r00003  =>      r6
        add     r06,r1  =>      r7
        nop
        store   r7      =>      r0
        output 1024

```

