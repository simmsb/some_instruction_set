data Instruction = OneArg OInstr
                 | TwoArg TInstr
                 | ThrArg FInstr deriving (Show)

data OInstr = Nop
            | Ret
            | Call
            | Halt

data TInstr = Jmp Int
            | Psh Int
            | Pop Int
            | Jeq Int
            | Jne Int
            | Jle Int
            | Jme Int
            | Ptc Int

data FInstr = Tst Int Int
            | Mov Int Int
            | Add Int Int
            | Sub Int Int
            | Mul Int Int
            | Div Int Int
            | Rem Int Int
            | Flc Int Int
            | Clf Int Int
            | Stf Int Int
            | Ldf Int Int
            | Mvf Int Int
            | Fad Int Int
            | Fsb Int Int
            | Fmu Int Int
            | Fdv Int Int deriving (Show)

type Stream = [Instruction]

compile :: Instruction -> Integer
compile OneArg instr = case instr of
  Nop  -> 0x0000
  Ret  -> 0x0001
  Call -> 0x0002
  Halt -> 0x0003
compile TwoArg (instr arg) = case instr of
  Jmp -- TODO 
