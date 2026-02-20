# pip install goto-statement
import sys
import traceback
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
                ("s64",c_int64),
                ("r32",_r32),
                ("r16",_r16),
                ("r8",_r8)]

class _mem:
    def __init__(self, addr, data):
        self.addr = addr
        self.size = len(data)
        self.mem = data

'''
union _eflags {
  	struct {
    	uint32_t CF 	: 1;
    	uint32_t _u1 	: 1;
		uint32_t PF		: 1;
		uint32_t _u2 	: 1;
		uint32_t AF		: 1;
		uint32_t _u3 	: 1;
		uint32_t ZF		: 1;
		uint32_t SF		: 1;
		uint32_t TF		: 1;
		uint32_t IF		: 1;
		uint32_t DF		: 1;
		uint32_t OF		: 1;
  	};
  	uint32_t r32;
};
'''
class _eflags(Structure):
    _fields_ = [("r32",c_uint32)]

class _cpu:
    rax = _reg()
    rbx = _reg()
    rcx = _reg()
    rdx = _reg()
    r8 = _reg()
    r9 = _reg()
    r10 = _reg()
    r11 = _reg()
    r12 = _reg()
    r13 = _reg()
    r14 = _reg()
    r15 = _reg()
    rdi = _reg()
    rsi = _reg()
    rbp = _reg()
    rsp = _reg()

    eflasg = _eflags()

    mems = []

    def panic(self,text):
        for line in traceback.format_stack():
            print(line.strip())
        print("panic: "+text)
        sys.exit(0)

    def add_mem(self,addr,data):
        self.mems.append(_mem(addr,data))

    def add_zero_mem(self,addr,size):
        self.add_mem(addr,[0] * size)

    def get_mem(self,addr,size):
        for m in self.mems:
            if (addr >= m.addr) and ((addr+size) <= (m.addr + m.size)):
                start = addr-m.addr
                return m.mem[start:start+size]
        self.panic("get_mem - "+hex(addr))

    def set_mem(self,addr,data):
        size = len(data)
        for m in self.mems:
            if (addr >= m.addr) and ((addr+size) <= (m.addr + m.size)):
                start = addr-m.addr
                pre = m.mem[:start]
                post = m.mem[start+size:]
                m.mem = pre+data+post
                return
        self.panic("set_mem - "+hex(addr))

    def load_mem (self,name,d_Offset,d_Size,v_Address,v_Size):
        if d_Size > 0:
            f = open(name,'rb')
            f.seek(d_Offset,1)
            data = f.read(d_Size)
            if d_Size < v_Size:
                a = bytearray(v_Size-d_Size)
                data += a
        else:
            data = bytearray(v_Size)
        self.add_mem(v_Address,data)
    
    def call_from_iat (self,lib,func):
        self.panic("call_from_iat")

    def jmp_from_iat (self,lib,func):
        self.panic("jmp_from_iat")

    def get_byte_ptr(self,addr):
        data = self.get_mem(addr,1)
        return data[0]
    
    def s_get_byte_ptr(self,addr):
        data = self.get_byte_ptr(addr)
        return c_int8(data).value

    def get_word_ptr(self,addr):
        data = self.get_mem(addr,2)
        return (data[1] << 8) | data[0]

    def s_get_word_ptr(self,addr):
        data = self.get_word_ptr(addr)
        return c_int16(data).value

    def get_dword_ptr(self,addr):
        data = self.get_mem(addr,4)
        return (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0]
    
    def s_get_dword_ptr(self,addr):
        data = self.get_dword_ptr(addr)
        return c_int32(data).value

    def get_qword_ptr(self,addr):
        data = self.get_mem(addr,8)
        return (data[7] << 56) | (data[6] << 48) | (data[5] << 40) | (data[4] << 32) | (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0]
    
    def s_get_qword_ptr(self,addr):
        data = self.get_qword_ptr(addr)
        return c_int64(data).value

    def set_byte_ptr(self,addr,value):
        self.set_mem(addr,[value])

    def set_word_ptr(self,addr,value):
        self.set_mem(addr,[value & 0xff])
        self.set_mem(addr+1,[(value >> 8) & 0xff])

    def set_dword_ptr(self,addr,value):
        self.set_mem(addr,[value & 0xff])
        self.set_mem(addr+1,[(value >> 8) & 0xff])
        self.set_mem(addr+2,[(value >> 16) & 0xff])
        self.set_mem(addr+3,[(value >> 24) & 0xff])

    def set_qword_ptr(self,addr,value):
        self.set_mem(addr,[value & 0xff])
        self.set_mem(addr+1,[(value >> 8) & 0xff])
        self.set_mem(addr+2,[(value >> 16) & 0xff])
        self.set_mem(addr+3,[(value >> 24) & 0xff])
        self.set_mem(addr+4,[(value >> 32) & 0xff])
        self.set_mem(addr+5,[(value >> 40) & 0xff])
        self.set_mem(addr+6,[(value >> 48) & 0xff])
        self.set_mem(addr+7,[(value >> 56) & 0xff])

    def push_byte(self,value):
        self.rsp.r64 = self.rsp.r64 - 1
        self.set_byte_ptr(self.rsp.r64,value)

    def push_word(self,value):
        self.rsp.r64 = self.rsp.r64 - 2
        self.set_word_ptr(self.rsp.r64,value)

    def push_dword(self,value):
        self.rsp.r64 = self.rsp.r64 - 4
        self.set_dword_ptr(self.rsp.r64,value)

    def push_qword(self,value):
        self.rsp.r64 = self.rsp.r64 - 8
        self.set_qword_ptr(self.rsp.r64,value)

    def push(self,bits,value):
        if bits == 8:
            self.push_byte(value)
        elif bits == 16:
            self.push_word(value)
        elif bits == 32:
            self.push_dword(value)
        else:
            self.push_qword(value)
     
    def pop_byte(self):
        v = self.get_byte_ptr(self.rsp.r64)
        self.rsp.r64 = self.rsp.r64 + 1
        return v

    def pop_word(self):
        v = self.get_word_ptr(self.rsp.r64)
        self.rsp.r64 = self.rsp.r64 + 2
        return v

    def pop_dword(self):
        v = self.get_dword_ptr(self.rsp.r64)
        self.rsp.r64 = self.rsp.r64 + 4
        return v

    def pop_qword(self):
        v = self.get_qword_ptr(self.rsp.r64)
        self.rsp.r64 = self.rsp.r64 + 8
        return v
    
    def pop(self,bits):
        if bits == 8:
            return self.pop_byte()
        elif bits == 16:
            return self.pop_word()
        elif bits == 32:
            return self.pop_dword()
        else:
            return self.pop_qword()
        
    # 64
    @property
    def _rax(self):
        return self.rax.r64
    @_rax.setter
    def _rax(self,v):
        self.rax.r64 = v

    @property
    def s_rax(self):
        return self.rax.s64
    @s_rax.setter
    def s_rax(self,v):
        self.rax.s64 = v

    @property
    def _rbx(self):
        return self.rbx.r64
    @_rbx.setter
    def _rbx(self,v):
        self.rbx.r64 = v

    @property
    def _rcx(self):
        return self.rcx.r64
    @_rcx.setter
    def _rcx(self,v):
        self.rcx.r64 = v

    @property
    def _rdx(self):
        return self.rdx.r64
    @_rdx.setter
    def _rdx(self,v):
        self.rdx.r64 = v

    @property
    def _r8(self):
        return self.r8.r64
    @_r8.setter
    def _r8(self,v):
        self.r8.r64 = v

    @property
    def _r9(self):
        return self.r9.r64
    @_r9.setter
    def _r9(self,v):
        self.r9.r64 = v

    @property
    def _r10(self):
        return self.r10.r64
    @_r10.setter
    def _r10(self,v):
        self.r10.r64 = v

    @property
    def _r11(self):
        return self.r11.r64
    @_r11.setter
    def _r11(self,v):
        self.r11.r64 = v

    @property
    def _r12(self):
        return self.r12.r64
    @_r12.setter
    def _r12(self,v):
        self.r12.r64 = v

    @property
    def _r13(self):
        return self.r13.r64
    @_r13.setter
    def _r13(self,v):
        self.r13.r64 = v

    @property
    def _r14(self):
        return self.r14.r64
    @_r14.setter
    def _r14(self,v):
        self.r14.r64 = v

    @property
    def _r15(self):
        return self.r15.r64
    @_r15.setter
    def _r15(self,v):
        self.r15.r64 = v

    @property
    def _rdi(self):
        return self.rdi.r64
    @_rdi.setter
    def _rdi(self,v):
        self.rdi.r64 = v

    @property
    def _rsi(self):
        return self.rsi.r64
    @_rsi.setter
    def _rsi(self,v):
        self.rsi.r64 = v

    @property
    def _rbp(self):
        return self.rbp.r64
    @_rbp.setter
    def _rbp(self,v):
        self.rbp.r64 = v

    @property
    def _rsp(self):
        return self.rsp.r64
    @_rsp.setter
    def _rsp(self,v):
        self.rsp.r64 = v

    # 32
    @property
    def _eax(self):
        return self.rax.r32.l
    @_eax.setter
    def _eax(self,v):
        self.rax.r32.l = v

    @property
    def _ebx(self):
        return self.rbx.r32.l
    @_ebx.setter
    def _ebx(self,v):
        self.rbx.r32.l = v

    @property
    def _ecx(self):
        return self.rcx.r32.l
    @_ecx.setter
    def _ecx(self,v):
        self.rcx.r32.l = v

    @property
    def _edx(self):
        return self.rdx.r32.l
    @_edx.setter
    def _edx(self,v):
        self.rdx.r32.l = v

    @property
    def _r8d(self):
        return self.r8.r32.l
    @_r8d.setter
    def _r8d(self,v):
        self.r8.r32.l = v

    @property
    def _r9d(self):
        return self.r9.r32.l
    @_r9d.setter
    def _r9d(self,v):
        self.r9.r32.l = v

    @property
    def _r10d(self):
        return self.r8.r32.l
    @_r10d.setter
    def _r10d(self,v):
        self.r10.r32.l = v

    @property
    def _r11d(self):
        return self.r11.r32.l
    @_r11d.setter
    def _r11d(self,v):
        self.r11.r32.l = v

    @property
    def _r12d(self):
        return self.r12.r32.l
    @_r12d.setter
    def _r12d(self,v):
        self.r12.r32.l = v

    @property
    def _r13d(self):
        return self.r13.r32.l
    @_r13d.setter
    def _r13d(self,v):
        self.r13.r32.l = v

    @property
    def _r14d(self):
        return self.r14.r32.l
    @_r14d.setter
    def _r14d(self,v):
        self.r14.r32.l = v

    @property
    def _r15d(self):
        return self.r15.r32.l
    @_r15d.setter
    def _r15d(self,v):
        self.r15.r32.l = v

    @property
    def _edi(self):
        return self.rdi.r32.l
    @_edi.setter
    def _edi(self,v):
        self.rdi.r32.l = v

    @property
    def _esi(self):
        return self.rsi.r32.l
    @_esi.setter
    def _esi(self,v):
        self.rsi.r32.l = v

    @property
    def _ebp(self):
        return self.rbp.r32.l
    @_ebp.setter
    def _ebp(self,v):
        self.rsi.r32.l = v

    @property
    def _esp(self):
        return self.rsp.r32.l
    @_esp.setter
    def _esp(self,v):
        self.rsp.r32.l = v

    # 16
    @property
    def _ax(self):
        return self.rax.r16.l
    @_ax.setter
    def _ax(self,v):
        self.rax.r16.l = v

    @property
    def _bx(self):
        return self.rbx.r16.l
    @_bx.setter
    def _bx(self,v):
        self.rbx.r16.l = v

    @property
    def _cx(self):
        return self.rcx.r16.l
    @_cx.setter
    def _cx(self,v):
        self.rcx.r16.l = v

    @property
    def _dx(self):
        return self.rdx.r16.l
    @_dx.setter
    def _dx(self,v):
        self.rdx.r16.l = v

    @property
    def _r8w(self):
        return self.r8.r16.l
    @_r8w.setter
    def _r8w(self,v):
        self.r8.r16.l = v

    @property
    def _r9w(self):
        return self.r9.r16.l
    @_r9w.setter
    def _r9w(self,v):
        self.r9.r16.l = v

    @property
    def _r10w(self):
        return self.r10.r16.l
    @_r10w.setter
    def _r10w(self,v):
        self.r10.r16.l = v

    @property
    def _r11w(self):
        return self.r11.r16.l
    @_r11w.setter
    def _r11w(self,v):
        self.r11.r16.l = v

    @property
    def _r12w(self):
        return self.r12.r16.l
    @_r12w.setter
    def _r12w(self,v):
        self.r12.r16.l = v

    @property
    def _r13w(self):
        return self.r13.r16.l
    @_r13w.setter
    def _r13w(self,v):
        self.r13.r16.l = v

    @property
    def _r14w(self):
        return self.r14.r16.l
    @_r14w.setter
    def _r14w(self,v):
        self.r14.r16.l = v

    @property
    def _r15w(self):
        return self.r15.r16.l
    @_r15w.setter
    def _r15w(self,v):
        self.r15.r16.l = v

    @property
    def _di(self):
        return self.rdi.r16.l
    @_di.setter
    def _di(self,v):
        self.rdi.r16.l = v

    @property
    def _si(self):
        return self.rsi.r16.l
    @_si.setter
    def _si(self,v):
        self.rsi.r16.l = v

    @property
    def _bp(self):
        return self.rbp.r16.l
    @_bp.setter
    def _bp(self,v):
        self.rsi.r16.l = v

    @property
    def _sp(self):
        return self.rsp.r16.l
    @_sp.setter
    def _sp(self,v):
        self.rsp.r16.l = v

    # 8
    @property
    def _al(self):
        return self.rax.r8.l
    @_al.setter
    def _al(self,v):
        self.rax.r8.l = v

    @property
    def _bl(self):
        return self.rbx.r8.l
    @_bl.setter
    def _bl(self,v):
        self.rbx.r8.l = v

    @property
    def _cl(self):
        return self.rcx.r8.l
    @_cl.setter
    def _cl(self,v):
        self.rcx.r8.l = v

    @property
    def _dl(self):
        return self.rdx.r8.l
    @_dl.setter
    def _dl(self,v):
        self.rdx.r8.l = v

    @property
    def _ah(self):
        return self.rax.r8.h
    @_ah.setter
    def _ah(self,v):
        self.rax.r8.h = v

    @property
    def _bh(self):
        return self.rbx.r8.h
    @_bh.setter
    def _bh(self,v):
        self.rbx.r8.h = v

    @property
    def _ch(self):
        return self.rcx.r8.h
    @_ch.setter
    def _ch(self,v):
        self.rcx.r8.h = v

    @property
    def _dh(self):
        return self.rdx.r8.h
    @_dh.setter
    def _dh(self,v):
        self.rdx.r8.h = v

    @property
    def _r8b(self):
        return self.r8.r8.l
    @_r8b.setter
    def _r8b(self,v):
        self.r8.r8.l = v

    @property
    def _r9b(self):
        return self.r9.r8.l
    @_r9b.setter
    def _r9b(self,v):
        self.r9.r8.l = v

    @property
    def _r10b(self):
        return self.r10.r8.l
    @_r10b.setter
    def _r10b(self,v):
        self.r10.r8.l = v

    @property
    def _r11b(self):
        return self.r11.r8.l
    @_r11b.setter
    def _r11b(self,v):
        self.r11.r8.l = v

    @property
    def _r12b(self):
        return self.r12.r8.l
    @_r12b.setter
    def _r12b(self,v):
        self.r12.r8.l = v

    @property
    def _r13b(self):
        return self.r13.r8.l
    @_r13b.setter
    def _r13b(self,v):
        self.r13.r18.l = v

    @property
    def _r14b(self):
        return self.r14.r8.l
    @_r14b.setter
    def _r14b(self,v):
        self.r14.r8.l = v

    @property
    def _r15b(self):
        return self.r15.r8.l
    @_r15b.setter
    def _r15b(self,v):
        self.r15.r8.l = v

    @property
    def _bpl(self):
        return self.rbp.r8.l
    @_bpl.setter
    def _bpl(self,v):
        self.rsi.r8.l = v

    @property
    def _spl(self):
        return self.rsp.r8.l
    @_spl.setter
    def _spl(self,v):
        self.rsp.r8.l = v

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
r.r64 = 0x0102030405060708
r.r64 = r.r64 + 1
s64 = c_int64(r.r64)
print(s64 + 1)
print(hex(r.r64))
print(hex(r.r32.h))
print(hex(r.r32.l))
print(hex(r.r16.h))
print(hex(r.r8.h))
print(hex(r.r8.l))
#test()

cpu = _cpu()
cpu._rax = 0x0123456789ABCDEF
print(hex(cpu._rax))
print(hex(cpu._eax))
print(hex(cpu._ax))
print(hex(cpu._ah))
print(hex(cpu._al))
'''
