# Meme tier instruction set, somewhat ARM based

# Operand Types

* `data`      -> `bXY00000000000000`
   - If X is set: dereference value
   - If Y is set: get register
   - If Y not set: use abs

 * `location` -> `bXY00000000000000`
    - If X is set: set memory at deref of value
    - If Y is set: use register
    - If Y not set: use memory address

# instruction table

| Opname | OpCode | arg1 type | arg2 type | description                                                                                  |
|--------|--------|-----------|-----------|----------------------------------------------------------------------------------------------|
| nop    | 0x0000 | -         | -         | No Operation                                                                                 |
| ret    | 0x0001 | -         | -         | Pops variable from the stack and returns to location                                         |
| call   | 0x0002 | -         | -         | Pops Address from stack, push return address, jump to location                               |
| halt   | 0x0003 | -         | -         | Halt cpu                                                                                     |
|        |        |           |           |                                                                                              |
| jmp    | 0x4000 | data      | -         | Jump to location specified by arg1                                                           |
| psh    | 0x4001 | data      | -         | Push arg1 to the stack                                                                       |
| pop    | 0x4002 | dest      | -         | Pop value from the stack into specified memory location                                      |
| jeq    | 0x4003 | data      | -         | Jump to location specified by arg1 if last comparison was equal                              |
| jne    | 0x4004 | data      | -         | Jump to location specified by arg1 if last comparison was not equal                          |
| jle    | 0x4005 | data      | -         | Jump to location specified by arg1 if last comparison was less than                          |
| jme    | 0x4006 | data      | -         | Jump to location specified by arg1 if last comparison was more than                          |
| ptc    | 0x4007 | data      | -         | Print character of arg1 to terminal                                                          |
|        |        |           |           |                                                                                              |
| tst    | 0x8000 | data      | data      | Compare arg1 with arg2                                                                       |
| mov    | 0x8001 | dest      | data      | Move arg2 into arg1                                                                          |
| add    | 0x8002 | data      | data      | Add arg1 to arg2 and store in ACC register                                                   |
| sub    | 0x8003 | data      | data      | Subtract arg2 from arg1 and store in ACC register                                            |
| mul    | 0x8004 | data      | data      | Multiply arg1 with arg2 and stor in ACC register                                             |
| divn   | 0x8005 | data      | data      | Divide arg1 by arg2 and store in ACC register                                                |
| rem    | 0x8006 | data      | data      | Get modulus of arg1 with arg2 and store in ACC register                                      |
| flc    | 0x8007 | data      | data      | Convert arg1 and arg2 into float and store in float reg aaa                                  |
| clf    | 0x8008 | dest      | dest      | Convert float register aaa into two ints and stor in arg1 and arg2                           |
| stf    | 0x8009 | dest      | dest      | Store upper and lower bits of float register aaa into arg1 and arg2                          |
| ldf    | 0x800A | data      | data      | Load upper and lower bits of a float into float register aaa (source arg1 arg2 respectively) |
| mvf    | 0x800B | data      | data      | Move float reg arg2 into float reg arg1                                                      |
| fad    | 0x800C | data      | data      | Add float register arg1 to float register arg2 and store in float register acc               |
| fsb    | 0x800D | data      | data      | Subtract float register arg2 from float register arg1 and store in float register acc        |
| fmu    | 0x800E | data      | data      | Multiply float register arg1 with float register arg2 and store in float register acc        |
| fdv    | 0x800F | data      | data      | Divide float register arg1 with float register arg2 and store in float register acc          |


# Registers
Registers are defined by the enum:
```c
enum Registers {
  aaa = 0,
  bbb = 1,
  ccc = 2,
  ddd = 3,
  eee = 4,
  fff = 5,
  ggg = 6,
  esp = 7,
  epb = 8,
  rip = 9,
  acc = 10,
};
```
Example for accessing register `ggg`:  
`b0100000000000110` -> `0x4006`

To write `*bbb` to a register `aaa`  
aaa  -> `b0100000000000000` -> `0x4000`  
\*bbb -> `b1100000000000001` -> `0xC001`  
mov -> `0x8001`  

final instruction -> `0x80014000C001`
