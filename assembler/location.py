"""Assembler for the Instruction set."""

import enum
import operator
from functools import reduce

import pyparsing as pp


class Register(enum.Enum):
    aaa = 0
    bbb = 1
    ccc = 2
    ddd = 3
    eee = 4
    fff = 5
    ggg = 6
    esp = 7
    epb = 8
    rip = 9
    acc = 10


class Dereference:

    def __init__(self, value):
        self.value = value

    def __str__(self):
        """No."""
        return f"[{self.value}]"

    def expand(self):
        yield from consume_math(*self.value)


def match_enum(enum_, *, prefix=None):
    """Return a parser that matches an enum and has the parse action set to the Enum."""
    def get_tok(tok):
        if prefix:
            return pp.Combine(pp.Literal(prefix).suppress() + pp.Literal(tok))
        return pp.Literal(tok)

    match = reduce(operator.or_, (get_tok(i.name) for i in enum_))  # type: pp.MatchFirst
    catch = match.setParseAction(lambda t: enum_[t[0]])
    return catch


class Operation:

    def __init__(self, name):
        self.name = name

    def __str__(self):
        return f"{self.__class__.__name__}: {self.name}"


class Push(Operation):
    pass


class Op(Operation):
    pass


class Deref:
    pass


def consume_math(*math):
    """Consume some math expression."""
    if len(math) == 1:
        yield math[0]

    def bin_op(left, op, right, *rest):
        def expansion(val):
            if isinstance(val, (Register, int)):
                yield Push(val)
            elif isinstance(val, Dereference):
                yield from val.expand()
                yield Deref()
            elif isinstance(val, (list, pp.ParseResults)):
                yield from consume_math(*val)

        yield from expansion(left)
        yield from expansion(right)
        yield Op(op)

        if rest:
            yield from bin_op(None, *rest)

    yield from bin_op(*math)


class Location:
    register = match_enum(Register, prefix='%')
    integer = pp.Word(pp.nums).setParseAction(lambda t: int(t[0]))

    expr = pp.Forward()

    combinator = register | integer | pp.nestedExpr(content=expr) | \
        pp.nestedExpr(opener="[", closer="]", content=expr).setParseAction(lambda t: Dereference(t[0]))

    expr <<= combinator + pp.ZeroOrMore(pp.oneOf("+ -") + combinator)
