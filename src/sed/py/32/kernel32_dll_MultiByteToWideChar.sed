/def __kernel32_dll_MultiByteToWideChar(/a \
    cin = cpu.get_dword_ptr(cpu._esp+8)\
    len = cpu.get_dword_ptr(cpu._esp+12)\
    out = cpu.get_dword_ptr(cpu._esp+16)\
    olen = cpu.get_dword_ptr(cpu._esp+20)\
    if olen != 0:\
        str = cpu.get_mem (cin,len)\
        cpu.set_unicode_ptr(out,str)\
    cpu._eax = len\
    cpu._esp += 4*6\
    return
