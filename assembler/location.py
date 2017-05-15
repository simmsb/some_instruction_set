"""Assembler for the Instruction set."""

import ast

import pyparsing as pp

from instructions import Compilable, Register, pack_address, wrap_hexpad


class Reference(Compilable):

    def __init__(self, t, *, is_deref=False):
        """Compile a reference into it's correct form."""
        self.t = t
        self.is_deref = is_deref

    @wrap_hexpad
    def compile(self, context):
        if isinstance(self.t, Register):
            return pack_address(self.t.value, is_reg=True, is_deref=self.is_deref)

        if isinstance(self.t, int):
            return pack_address(self.t, is_deref=self.is_deref)

        if isinstance(self.t, str):
            if self.t in context.labels:
                return pack_address(context.labels[self.t])

        raise Exception(f"Cannnot compile {self.t} object of type {type(self.t)} into a raw memory location or unresolved label")

    def __str__(self):
        if self.is_deref:
            return f"[{self.t}]"
        return str(self.t)


def match_enum(enum_, *, prefix=None):
    """Return a parser that matches an enum and has the parse action set to the Enum."""
    def get_tok(tok):
        if prefix:
            return pp.Combine(pp.Literal(prefix).suppress() + pp.Literal(tok))
        return pp.Literal(tok)

    match = pp.MatchFirst(get_tok(i.name) for i in enum_)  # type: pp.MatchFirst
    catch = match.setParseAction(lambda t: enum_[t[0]])
    return catch


class Location:
    register = match_enum(Register, prefix='%')
    integer = pp.Word(pp.nums).setParseAction(lambda t: int(t[0]))

    deref = pp.nestedExpr(opener='[', closer=']', content=register | integer)
    deref.setParseAction(lambda t: Reference(t[0][0], is_deref=True))

    nonref = register | integer
    nonref.setParseAction(lambda t: Reference(t[0]))

    label_ref = pp.Word(pp.alphanums)
    label_ref.setParseAction(lambda t: Reference(t[0]))

    quote = "\'"
    char_lit_escapes = pp.Word(r"\n\r\t", max=2) | pp.Word(pp.alphanums, max=1)
    char_lit = pp.Combine(quote + char_lit_escapes + quote)
    char_lit.setParseAction(lambda t: Reference(ord(ast.literal_eval(t[0]))))

    expr = deref | nonref | label_ref | char_lit
