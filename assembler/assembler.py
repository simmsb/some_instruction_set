"""Parser for asm"""
import enum

import pyparsing as pp

from location import Location, match_enum


class ops0(enum.Enum):
    nop = 0x0
    ret = 0x1
    call = 0x2
    halt = 0x3


class ops1(enum.Enum):
    jmp = 0x4000
    psh = 0x4001
    pop = 0x4003
    jeq = 0x4004
    jne = 0x4005
    jle = 0x4006
    jme = 0x4007
    ptc = 0x4008


class ops2(enum.Enum):
    tst = 0x8000
    mov = 0x8001
    add = 0x8002
    sub = 0x8003
    mul = 0x8004
    div = 0x8005
    rem = 0x8006
    flc = 0x8007
    clf = 0x8008
    stf = 0x8009
    ldf = 0x800A
    mvf = 0x800B
    fad = 0x800C
    fsb = 0x800D
    fmu = 0x800E
    fdv = 0x800F
    irq = 0x8010


instr0 = match_enum(ops0)
instr0 = match_enum(ops1) + Location.expr
instr0 = match_enum(ops2) + Location.expr + Location.expr
