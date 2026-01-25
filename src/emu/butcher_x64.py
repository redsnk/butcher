# pip install goto-statement

from ctypes import *
#from goto import with_goto

class _r8(Structure):
    _fields_ = [("l",c_uint8),
                ("h",c_uint8),
                ("l2",c_uint8),
                ("h2",c_uint8),
                ("l3",c_uint8),
                ("h4",c_uint8),
                ("l4",c_uint8),
                ("h4",c_uint8)]

class _r16(Structure):
    _fields_ = [("l",c_uint16),
                ("h",c_uint16),
                ("l2",c_uint16),
                ("h2",c_uint16)]

class _r32(Structure):
    _fields_ = [("l",c_uint32),
                ("h",c_uint32)]

class _reg(Union):
    _fields_ = [("r64",c_uint64),
                ("r32",_r32),
                ("r16",_r16),
                ("r8",_r8)]

'''
@with_goto
def test():
    print("1")
    label ._0x00001
    print("2")
    goto .next_step
    print("3")
    label .next_step
    print("4")

r = _reg()
r.r64 = 0x0102030405060708;
print(hex(r.r64))
print(hex(r.r32.h))
print(hex(r.r32.l))
print(hex(r.r16.h))
print(hex(r.r8.h))
print(hex(r.r8.l))
test()
'''
