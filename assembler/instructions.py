import enum


class ops0(enum.IntEnum):
    nop = 0x0
    ret = 0x1
    call = 0x2
    halt = 0x3


class ops1(enum.IntEnum):
    jmp = 0x4000
    psh = 0x4001
    pop = 0x4002
    jeq = 0x4003
    jne = 0x4004
    jle = 0x4005
    jme = 0x4006
    ptc = 0x4007


class ops2(enum.IntEnum):
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


class Register(enum.IntEnum):
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


def hexpad(val: int):
    """Compile integer into 4 char padded hex."""
    if val > 0xFFFF:
        raise Exception("Cannot pack value larger than 0xFFFF")
    return hex(val)[2:].zfill(4).upper()


def wrap_hexpad(func):
    """A decorator to hexpad the result of a function."""
    def inner(*args, **kwargs):
        return hexpad(func(*args, **kwargs))
    return inner


def pack_address(value, *, is_reg=False, is_deref=False):
    """Pack an address into an integer."""
    return value | is_reg << 14 | is_deref << 15


class Compilable:
    """Base class for compilable objects."""

    @staticmethod
    def compile(context):
        return NotImplemented


class Instruction:

    def __init__(self, instr, *args):
        self.instr = instr
        self.args = args

    def __str__(self):
        args = " ".join(map(str, self.args))
        return f"{self.instr.name} {args}".strip()

    def __len__(self):
        return len(self.args) + 1

    def compile(self, context):
        comp = hexpad(self.instr) + "".join(i.compile(context) for i in self.args)
        print(f"compiling: {self} -> {comp}")
        return comp
