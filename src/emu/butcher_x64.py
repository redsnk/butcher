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
    
class _s8(Structure):
    _fields_ = [("l",c_int8),
                ("h",c_int8),
                ("l2",c_int8),
                ("h2",c_int8),
                ("l3",c_int8),
                ("h4",c_int8),
                ("l4",c_int8),
                ("h4",c_int8)]

class _r16(Structure):
    _fields_ = [("l",c_uint16),
                ("h",c_uint16),
                ("l2",c_uint16),
                ("h2",c_uint16)]
    
class _s16(Structure):
    _fields_ = [("l",c_int16),
                ("h",c_int16),
                ("l2",c_int16),
                ("h2",c_int16)]

class _r32(Structure):
    _fields_ = [("l",c_uint32),
                ("h",c_uint32)]

class _s32(Structure):
    _fields_ = [("l",c_int32),
                ("h",c_int32)]

class _reg(Union):
    _fields_ = [("r64",c_uint64),
                ("s64",c_int64),
                ("r32",_r32),
                ("s32",_s32),
                ("r16",_r16),
                ("s16",_s16),
                ("r8",_r8),
                ("s8",_s8)]

class _xmm(Structure):
    _fields_ = [("l",c_uint64),
                ("h",c_uint64)]

class _mem:
    def __init__(self, addr, data, extra_mem):
        self.addr = addr
        self.size = len(data)
        extra = [0] * extra_mem
        self.mem = list(data) + extra
        self.real_size = self.size + extra_mem

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
    _fields_ = [("CF",c_bool),
                ("OF",c_bool),
                ("PF",c_bool),
                ("ZF",c_bool),
                ("SF",c_bool)]

