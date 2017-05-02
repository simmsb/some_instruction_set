# Assembler for instruction set

example

```hs
*Main> b = Three Mov (Reg True 1) (Mem False 44)
*Main> c = Three Mov (Reg True 1) (Mem True 22)
*Main> d = One Nop
*Main> e = Two Psh (Reg False 0)
*Main> e
Two Psh (Reg False 0)
*Main> n = [b, c, d, e]
*Main> n
[Three Mov (Reg True 1) (Mem False 44),Three Mov (Reg True 1) (Mem True 22),One Nop,Two Psh (Reg False 0)]
*Main> assemble n
"8001c001002c8001c0018016000040014000"
*Main>```
