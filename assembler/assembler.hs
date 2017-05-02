import           Numeric (showHex)

data Location = Reg Bool Integer
              | Mem Bool Integer deriving (Show)

data OInstr = Nop
            | Ret
            | Call
            | Halt deriving (Show, Enum)

data TInstr = Jmp
            | Psh
            | Pop
            | Jeq
            | Jne
            | Jle
            | Jme
            | Ptc deriving (Show, Enum)

data FInstr = Tst
            | Mov
            | Add
            | Sub
            | Mul
            | Div
            | Rem
            | Flc
            | Clf
            | Stf
            | Ldf
            | Mvf
            | Fad
            | Fsb
            | Fmu
            | Fdv
            | Irq deriving (Show, Enum)

data Instruction = One OInstr | Two TInstr Location | Three FInstr Location Location deriving (Show)

type Stream = [Instruction]

boolToInt :: Bool -> Integer
boolToInt True  = 1
boolToInt False = 0

pack :: Location -> String
pack (Reg deref value) = zfill 4 (showHex num "")
  where num = value + (boolToInt deref * 2^15) + (2^14)
pack (Mem deref value) = zfill 4 (showHex num "")
  where num = value + (boolToInt deref * 2^15)

zfill :: Int -> String -> String
zfill n s = replicate (n - length s) '0' ++ s

getHex :: Enum a => a -> String
getHex s = zfill 3 $ showHex (fromEnum s) ""

compile :: Instruction -> String
compile (One instr)             = '0' : getHex instr
compile (Two instr arg)         = '4' : getHex instr ++ pack arg
compile (Three instr arg1 arg2) = '8' : getHex instr ++ pack arg1 ++ pack arg2


assemble :: Stream -> String
assemble = concatMap compile