class _cpu:
    b32 = False

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

    xmm0 = _xmm()
    xmm1 = _xmm()
    xmm2 = _xmm()
    xmm3 = _xmm()
    xmm4 = _xmm()
    xmm5 = _xmm()
    xmm6 = _xmm()
    xmm7 = _xmm()

    eflags = _eflags()

    mems = []

    tmp = 0

    EXTRA_MEM = 1024

    def panic(self,text):
        for line in traceback.format_stack():
            print(line.strip())
        print("PANIC: "+text)
        sys.exit(0)

    def locate_mem(self,addr):
        for n in range(len(self.mems)):
            if self.mems[n].addr == addr:
                return n
        return -1        

    def locate_addr_mem(self,addr):
        for n in range(len(self.mems)):
            if (addr >= self.mems[n].addr) and (addr < (self.mems[n].addr+self.mems[n].size)):
                return n
        return -1

    def add_mem(self,addr,data):
        self.mems.append(_mem(addr,data,self.EXTRA_MEM))
        self.sort_mem()

    '''
    void sort_mem (struct _cpu *cpu) {
    int n;
    int lexit;
    struct _mem m;

        do {
            lexit = TRUE;
            for (n=0;n<(cpu->mem_count-1);n++) {
                if (cpu->mems[n].addr > cpu->mems[n+1].addr) {
                    m = cpu->mems[n];
                    cpu->mems[n] = cpu->mems[n+1];
                    cpu->mems[n+1] = m;
                    lexit = FALSE;
                    break;
                }
            }

        }
        while (!lexit);
    }
    '''

    def sort_mem(self):
        while True:
            lexit = True
            for n in range(len(self.mems)-1):
                if self.mems[n].addr > self.mems[n+1].addr:
                    m = self.mems[n]
                    self.mems[n] = self.mems[n+1]
                    self.mems[n+1] = m
                    lexit = False
                    break
            if lexit:
                break


    def add_zero_mem(self,addr,size):
        self.add_mem(addr,[0] * size)

    def get_mem(self,addr,size):
        if self.b32:
            addr = c_uint32(addr).value
        for m in self.mems:
            if (addr >= m.addr) and ((addr+size) <= (m.addr + m.size)):
                start = addr-m.addr
                return m.mem[start:start+size]
        self.panic("get_mem error: "+hex(addr)+":"+str(size))

    def get_mem_dump(self,addr,size):
        n = self.locate_addr_mem(addr)
        if n >= 0:
            last = self.mems[n].addr + self.mems[n].size
            if (addr+size) > last:
                size = last - addr
            return self.get_mem(addr,size)
        return []

    def set_mem(self,addr,data):
        if self.b32:
            addr = c_uint32(addr).value
        size = len(data)
        for m in self.mems:
            if (addr >= m.addr) and ((addr+size) <= (m.addr + m.size)):
                start = addr-m.addr
                pre = m.mem[:start]
                post = m.mem[start+size:]
                m.mem = pre+data+post
                return
        self.panic("set_mem error: "+hex(addr)+":"+str(len(addr)))
    
    '''
    void del_mem(struct _cpu *cpu,int n) {
    int m;

        for (m=n;m<cpu->mem_count-1;m++) {
            cpu->mems[m] = cpu->mems[m+1];
        }
        cpu->mem_count--;
    }
    '''

    def del_mem(self,n):
        for m in range(n,len(self.mems)-1):
            self.mems[m] = self.mems[m+1]
        self.mems = self.mems[0:len(self.mems)-1]

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
        self.add_mem(v_Address,list(data))

    def hexdump (self,addr,mem):
        print(mem)

    def dump_mem (self,addr,size):
        mem = self.get_mem_dump(addr,size)
        if len(mem) > 0:
            self.hexdump(addr,mem)
    
    '''
    uint64_t get_free_chunk(struct _cpu *cpu,int size) {
    uint64_t i_addr,e_addr;
    int n;

        i_addr = 0x1000;
        for (n=0;n<cpu->mem_count;n++) {
            e_addr = i_addr+size+EXTRA_MEM-1;
            if (e_addr < cpu->mems[n].addr) {
                return (i_addr);
            }
            i_addr = cpu->mems[n].addr+cpu->mems[n].real_size;
        }
        return (i_addr);
    }
    '''

    def get_free_chunk (self,size):
        i_addr = 0x1000
        for n in range(len(self.mems)):
            e_addr = i_addr + size + self.EXTRA_MEM-1
            if e_addr < self.mems[n].addr:
                return i_addr
            i_addr = self.mems[n].addr + self.mems[n].real_size
        return i_addr

    '''
    uint64_t alloc_mem (struct _cpu *cpu,int size) {
    uint64_t addr;

        addr = get_free_chunk(cpu,size);
        add_mem(cpu,addr,NULL,size);
        return (addr);
    }

    '''

    def alloc_mem (self,size):
        addr = self.get_free_chunk(size)
        self.add_zero_mem(addr,size)
        return addr
    
    def realloc_mem (self,addr,size):
        n = self.locate_mem(addr)
        if n >= 0:
            # if (size >= cpu->mems[n].real_size) {
            if size >= self.mems[n].real_size:
                ret = self.alloc_mem(size)
                m = self.locate_mem(ret)
                if m >= 0:
                    self.set_mem(ret,self.mems[n].mem)
                    self.del_mem(n)
                    return ret
            else:
                # cpu->mems[n].size = size;
                self.mems[n].size = size
                return addr
        self.panic("realloc_mem error: "+hex(addr)+":"+str(len(addr)))

    def free_mem (self,addr):
        n = self.locate_mem(addr)
        if n >= 0:
            self.del_mem(n)
        else:
            self.panic("free_mem error: "+hex(addr))

    def call_from_iat (self,lib,func):
        self.panic("call_from_iat not implemented - "+func)

    def jmp_from_iat (self,lib,func):
        self.panic("jmp_from_iat not implemented - "+func)

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
    
    def get_dqword_ptr(self,addr):
        data = self.get_mem(addr,16)
        l = (data[7] << 56) | (data[6] << 48) | (data[5] << 40) | (data[4] << 32) | (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0]
        h = (data[15] << 56) | (data[14] << 48) | (data[13] << 40) | (data[12] << 32) | (data[11] << 24) | (data[10] << 16) | (data[9] << 8) | data[8]
        return (l,h)

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
        if type(value) == tuple:
            value = value[0]
        self.set_mem(addr,[value & 0xff])
        self.set_mem(addr+1,[(value >> 8) & 0xff])
        self.set_mem(addr+2,[(value >> 16) & 0xff])
        self.set_mem(addr+3,[(value >> 24) & 0xff])
        self.set_mem(addr+4,[(value >> 32) & 0xff])
        self.set_mem(addr+5,[(value >> 40) & 0xff])
        self.set_mem(addr+6,[(value >> 48) & 0xff])
        self.set_mem(addr+7,[(value >> 56) & 0xff])

    def set_dqword_ptr(self,addr,value):
        self.set_mem(addr,[value[0] & 0xff])
        self.set_mem(addr+1,[(value[0] >> 8) & 0xff])
        self.set_mem(addr+2,[(value[0] >> 16) & 0xff])
        self.set_mem(addr+3,[(value[0] >> 24) & 0xff])
        self.set_mem(addr+4,[(value[0] >> 32) & 0xff])
        self.set_mem(addr+5,[(value[0] >> 40) & 0xff])
        self.set_mem(addr+6,[(value[0] >> 48) & 0xff])
        self.set_mem(addr+7,[(value[0] >> 56) & 0xff])
        self.set_mem(addr+8,[value[1] & 0xff])
        self.set_mem(addr+9,[(value[1] >> 8) & 0xff])
        self.set_mem(addr+10,[(value[1] >> 16) & 0xff])
        self.set_mem(addr+11,[(value[1] >> 24) & 0xff])
        self.set_mem(addr+12,[(value[1] >> 32) & 0xff])
        self.set_mem(addr+13,[(value[1] >> 40) & 0xff])
        self.set_mem(addr+14,[(value[1] >> 48) & 0xff])
        self.set_mem(addr+15,[(value[1] >> 56) & 0xff])

    def set_unicode_ptr(self,addr,str):
        n = 0
        for c in str:
            self.set_word_ptr(addr+(n*2),ord(c))
            n += 1

    def get_unicode_ptr(self,addr):
        n = 0
        str = ""
        while True:
            c = self.get_byte_ptr(addr)
            if c > 0:
                str += chr(c)
                addr += 2
            else:
                break
        return str

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
        
    def flag_z(self,b):
        self.eflags.ZF = b
    
    def get_flag_z(self):
        return self.eflags.ZF

    def flag_c(self,b):
        self.eflags.CF = b
    
    def get_flag_c(self):
        return self.eflags.CF
    '''
    def num_flag_c(self):
        if self.eflags.CF:
            return 1
        return 0
    '''
    def flag_o(self,b):
        self.eflags.OF = b

    def get_flag_o(self):
        return self.eflags.OF
    
    def flag_s(self,b):
        self.eflags.SF = b

    def get_flag_s(self):
        return self.eflags.SF

    def sign(self,n):
        return (n < 0)

    def add_flag_o(self,bits,op1,op2):
        self.flag_o(False)
        if bits == 8:
            sop1 = c_int8(op1).value
            sop2 = c_int8(op2).value         
        elif bits == 16:
            sop1 = c_int16(op1).value
            sop2 = c_int16(op2).value
        elif bits == 32:
            sop1 = c_int32(op1).value
            sop2 = c_int32(op2).value
        else:
            sop1 = c_int64(op1).value
            sop2 = c_int64(op2).value
        if self.sign(sop1) == self.sign(sop2):
            self.flag_o(self.sign(sop1+sop2) != self.sign(sop1))
    
    def sub_flag_o(self,bits,op1,op2):
        self.flag_o(False)
        if bits == 8:
            sop1 = c_int8(op1).value
            sop2 = c_int8(op2).value         
        elif bits == 16:
            sop1 = c_int16(op1).value
            sop2 = c_int16(op2).value
        elif bits == 32:
            sop1 = c_int32(op1).value
            sop2 = c_int32(op2).value
        else:
            sop1 = c_int64(op1).value
            sop2 = c_int64(op2).value
        if self.sign(sop1) != self.sign(sop2):
            self.flag_o(self.sign(sop1-sop2) != self.sign(sop1))

    def add_flag_c(self,bits,op1,op2):
        t1 = c_uint64(op1).value
        t2 = c_uint64(op2).value
        t = c_uint64(t1 + t2).value
        if bits == 8:
            self.flag_c(t > 0xff)
        elif bits == 16:
            self.flag_c(t > 0xffff)
        elif bits == 32:
            self.flag_c(t > 0xffffffff)
        else:
            t = (t1 & 0xffffffff) + (t2 & 0xffffffff)
            r = t >> 32
            t = (t1 >> 32) + (t2 >> 32) + r
            self.flag_c(t > 0xffffffff)

    def f_not(self,b,p):
        return ~c_uint64(p).value

    def f_neg(self,b,p):
        return ~c_uint64(p).value + c_uint64(1).value

    def idiv(self,a,b):
        return int(a/b)
        
    def pow(self,b,p):
        r = 1
        for n in range(p):
            r = r * p
        return r

    def pshufd(self,op1,op2):
        #l = c_uint64(0).value
        #h = c_uint64(0).value
        l = 0
        h = 0
        c = op2 & 0b00000011
        if c < 2:
            d =  (op1[0] >> (32*c)) & 0xffffffff
        else:
            d =  (op1[1] >> (32*(c-2))) & 0xffffffff
        l = l | d
        c = (op2 & 0b00001100) >> 2
        if c < 2:
            d =  ((op1[0] >> (32*c)) & 0xffffffff) << 32
        else:
            d =  ((op1[1] >> (32*(c-2))) & 0xffffffff) << 32
        l = l | d
        c = (op2 & 0b00110000) >> 4
        if c < 2:
            d =  (op1[0] >> (32*c)) & 0xffffffff
        else:
            d =  (op1[1] >> (32*(c-2))) & 0xffffffff
        h = h | d
        c = (op2 & 0b11000000) >> 6
        if c < 2:
            d =  ((op1[0] >> (32*c)) & 0xffffffff) << 32
        else:
            d =  ((op1[1] >> (32*(c-2))) & 0xffffffff) << 32
        h = h | d
        return (l,h)

    def mask(self,bits):
        v = (1 << bits)
        v = v - 1
        return v

    #---------------------------------------------------------------
    # 64
    @property
    def _rax(self):
        return self.rax.r64
    @_rax.setter
    def _rax(self,v):
        if type(v) == tuple:
            self.rax.r64 = v[0]
        else:
            self.rax.r64 = v

    @property
    def s_rax(self):
        return self.rax.s64
    @s_rax.setter
    def s_rax(self,v):
        if type(v) == tuple:
            self.rax.s64 = v[0]
        else:
            self.rax.s64 = v

    @property
    def _rbx(self):
        return self.rbx.r64
    @_rbx.setter
    def _rbx(self,v):
        if type(v) == tuple:
            self.rbx.r64 = v[0]
        else:
            self.rbx.r64 = v

    @property
    def s_rbx(self):
        return self.rbx.s64
    @s_rbx.setter
    def s_rbx(self,v):
        if type(v) == tuple:
            self.rbx.s64 = v[0]
        else:
            self.rbx.s64 = v

    @property
    def _rcx(self):
        return self.rcx.r64
    @_rcx.setter
    def _rcx(self,v):
        if type(v) == tuple:
            self.rcx.r64 = v[0]
        else:
            self.rcx.r64 = v

    @property
    def s_rcx(self):
        return self.rcx.s64
    @s_rcx.setter
    def s_rcx(self,v):
        if type(v) == tuple:
            self.rcx.s64 = v[0]
        else:
            self.rcx.s64 = v

    @property
    def _rdx(self):
        return self.rdx.r64
    @_rdx.setter
    def _rdx(self,v):
        if type(v) == tuple:
            self.rdx.r64 = v[0]
        else:
            self.rdx.r64 = v

    @property
    def s_rdx(self):
        return self.rdx.s64
    @s_rdx.setter
    def s_rdx(self,v):
        if type(v) == tuple:
            self.rdx.s64 = v[0]
        else:
            self.rdx.s64 = v

    @property
    def _r8(self):
        return self.r8.r64
    @_r8.setter
    def _r8(self,v):
        if type(v) == tuple:
            self.r8.r64 = v[0]
        else:
            self.r8.r64 = v

    @property
    def s_r8(self):
        return self.r8.s64
    @s_r8.setter
    def s_r8(self,v):
        if type(v) == tuple:
            self.r8.s64 = v[0]
        else:
            self.r8.s64 = v

    @property
    def _r9(self):
        return self.r9.r64
    @_r9.setter
    def _r9(self,v):
        if type(v) == tuple:
            self.r9.r64 = v[0]
        else:
            self.r9.r64 = v

    @property
    def s_r9(self):
        return self.r9.s64
    @s_r9.setter
    def s_r9(self,v):
        if type(v) == tuple:
            self.r9.s64 = v[0]
        else:
            self.r9.s64 = v

    @property
    def _r10(self):
        return self.r10.r64
    @_r10.setter
    def _r10(self,v):
        if type(v) == tuple:
            self.r10.r64 = v[0]
        else:
            self.r10.r64 = v

    @property
    def s_r10(self):
        return self.r10.s64
    @s_r10.setter
    def s_r10(self,v):
        if type(v) == tuple:
            self.r10.s64 = v[0]
        else:
            self.r10.s64 = v

    @property
    def _r11(self):
        return self.r11.r64
    @_r11.setter
    def _r11(self,v):
        if type(v) == tuple:
            self.r11.r64 = v[0]
        else:
            self.r11.r64 = v

    @property
    def s_r11(self):
        return self.r11.s64
    @s_r11.setter
    def s_r11(self,v):
        if type(v) == tuple:
            self.r11.s64 = v[0]
        else:
            self.r11.s64 = v

    @property
    def _r12(self):
        return self.r12.r64
    @_r12.setter
    def _r12(self,v):
        if type(v) == tuple:
            self.r12.r64 = v[0]
        else:
            self.r12.r64 = v

    @property
    def s_r12(self):
        return self.r12.s64
    @s_r12.setter
    def s_r12(self,v):
        if type(v) == tuple:
            self.r12.s64 = v[0]
        else:
            self.r12.s64 = v

    @property
    def _r13(self):
        return self.r13.r64
    @_r13.setter
    def _r13(self,v):
        if type(v) == tuple:
            self.r13.r64 = v[0]
        else:
            self.r13.r64 = v

    @property
    def s_r13(self):
        return self.r13.s64
    @s_r13.setter
    def s_r13(self,v):
        if type(v) == tuple:
            self.r13.s64 = v[0]
        else:
            self.r13.s64 = v

    @property
    def _r14(self):
        return self.r14.r64
    @_r14.setter
    def _r14(self,v):
        if type(v) == tuple:
            self.r14.r64 = v[0]
        else:
            self.r14.r64 = v

    @property
    def s_r14(self):
        return self.r14.s64
    @s_r14.setter
    def s_r14(self,v):
        if type(v) == tuple:
            self.r14.s64 = v[0]
        else:
            self.r14.s64 = v

    @property
    def _r15(self):
        return self.r15.r64
    @_r15.setter
    def _r15(self,v):
        if type(v) == tuple:
            self.r15.r64 = v[0]
        else:
            self.r15.r64 = v

    @property
    def s_r15(self):
        return self.r15.s64
    @s_r15.setter
    def s_r15(self,v):
        if type(v) == tuple:
            self.r15.s64 = v[0]
        else:
            self.r15.s64 = v

    @property
    def _rsi(self):
        return self.rsi.r64
    @_rsi.setter
    def _rsi(self,v):
        if type(v) == tuple:
            self.rsi.r64 = v[0]
        else:
            self.rsi.r64 = v

    @property
    def s_rsi(self):
        return self.rsi.s64
    @s_rsi.setter
    def s_rsi(self,v):
        if type(v) == tuple:
            self.rsi.s64 = v[0]
        else:
            self.rsi.s64 = v

    @property
    def _rdi(self):
        return self.rdi.r64
    @_rdi.setter
    def _rdi(self,v):
        if type(v) == tuple:
            self.rdi.r64 = v[0]
        else:
            self.rdi.r64 = v

    @property
    def s_rdi(self):
        return self.rdi.s64
    @s_rdi.setter
    def s_rdi(self,v):
        if type(v) == tuple:
            self.rdi.s64 = v[0]
        else:
            self.rdi.s64 = v

    @property
    def _rbp(self):
        return self.rbp.r64
    @_rbp.setter
    def _rbp(self,v):
        if type(v) == tuple:
            self.rbp.r64 = v[0]
        else:
            self.rbp.r64 = v

    @property
    def s_rbp(self):
        return self.rbp.s64
    @s_rbp.setter
    def s_rbp(self,v):
        if type(v) == tuple:
            self.rbp.s64 = v[0]
        else:
            self.rbp.s64 = v

    @property
    def _rsp(self):
        return self.rsp.r64
    @_rsp.setter
    def _rsp(self,v):
        if type(v) == tuple:
            self.rsp.r64 = v[0]
        else:
            self.rsp.r64 = v

    @property
    def s_rsp(self):
        return self.rsp.s64
    @s_rsp.setter
    def s_rsp(self,v):
        if type(v) == tuple:
            self.rsp.s64 = v[0]
        else:
            self.rsp.s64 = v

    # 32
    @property
    def _eax(self):
        return self.rax.r32.l
    @_eax.setter
    def _eax(self,v):
        if type(v) == tuple:
            self.rax.r32.l = v[0]
        else:
            self.rax.r32.l = v

    @property
    def s_eax(self):
        return self.rax.s32.l
    @s_eax.setter
    def s_eax(self,v):
        if type(v) == tuple:
            self.rax.s32.l = v[0]
        else:
            self.rax.s32.l = v

    @property
    def _ebx(self):
        return self.rbx.r32.l
    @_ebx.setter
    def _ebx(self,v):
        if type(v) == tuple:
            self.rbx.r32.l = v[0]
        else:
            self.rbx.r32.l = v

    @property
    def s_ebx(self):
        return self.rbx.s32.l
    @s_ebx.setter
    def s_ebx(self,v):
        if type(v) == tuple:
            self.rbx.s32.l = v[0]
        else:
            self.rbx.s32.l = v

    @property
    def _ecx(self):
        return self.rcx.r32.l
    @_ecx.setter
    def _ecx(self,v):
        if type(v) == tuple:
            self.rcx.r32.l = v[0]
        else:
            self.rcx.r32.l = v

    @property
    def s_ecx(self):
        return self.rcx.s32.l
    @s_ecx.setter
    def s_ecx(self,v):
        if type(v) == tuple:
            self.rcx.s32.l = v[0]
        else:
            self.rcx.s32.l = v

    @property
    def _edx(self):
        return self.rdx.r32.l
    @_edx.setter
    def _edx(self,v):
        if type(v) == tuple:
            self.rdx.r32.l = v[0]
        else:
            self.rdx.r32.l = v

    @property
    def s_edx(self):
        return self.rdx.s32.l
    @s_edx.setter
    def s_edx(self,v):
        if type(v) == tuple:
            self.rdx.s32.l = v[0]
        else:
            self.rdx.s32.l = v

    @property
    def _r8d(self):
        return self.r8.r32.l
    @_r8d.setter
    def _r8d(self,v):
        if type(v) == tuple:
            self.r8.r32.l = v[0]
        else:
            self.r8.r32.l = v

    @property
    def s_r8d(self):
        return self.r8.s32.l
    @s_r8d.setter
    def s_r8d(self,v):
        if type(v) == tuple:
            self.r8.s32.l = v[0]
        else:
            self.r8.s32.l = v

    @property
    def _r9d(self):
        return self.r9.r32.l
    @_r9d.setter
    def _r9d(self,v):
        if type(v) == tuple:
            self.r9.r32.l = v[0]
        else:
            self.r9.r32.l = v

    @property
    def s_r9d(self):
        return self.r9.s32.l
    @s_r9d.setter
    def s_r9d(self,v):
        if type(v) == tuple:
            self.r9.s32.l = v[0]
        else:
            self.r9.s32.l = v

    @property
    def _r10d(self):
        return self.r10.r32.l
    @_r10d.setter
    def _r10d(self,v):
        if type(v) == tuple:
            self.r10.r32.l = v[0]
        else:
            self.r10.r32.l = v

    @property
    def s_r10d(self):
        return self.r10.s32.l
    @s_r10d.setter
    def s_r10d(self,v):
        if type(v) == tuple:
            self.r10.s32.l = v[0]
        else:
            self.r10.s32.l = v

    @property
    def _r11d(self):
        return self.r11.r32.l
    @_r11d.setter
    def _r11d(self,v):
        if type(v) == tuple:
            self.r11.r32.l = v[0]
        else:
            self.r11.r32.l = v

    @property
    def s_r11d(self):
        return self.r11.s32.l
    @s_r11d.setter
    def s_r11d(self,v):
        if type(v) == tuple:
            self.r11.s32.l = v[0]
        else:
            self.r11.s32.l = v

    @property
    def _r12d(self):
        return self.r12.r32.l
    @_r12d.setter
    def _r12d(self,v):
        if type(v) == tuple:
            self.r12.r32.l = v[0]
        else:
            self.r12.r32.l = v

    @property
    def s_r12d(self):
        return self.r12.s32.l
    @s_r12d.setter
    def s_r12d(self,v):
        if type(v) == tuple:
            self.r12.s32.l = v[0]
        else:
            self.r12.s32.l = v

    @property
    def _r13d(self):
        return self.r13.r32.l
    @_r13d.setter
    def _r13d(self,v):
        if type(v) == tuple:
            self.r13.r32.l = v[0]
        else:
            self.r13.r32.l = v

    @property
    def s_r13d(self):
        return self.r13.s32.l
    @s_r13d.setter
    def s_r13d(self,v):
        if type(v) == tuple:
            self.r13.s32.l = v[0]
        else:
            self.r13.s32.l = v

    @property
    def _r14d(self):
        return self.r14.r32.l
    @_r14d.setter
    def _r14d(self,v):
        if type(v) == tuple:
            self.r14.r32.l = v[0]
        else:
            self.r14.r32.l = v

    @property
    def s_r14d(self):
        return self.r14.s32.l
    @s_r14d.setter
    def s_r14d(self,v):
        if type(v) == tuple:
            self.r14.s32.l = v[0]
        else:
            self.r14.s32.l = v

    @property
    def _r15d(self):
        return self.r15.r32.l
    @_r15d.setter
    def _r15d(self,v):
        if type(v) == tuple:
            self.r15.r32.l = v[0]
        else:
            self.r15.r32.l = v

    @property
    def s_r15d(self):
        return self.r15.s32.l
    @s_r15d.setter
    def s_r15d(self,v):
        if type(v) == tuple:
            self.r15.s32.l = v[0]
        else:
            self.r15.s32.l = v

    @property
    def _esi(self):
        return self.rsi.r32.l
    @_esi.setter
    def _esi(self,v):
        if type(v) == tuple:
            self.rsi.r32.l = v[0]
        else:
            self.rsi.r32.l = v

    @property
    def s_esi(self):
        return self.rsi.s32.l
    @s_esi.setter
    def s_esi(self,v):
        if type(v) == tuple:
            self.rsi.s32.l = v[0]
        else:
            self.rsi.s32.l = v

    @property
    def _edi(self):
        return self.rdi.r32.l
    @_edi.setter
    def _edi(self,v):
        if type(v) == tuple:
            self.rdi.r32.l = v[0]
        else:
            self.rdi.r32.l = v

    @property
    def s_edi(self):
        return self.rdi.s32.l
    @s_edi.setter
    def s_edi(self,v):
        if type(v) == tuple:
            self.rdi.s32.l = v[0]
        else:
            self.rdi.s32.l = v

    @property
    def _ebp(self):
        return self.rbp.r32.l
    @_ebp.setter
    def _ebp(self,v):
        if type(v) == tuple:
            self.rbp.r32.l = v[0]
        else:
            self.rbp.r32.l = v

    @property
    def s_ebp(self):
        return self.rbp.s32.l
    @s_ebp.setter
    def s_ebp(self,v):
        if type(v) == tuple:
            self.rbp.s32.l = v[0]
        else:
            self.rbp.s32.l = v

    @property
    def _esp(self):
        return self.rsp.r32.l
    @_esp.setter
    def _esp(self,v):
        if type(v) == tuple:
            self.rsp.r32.l = v[0]
        else:
            self.rsp.r32.l = v

    @property
    def s_esp(self):
        return self.rsp.s32.l
    @s_esp.setter
    def s_esp(self,v):
        if type(v) == tuple:
            self.rsp.s32.l = v[0]
        else:
            self.rsp.s32.l = v

    # 16
    @property
    def _ax(self):
        return self.rax.r16.l
    @_ax.setter
    def _ax(self,v):
        if type(v) == tuple:
            self.rax.r16.l = v[0]
        else:
            self.rax.r16.l = v

    @property
    def s_ax(self):
        return self.rax.s16.l
    @s_ax.setter
    def s_ax(self,v):
        if type(v) == tuple:
            self.rax.s16.l = v[0]
        else:
            self.rax.s16.l = v

    @property
    def _bx(self):
        return self.rbx.r16.l
    @_bx.setter
    def _bx(self,v):
        if type(v) == tuple:
            self.rbx.r16.l = v[0]
        else:
            self.rbx.r16.l = v

    @property
    def s_bx(self):
        return self.rbx.s16.l
    @s_bx.setter
    def s_bx(self,v):
        if type(v) == tuple:
            self.rbx.s16.l = v[0]
        else:
            self.rbx.s16.l = v

    @property
    def _cx(self):
        return self.rcx.r16.l
    @_cx.setter
    def _cx(self,v):
        if type(v) == tuple:
            self.rcx.r16.l = v[0]
        else:
            self.rcx.r16.l = v

    @property
    def s_cx(self):
        return self.rcx.s16.l
    @s_cx.setter
    def s_cx(self,v):
        if type(v) == tuple:
            self.rcx.s16.l = v[0]
        else:
            self.rcx.s16.l = v

    @property
    def _dx(self):
        return self.rdx.r16.l
    @_dx.setter
    def _dx(self,v):
        if type(v) == tuple:
            self.rdx.r16.l = v[0]
        else:
            self.rdx.r16.l = v

    @property
    def s_dx(self):
        return self.rdx.s16.l
    @s_dx.setter
    def s_dx(self,v):
        if type(v) == tuple:
            self.rdx.s16.l = v[0]
        else:
            self.rdx.s16.l = v

    @property
    def _r8w(self):
        return self.r8.r16.l
    @_r8w.setter
    def _r8w(self,v):
        if type(v) == tuple:
            self.r8.r16.l = v[0]
        else:
            self.r8.r16.l = v

    @property
    def s_r8w(self):
        return self.r8.s16.l
    @s_r8w.setter
    def s_r8w(self,v):
        if type(v) == tuple:
            self.r8.s16.l = v[0]
        else:
            self.r8.s16.l = v

    @property
    def _r9w(self):
        return self.r9.r16.l
    @_r9w.setter
    def _r9w(self,v):
        if type(v) == tuple:
            self.r9.r16.l = v[0]
        else:
            self.r9.r16.l = v

    @property
    def s_r9w(self):
        return self.r9.s16.l
    @s_r9w.setter
    def s_r9w(self,v):
        if type(v) == tuple:
            self.r9.s16.l = v[0]
        else:
            self.r9.s16.l = v

    @property
    def _r10w(self):
        return self.r10.r16.l
    @_r10w.setter
    def _r10w(self,v):
        if type(v) == tuple:
            self.r10.r16.l = v[0]
        else:
            self.r10.r16.l = v

    @property
    def s_r10w(self):
        return self.r10.s16.l
    @s_r10w.setter
    def s_r10w(self,v):
        if type(v) == tuple:
            self.r10.s16.l = v[0]
        else:
            self.r10.s16.l = v

    @property
    def _r11w(self):
        return self.r11.r16.l
    @_r11w.setter
    def _r11w(self,v):
        if type(v) == tuple:
            self.r11.r16.l = v[0]
        else:
            self.r11.r16.l = v

    @property
    def s_r11w(self):
        return self.r11.s16.l
    @s_r11w.setter
    def s_r11w(self,v):
        if type(v) == tuple:
            self.r11.s16.l = v[0]
        else:
            self.r11.s16.l = v

    @property
    def _r12w(self):
        return self.r12.r16.l
    @_r12w.setter
    def _r12w(self,v):
        if type(v) == tuple:
            self.r12.r16.l = v[0]
        else:
            self.r12.r16.l = v

    @property
    def s_r12w(self):
        return self.r12.s16.l
    @s_r12w.setter
    def s_r12w(self,v):
        if type(v) == tuple:
            self.r12.s16.l = v[0]
        else:
            self.r12.s16.l = v

    @property
    def _r13w(self):
        return self.r13.r16.l
    @_r13w.setter
    def _r13w(self,v):
        if type(v) == tuple:
            self.r13.r16.l = v[0]
        else:
            self.r13.r16.l = v

    @property
    def s_r13w(self):
        return self.r13.s16.l
    @s_r13w.setter
    def s_r13w(self,v):
        if type(v) == tuple:
            self.r13.s16.l = v[0]
        else:
            self.r13.s16.l = v

    @property
    def _r14w(self):
        return self.r14.r16.l
    @_r14w.setter
    def _r14w(self,v):
        if type(v) == tuple:
            self.r14.r16.l = v[0]
        else:
            self.r14.r16.l = v

    @property
    def s_r14w(self):
        return self.r14.s16.l
    @s_r14w.setter
    def s_r14w(self,v):
        if type(v) == tuple:
            self.r14.s16.l = v[0]
        else:
            self.r14.s16.l = v

    @property
    def _r15w(self):
        return self.r15.r16.l
    @_r15w.setter
    def _r15w(self,v):
        if type(v) == tuple:
            self.r15.r16.l = v[0]
        else:
            self.r15.r16.l = v

    @property
    def s_r15w(self):
        return self.r15.s16.l
    @s_r15w.setter
    def s_r15w(self,v):
        if type(v) == tuple:
            self.r15.s16.l = v[0]
        else:
            self.r15.s16.l = v

    @property
    def _si(self):
        return self.rsi.r16.l
    @_si.setter
    def _si(self,v):
        if type(v) == tuple:
            self.rsi.r16.l = v[0]
        else:
            self.rsi.r16.l = v

    @property
    def s_si(self):
        return self.rsi.s16.l
    @s_si.setter
    def s_si(self,v):
        if type(v) == tuple:
            self.rsi.s16.l = v[0]
        else:
            self.rsi.s16.l = v

    @property
    def _di(self):
        return self.rdi.r16.l
    @_di.setter
    def _di(self,v):
        if type(v) == tuple:
            self.rdi.r16.l = v[0]
        else:
            self.rdi.r16.l = v

    @property
    def s_di(self):
        return self.rdi.s16.l
    @s_di.setter
    def s_di(self,v):
        if type(v) == tuple:
            self.rdi.s16.l = v[0]
        else:
            self.rdi.s16.l = v

    @property
    def _bp(self):
        return self.rbp.r16.l
    @_bp.setter
    def _bp(self,v):
        if type(v) == tuple:
            self.rbp.r16.l = v[0]
        else:
            self.rbp.r16.l = v

    @property
    def s_bp(self):
        return self.rbp.s16.l
    @s_bp.setter
    def s_bp(self,v):
        if type(v) == tuple:
            self.rbp.s16.l = v[0]
        else:
            self.rbp.s16.l = v

    @property
    def _sp(self):
        return self.rsp.r16.l
    @_sp.setter
    def _sp(self,v):
        if type(v) == tuple:
            self.rsp.r16.l = v[0]
        else:
            self.rsp.r16.l = v

    @property
    def s_sp(self):
        return self.rsp.s16.l
    @s_sp.setter
    def s_sp(self,v):
        if type(v) == tuple:
            self.rsp.s16.l = v[0]
        else:
            self.rsp.s16.l = v

    @property
    def _al(self):
        return self.rax.r8.l
    @_al.setter
    def _al(self,v):
        if type(v) == tuple:
            self.rax.r8.l = v[0]
        else:
            self.rax.r8.l = v

    @property
    def s_al(self):
        return self.rax.s8.l
    @s_al.setter
    def s_al(self,v):
        if type(v) == tuple:
            self.rax.s8.l = v[0]
        else:
            self.rax.s8.l = v

    @property
    def _bl(self):
        return self.rbx.r8.l
    @_bl.setter
    def _bl(self,v):
        if type(v) == tuple:
            self.rbx.r8.l = v[0]
        else:
            self.rbx.r8.l = v

    @property
    def s_bl(self):
        return self.rbx.s8.l
    @s_bl.setter
    def s_bl(self,v):
        if type(v) == tuple:
            self.rbx.s8.l = v[0]
        else:
            self.rbx.s8.l = v

    @property
    def _cl(self):
        return self.rcx.r8.l
    @_cl.setter
    def _cl(self,v):
        if type(v) == tuple:
            self.rcx.r8.l = v[0]
        else:
            self.rcx.r8.l = v

    @property
    def s_cl(self):
        return self.rcx.s8.l
    @s_cl.setter
    def s_cl(self,v):
        if type(v) == tuple:
            self.rcx.s8.l = v[0]
        else:
            self.rcx.s8.l = v

    @property
    def _dl(self):
        return self.rdx.r8.l
    @_dl.setter
    def _dl(self,v):
        if type(v) == tuple:
            self.rdx.r8.l = v[0]
        else:
            self.rdx.r8.l = v

    @property
    def s_dl(self):
        return self.rdx.s8.l
    @s_dl.setter
    def s_dl(self,v):
        if type(v) == tuple:
            self.rdx.s8.l = v[0]
        else:
            self.rdx.s8.l = v

    @property
    def _r8b(self):
        return self.r8.r8.l
    @_r8b.setter
    def _r8b(self,v):
        if type(v) == tuple:
            self.r8.r8.l = v[0]
        else:
            self.r8.r8.l = v

    @property
    def s_r8b(self):
        return self.r8.s8.l
    @s_r8b.setter
    def s_r8b(self,v):
        if type(v) == tuple:
            self.r8.s8.l = v[0]
        else:
            self.r8.s8.l = v

    @property
    def _r9b(self):
        return self.r9.r8.l
    @_r9b.setter
    def _r9b(self,v):
        if type(v) == tuple:
            self.r9.r8.l = v[0]
        else:
            self.r9.r8.l = v

    @property
    def s_r9b(self):
        return self.r9.s8.l
    @s_r9b.setter
    def s_r9b(self,v):
        if type(v) == tuple:
            self.r9.s8.l = v[0]
        else:
            self.r9.s8.l = v

    @property
    def _r10b(self):
        return self.r10.r8.l
    @_r10b.setter
    def _r10b(self,v):
        if type(v) == tuple:
            self.r10.r8.l = v[0]
        else:
            self.r10.r8.l = v

    @property
    def s_r10b(self):
        return self.r10.s8.l
    @s_r10b.setter
    def s_r10b(self,v):
        if type(v) == tuple:
            self.r10.s8.l = v[0]
        else:
            self.r10.s8.l = v

    @property
    def _r11b(self):
        return self.r11.r8.l
    @_r11b.setter
    def _r11b(self,v):
        if type(v) == tuple:
            self.r11.r8.l = v[0]
        else:
            self.r11.r8.l = v

    @property
    def s_r11b(self):
        return self.r11.s8.l
    @s_r11b.setter
    def s_r11b(self,v):
        if type(v) == tuple:
            self.r11.s8.l = v[0]
        else:
            self.r11.s8.l = v

    @property
    def _r12b(self):
        return self.r12.r8.l
    @_r12b.setter
    def _r12b(self,v):
        if type(v) == tuple:
            self.r12.r8.l = v[0]
        else:
            self.r12.r8.l = v

    @property
    def s_r12b(self):
        return self.r12.s8.l
    @s_r12b.setter
    def s_r12b(self,v):
        if type(v) == tuple:
            self.r12.s8.l = v[0]
        else:
            self.r12.s8.l = v

    @property
    def _r13b(self):
        return self.r13.r8.l
    @_r13b.setter
    def _r13b(self,v):
        if type(v) == tuple:
            self.r13.r8.l = v[0]
        else:
            self.r13.r8.l = v

    @property
    def s_r13b(self):
        return self.r13.s8.l
    @s_r13b.setter
    def s_r13b(self,v):
        if type(v) == tuple:
            self.r13.s8.l = v[0]
        else:
            self.r13.s8.l = v

    @property
    def _r14b(self):
        return self.r14.r8.l
    @_r14b.setter
    def _r14b(self,v):
        if type(v) == tuple:
            self.r14.r8.l = v[0]
        else:
            self.r14.r8.l = v

    @property
    def s_r14b(self):
        return self.r14.s8.l
    @s_r14b.setter
    def s_r14b(self,v):
        if type(v) == tuple:
            self.r14.s8.l = v[0]
        else:
            self.r14.s8.l = v

    @property
    def _r15b(self):
        return self.r15.r8.l
    @_r15b.setter
    def _r15b(self,v):
        if type(v) == tuple:
            self.r15.r8.l = v[0]
        else:
            self.r15.r8.l = v

    @property
    def s_r15b(self):
        return self.r15.s8.l
    @s_r15b.setter
    def s_r15b(self,v):
        if type(v) == tuple:
            self.r15.s8.l = v[0]
        else:
            self.r15.s8.l = v

    @property
    def _bpl(self):
        return self.rbp.r8.l
    @_bpl.setter
    def _bpl(self,v):
        if type(v) == tuple:
            self.rbp.r8.l = v[0]
        else:
            self.rbp.r8.l = v

    @property
    def s_bpl(self):
        return self.rbp.s8.l
    @s_bpl.setter
    def s_bpl(self,v):
        if type(v) == tuple:
            self.rbp.s8.l = v[0]
        else:
            self.rbp.s8.l = v

    @property
    def _spl(self):
        return self.rsp.r8.l
    @_spl.setter
    def _spl(self,v):
        if type(v) == tuple:
            self.rsp.r8.l = v[0]
        else:
            self.rsp.r8.l = v

    @property
    def s_spl(self):
        return self.rsp.s8.l
    @s_spl.setter
    def s_spl(self,v):
        if type(v) == tuple:
            self.rsp.s8.l = v[0]
        else:
            self.rsp.s8.l = v

    # 8
    @property
    def _ah(self):
        return self.rax.r8.h
    @_ah.setter
    def _ah(self,v):
        if type(v) == tuple:
            self.rax.r8.h = v[0]
        else:
            self.rax.r8.h = v

    @property
    def s_ah(self):
        return self.rax.s8.h
    @s_ah.setter
    def s_ah(self,v):
        if type(v) == tuple:
            self.rax.s8.h = v[0]
        else:
            self.rax.s8.h = v

    @property
    def _bh(self):
        return self.rbx.r8.h
    @_bh.setter
    def _bh(self,v):
        if type(v) == tuple:
            self.rbx.r8.h = v[0]
        else:
            self.rbx.r8.h = v

    @property
    def s_bh(self):
        return self.rbx.s8.h
    @s_bh.setter
    def s_bh(self,v):
        if type(v) == tuple:
            self.rbx.s8.h = v[0]
        else:
            self.rbx.s8.h = v

    @property
    def _ch(self):
        return self.rcx.r8.h
    @_ch.setter
    def _ch(self,v):
        if type(v) == tuple:
            self.rcx.r8.h = v[0]
        else:
            self.rcx.r8.h = v

    @property
    def s_ch(self):
        return self.rcx.s8.h
    @s_ch.setter
    def s_ch(self,v):
        if type(v) == tuple:
            self.rcx.s8.h = v[0]
        else:
            self.rcx.s8.h = v

    @property
    def _dh(self):
        return self.rdx.r8.h
    @_dh.setter
    def _dh(self,v):
        if type(v) == tuple:
            self.rdx.r8.h = v[0]
        else:
            self.rdx.r8.h = v

    @property
    def s_dh(self):
        return self.rdx.s8.h
    @s_dh.setter
    def s_dh(self,v):
        if type(v) == tuple:
            self.rdx.s8.h = v[0]
        else:
            self.rdx.s8.h = v

    @property
    def _xmm0(self):
        return (self.xmm0.l,self.xmm0.h)
    @_xmm0.setter
    def _xmm0(self,v):
        if type(v) == tuple:
            self.xmm0.l = v[0]
            self.xmm0.h = v[1]
        else:
            self.xmm0.l = v
            self.xmm0.h = 0

    @property
    def _xmm1(self):
        return (self.xmm1.l,self.xmm1.h)
    @_xmm1.setter
    def _xmm1(self,v):
        if type(v) == tuple:
            self.xmm1.l = v[0]
            self.xmm1.h = v[1]
        else:
            self.xmm1.l = v
            self.xmm1.h = 0

    @property
    def _xmm2(self):
        return (self.xmm2.l,self.xmm2.h)
    @_xmm2.setter
    def _xmm2(self,v):
        if type(v) == tuple:
            self.xmm2.l = v[0]
            self.xmm2.h = v[1]
        else:
            self.xmm2.l = v
            self.xmm2.h = 0

    @property
    def _xmm3(self):
        return (self.xmm3.l,self.xmm3.h)
    @_xmm3.setter
    def _xmm3(self,v):
        if type(v) == tuple:
            self.xmm3.l = v[0]
            self.xmm3.h = v[1]
        else:
            self.xmm3.l = v
            self.xmm3.h = 0

    @property
    def _xmm4(self):
        return (self.xmm4.l,self.xmm4.h)
    @_xmm4.setter
    def _xmm4(self,v):
        if type(v) == tuple:
            self.xmm4.l = v[0]
            self.xmm4.h = v[1]
        else:
            self.xmm4.l = v
            self.xmm4.h = 0

    @property
    def _xmm5(self):
        return (self.xmm5.l,self.xmm5.h)
    @_xmm5.setter
    def _xmm5(self,v):
        if type(v) == tuple:
            self.xmm5.l = v[0]
            self.xmm5.h = v[1]
        else:
            self.xmm5.l = v
            self.xmm5.h = 0

    @property
    def _xmm6(self):
        return (self.xmm6.l,self.xmm6.h)
    @_xmm6.setter
    def _xmm6(self,v):
        if type(v) == tuple:
            self.xmm6.l = v[0]
            self.xmm6.h = v[1]
        else:
            self.xmm6.l = v
            self.xmm6.h = 0

    @property
    def _xmm7(self):
        return (self.xmm7.l,self.xmm7.h)
    @_xmm7.setter
    def _xmm7(self,v):
        if type(v) == tuple:
            self.xmm7.l = v[0]
            self.xmm7.h = v[1]
        else:
            self.xmm7.l = v
            self.xmm7.h = 0

    #---------------------------------------------------------------
    
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
s64 = c_int64(r.r64).value
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
