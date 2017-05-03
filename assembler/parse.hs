import Data.Attoparsec.ByteString
import Assembler

data Register = Aaa
              | Bbb
              | Ccc
              | Ddd
              | Eee
              | Fff
              | Ggg
              | Esp
              | Epb
              | Rip
              | Acc deriving (Show, Eq, Enum, Bounded)


data Dereference = IntDeref Int | RegDeref Register

parseEnumValue :: Show a => a -> Parser a
parseEnumValue val = string (map toLower $ show val) >> return val

parseEnum :: (Show a, Enum a, Bounded a) => Parser a
parseEnum = choice $ map parseEnumValue [minBound..maxBound]

register :: Parser Register
register = do
  string "%"
  return $ parseEnum :: Register

dereference :: Parser Dereference
dereference = do
  char '['
  return $ read (manyTill anychar (char ']')) <|> register *> char ']'

poInstr :: Parser OInstr
poInstr = parseEnum :: OInstr
  
