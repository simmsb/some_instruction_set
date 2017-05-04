"""Assembler for the Instruction set."""

import operator
from functools import reduce

import pyparsing as pp

import enum


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


def match_enum(enum_):
    """Return a parser that matches an enum and has the parse action set to the Enum."""
    match = reduce(operator.or_, (pp.Literal(i.name) for i in enum_))  # type: pp.MatchFirst
    catch = match.setParseAction(lambda t: enum_[t[0]])
    return catch


class Parsing:
    register = match_enum(Register)
