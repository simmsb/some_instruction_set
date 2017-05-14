"""Parser for asm."""
import pyparsing as pp

from instructions import Compilable, Instruction, ops0, ops1, ops2
from location import Location, match_enum

DEBUG = True


class WrappedExpr:

    def __init__(self, terms):
        if len(terms) > 1:
            raise Exception("Argument somehow has multiple parts")
        self.term = terms.expression

    def __str__(self):
        return str(self.term)


class Label:

    def __init__(self, name):
        self.name = name[1:]


class AssemberContext(Compilable):

    def __init__(self, data):
        self.data = data
        self.labels = {}

        self.resolve_labels()

    def resolve_labels(self):
        label_count = 0
        for c, i in enumerate(self.data):
            if isinstance(i, Label):
                self.labels[i.name] = c + label_count
                label_count += 1

        self.data = [i for i in self.data if isinstance(i, Instruction)]
        if DEBUG:
            print("\n".join(str(i) for i in self.data))

    def compile(self):
        return "".join(i.compile(self) for i in self.data)


instr0 = match_enum(ops0)
instr1 = match_enum(ops1) + Location.expr
instr2 = match_enum(ops2) + Location.expr + Location.expr

label = pp.Word(":", bodyChars=pp.alphas).setParseAction(lambda t: Label(t[0]))

code_line = instr0 | instr1 | instr2
code_line.setParseAction(lambda t: Instruction(*t))
line = code_line | label
code = pp.delimitedList(line, delim=";")


def assemble(string: str):
    """Assemble some ASM into machine instructions."""
    data = code.parseString(string)

    context = AssemberContext(data)

    return context.compile()


if __name__ == '__main__':
    import sys
    for i in sys.argv[1:]:
        print(f"\nCompiling {i}:\n")
        with open(i) as f:
            program = f.read()
            assembled = assemble(program)
            print(assembled)
